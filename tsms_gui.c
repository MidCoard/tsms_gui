#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"
#include "tsms_lock.h"
#include "tsms_math.h"
#include "tsms_algorithm.h"
#include "tsms_printer.h"
#include "tsms_component_container.h"

TSMS_GRID_INFO TSMS_GUI_INVALID_GRID = {0, 0, 0, 0, 0, TSMS_STYLE_DISPLAY_BLOCK};

TSMS_INLINE long __tsms_internal_compare_grid_render_info(void * a, void * b) {
	pGuiElement element1 = (pGuiElement) a;
	pGuiElement element2 = (pGuiElement) b;
	if (element1->requestRender != element2->requestRender)
		// true requestRender should be rendered first
		return element1->requestRender ? -1 : 1;
	if (element1->grid.zIndex != element2->grid.zIndex)
		// high zIndex should be rendered first
		return element2->grid.zIndex - element1->grid.zIndex;
	if (element1->level != element2->level)
		return element1->level - element2->level;
	return element1->grid.x - element2->grid.x;
}

TSMS_INLINE void __tsms_internal_request_render(pGuiElement element, bool parentFlag, uint8_t zIndex) {
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_request_render(element->children->list[i], element->requestRender || parentFlag, max(zIndex, style.zIndex));
	if (parentFlag && style.zIndex <= zIndex)
		element->requestRender = false;
}

TSMS_INLINE void __tsms_internal_force_render(pGuiElement element) {
	element->forceRender = true;
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_force_render(element->children->list[i]);
}

TSMS_INLINE void __tsms_internal_check_render(pGuiElement element) {
	if (element->requestRender)
		return;
	if (!TSMS_STYLE_equals(element->lastStyle, element->computedStyle) || !TSMS_GUI_equalsGrid(element->lastGrid, element->grid)) {
		element->requestRender = true;
		return;
	}
	bool childGridChanged = false;
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			if (!TSMS_GUI_equalsGrid(child->lastGrid, child->grid))
				childGridChanged = true;
			__tsms_internal_check_render(child);
		}
	__tsms_internal_force_render(element);
	if (childGridChanged)
		element->requestRender = true;
}

TSMS_INLINE void __tsms_internal_add_render_entity(pGui gui, pGuiElement element) {
	TSMS_LIST_add(gui->list, element);
	if (element->type == TSMS_GUI_TYPE_TOUCHABLE)
		TSMS_LIST_add(gui->touchableList, element);
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_add_render_entity(gui, element->children->list[i]);
}

TSMS_INLINE void __tsms_internal_touch_callback(TSMS_THP touch, uint8_t id, uint16_t x, uint16_t y, uint16_t size, TSMS_TOUCH_STATE state, void * handler) {
	pGui gui = handler;
	print("touch callback: %d, %d, %d, %d, %d, %d\n", touch, id, x, y, size, state);
	y = gui->display->screen->height - y;
	for (TSMS_POS i = 0; i < gui->touchableList->length; i++) {
		pGuiTouchableElement element = gui->touchableList->list[i];
		double now = TSMS_TIMER_now(TSMS_TIMER_getDefaultTimer());
		if (state == TSMS_TOUCH_STATE_RELEASE && TSMS_INT_LIST_contains(element->points, id)) {
			TSMS_INT_LIST_removeElement(element->points, id);
			if (element->releaseCallback != TSMS_NULL)
				element->releaseCallback(element, element->releaseHandler);
			if (element->press == TSMS_PRESS_STATE_PRESS)
				element->press = TSMS_PRESS_STATE_PRESS_AND_RELEASE;
			else
				element->press = 0;
			element->lastUpdate = now;
		}
		else if (TSMS_GUI_inGrid(element->grid, x, y)) {
			if (element->press == TSMS_PRESS_STATE_PRESS_AND_RELEASE && now - element->lastUpdate > TSMS_GUI_DOUBLE_PRESS_TIME)
				element->press = 0;
			if (state == TSMS_TOUCH_STATE_PRESS) {
				TSMS_INT_LIST_add(element->points, id);
				if (element->press == TSMS_PRESS_STATE_PRESS_AND_RELEASE) {
					if (element->doublePressCallback != TSMS_NULL)
						element->doublePressCallback(element, element->doublePressHandler);
					element->press = TSMS_PRESS_STATE_DOUBLE_PRESS;
				}
				if (element->press == 0) {
					if (element->pressCallback != TSMS_NULL)
						element->pressCallback(element, element->pressHandler);
					element->press = TSMS_PRESS_STATE_PRESS;
				}
			} else if (state == TSMS_TOUCH_STATE_LONG_PRESS) {
				if (element->longPressCallback != TSMS_NULL) {
					element->longPressCallback(element, element->longPressHandler);
					element->press = TSMS_PRESS_STATE_LONG_PRESS;
				}
			}
			element->lastUpdate = now;
		}
	}
}

bool TSMS_GUI_inGrid(TSMS_GRID_INFO grid, uint16_t x, uint16_t y) {
	return x >= grid.x && x < grid.x + grid.width && y <= grid.y && y > grid.y - grid.height;
}

void TSMS_GUI_defaultStyleUpdateCallback(pMutableStyle style, TSMS_STYLE data, void * handler) {
	pGuiElement element = (pGuiElement) handler;
	element->requestRender = true;
}

pGui TSMS_GUI_getGUI(pGuiElement element) {
	if (element == TSMS_NULL)
		return TSMS_NULL;
	if (element->gui != TSMS_NULL)
		return element->gui;
	return TSMS_GUI_getGUI(element->parent);
}

bool TSMS_GUI_isInvalidGrid(TSMS_GRID_INFO grid) {
	return grid.width == 0 && grid.displayType == TSMS_STYLE_DISPLAY_BLOCK;
}

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, TSMS_STYLE style, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight) {
	if (element == TSMS_NULL)
		return TSMS_GUI_INVALID_GRID;
	if (style.display == TSMS_STYLE_DISPLAY_NONE)
		return (TSMS_GRID_INFO) {x, y, 0, 0, 0, TSMS_STYLE_DISPLAY_NONE};
	boxWidth += TSMS_STYLE_X_ATTACHMENT(style);
	boxHeight += TSMS_STYLE_Y_ATTACHMENT(style);
	if (style.position == TSMS_STYLE_POSITION_RELATIVE) {
		if (boxWidth > parentWidth || boxHeight > parentHeight)
			return TSMS_GUI_INVALID_GRID;
		return (TSMS_GRID_INFO) {x, y, boxWidth, boxHeight, style.zIndex, TSMS_STYLE_DISPLAY_BLOCK};
	}
	else {
		uint16_t top = style.top;
		uint16_t left = style.left;
		uint16_t screenWidth = TSMS_GUI_getGUI(element)->display->screen->width;
		uint16_t screenHeight = TSMS_GUI_getGUI(element)->display->screen->height;
		if (top == TSMS_STYLE_AUTO)
			top = screenHeight;
		if (left == TSMS_STYLE_AUTO)
			left = 0;
		if (boxWidth + left > screenWidth || top - boxHeight < 0)
			return TSMS_GUI_INVALID_GRID;
		return (TSMS_GRID_INFO) {left, top, boxWidth, boxHeight, style.zIndex, TSMS_STYLE_DISPLAY_BLOCK};
	}
}

pGui TSMS_GUI_create(TSMS_DPHP display) {
	pGui gui = (pGui) malloc(sizeof(tGui));
	if (gui == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for gui");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	gui->type = TSMS_GUI_TYPE_GUI;
	gui->preRender = TSMS_CONTAINER_preRender;
	gui->render = TSMS_CONTAINER_render;
	gui->parent = TSMS_NULL;
	gui->children = TSMS_LIST_create(10);
	gui->style = TSMS_MUTABLE_STYLE_create(TSMS_STYLE_DEFAULT);
	TSMS_MUTABLE_STYLE_setCallback(gui->style, TSMS_GUI_defaultStyleUpdateCallback, gui);
	gui->lastStyle = TSMS_STYLE_DEFAULT;
	gui->computedStyle = TSMS_STYLE_DEFAULT;
	gui->requestRender = true;
	gui->forceRender = true;
	gui->grid = TSMS_GUI_INVALID_GRID;
	gui->lastGrid = TSMS_GUI_INVALID_GRID;
	gui->gui = gui;
	gui->level = 0;

	gui->display = display;
	gui->list = TSMS_LIST_create(10);
	gui->touchableList = TSMS_LIST_create(10);
	gui->lock = TSMS_LOCK_create();
	TSMS_TOUCH_setCallback(display->touch, __tsms_internal_touch_callback, gui);
	return gui;
}

TSMS_RESULT TSMS_GUI_add(pGuiElement parent, pGuiElement element) {
	if (parent == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	if (parent->children == TSMS_NULL)
		return TSMS_ERROR;
	pGui gui = TSMS_GUI_getGUI(parent);
	element->parent = parent;
	element->gui = gui;
	element->level = parent->level + 1;
	return TSMS_LIST_add(parent->children, element);
}

TSMS_RESULT TSMS_GUI_draw(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	if (gui->list->length == 0)
		__tsms_internal_add_render_entity(gui, gui);
	TSMS_LOCK_lock(gui->lock);
	__tsms_internal_request_render(gui, false, 0);
	gui->preRender(gui, 0, gui->display->screen->height, gui->display->screen->width, gui->display->screen->height);
	__tsms_internal_check_render(gui);
	TSMS_ALGORITHM_sort(gui->list, __tsms_internal_compare_grid_render_info);
	TSMS_SIZE renderRange;
	for (renderRange = 0; renderRange < gui->list->length; renderRange++) {
		pGuiElement element = gui->list->list[renderRange];
		if (!element->requestRender)
			break;
	}
	pLock lock = TSMS_SEQUENCE_PRIORITY_LOCK_lock(gui->display->screen->lock, TSMS_NULL, 0);
	if (lock != TSMS_NULL) {
		for (TSMS_POS i = 0; i < renderRange; i++) {
			pGuiElement element = gui->list->list[i];
			if (element->requestRender) {
				print("RENDER: %d\n" , element->type);
				element->render(element, lock);
			}
		}
		TSMS_SEQUENCE_PRIORITY_LOCK_unlock(gui->display->screen->lock, lock);
	}
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	TSMS_LOCK_unlock(gui->lock);
	return result;
}


// do not check grid is valid, because it must be valid to call this function
TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element, TSMS_STYLE style, pLock lock) {
	TSMS_GRID_INFO grid = element->grid;
	bool ignoreMargin = false;
	bool ignoreBorder = false;
	bool ignorePadding = false;
	bool ignoreBackground = false;

	// lack z index support
	if (TSMS_GUI_equalsGrid(grid, element->lastGrid)) {
		if (TSMS_STYLE_equals(style, element->lastStyle)) {
			ignoreMargin = true;
			ignoreBorder = true;
			ignorePadding = true;
		}
	}

	if (element->forceRender) {
		ignoreMargin = false;
		ignoreBorder = false;
		ignorePadding = false;
		ignoreBackground = false;
	}
	// render margin
	if (!ignoreMargin) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y, grid.width, style.margin.top,
		                     *style.margin.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y - grid.height + style.margin.bottom,
		                     grid.width, style.margin.bottom, *style.margin.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y - style.margin.top, style.margin.left,
		                     grid.height - style.margin.top - style.margin.bottom, *style.margin.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x + grid.width - style.margin.right,
		                     grid.y - style.margin.top, style.margin.right,
		                     grid.height - style.margin.top - style.margin.bottom, *style.margin.color, lock);
	}

	// render border
	if (!ignoreBorder) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x + style.margin.left,
		                     grid.y - style.margin.top, grid.width - style.margin.left - style.margin.right,
		                     style.border.top, *style.border.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left,
		                     grid.y - grid.height + style.margin.bottom + style.border.bottom,
		                     grid.width - style.margin.left - style.margin.right,
		                     style.border.bottom, *style.border.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left,
		                     grid.y - style.margin.top,
		                     style.border.left,
		                     grid.height - style.margin.top - style.margin.bottom, *style.border.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + grid.width - style.margin.right - style.border.right,
		                     grid.y - style.margin.top,
		                     style.border.right,
		                     grid.height - style.margin.top - style.margin.bottom, *style.border.color, lock);
	}

	if (!ignorePadding) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - style.margin.top - style.border.top,
		                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right,
		                     style.padding.top, *style.padding.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - grid.height + style.margin.bottom + style.border.bottom + style.padding.bottom,
		                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right,
		                     style.padding.bottom, *style.padding.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - style.margin.top - style.border.top,
		                     style.padding.left,
		                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom,
		                     *style.padding.color, lock);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + grid.width - style.margin.right - style.border.right - style.padding.right,
		                     grid.y - style.margin.top - style.border.top,
		                     style.padding.right,
		                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom,
		                     *style.padding.color, lock);
	}

	// render background color
	if (!ignoreBackground)
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left + style.padding.left,
		                     grid.y - style.margin.top - style.border.top - style.padding.top,
		                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right - style.padding.left - style.padding.right,
		                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom - style.padding.top - style.padding.bottom,
		                     *style.backgroundColor, lock);
	return TSMS_SUCCESS;
}

bool TSMS_GUI_equalsGrid(TSMS_GRID_INFO grid1, TSMS_GRID_INFO grid2) {
	return grid1.x == grid2.x && grid1.y == grid2.y && grid1.width == grid2.width && grid1.height == grid2.height;
}