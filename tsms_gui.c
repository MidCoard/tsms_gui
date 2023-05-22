#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"
#include "tsms_lock.h"
#include "tsms_math.h"
#include "tsms_algorithm.h"
#include "tsms_printer.h"

TSMS_GRID_INFO TSMS_GUI_INVALID_GRID = {0, 0, 0, 0, 0, TSMS_STYLE_DISPLAY_BLOCK};

TSMS_INLINE long __tsms_internal_compare_z_index(void * a, void * b) {
	pGuiElement element1 = (pGuiElement) a;
	pGuiElement element2 = (pGuiElement) b;
	return element2->computedStyle.zIndex - element1->computedStyle.zIndex;
}

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

// compute style and merge all
TSMS_INLINE void __tsms_internal_request_render(pGuiElement element, bool parentFlag) {
	TSMS_STYLE style = element->computedStyle;
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_request_render(element->children->list[i], element->requestRender || parentFlag);
	if (parentFlag && style.position != TSMS_STYLE_POSITION_ABSOLUTE)
		element->requestRender = false;
}

TSMS_INLINE void __tsms_internal_check_render(pGuiElement element) {
	if (element->requestRender)
		return;
	if (!TSMS_STYLE_equals(element->lastStyle, element->computedStyle) || !TSMS_GUI_equalsGrid(element->lastGrid, element->grid)) {
		element->requestRender = true;
		return;
	}
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			__tsms_internal_check_render(child);
		}
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
	y = gui->display->screen->height - y;
	for (TSMS_POS i = 0; i < gui->touchableList->length; i++) {
		pGuiTouchableElement element = gui->touchableList->list[i];
		double now = TSMS_TIMER_now(TSMS_TIMER_getDefaultTimer());
		if (state == TSMS_TOUCH_STATE_RELEASE && TSMS_INT_LIST_contains(element->points, id)) {
			TSMS_INT_LIST_removeElement(element->points, id);
			if (element->releaseCallback != TSMS_NULL)
				element->releaseCallback(element, x, y, element->releaseHandler);
			if (element->press == TSMS_PRESS_STATE_PRESS)
				element->press = TSMS_PRESS_STATE_PRESS_AND_RELEASE;
			else
				element->press = 0;
			element->lastUpdate = now;
			break;
		}
		else if (TSMS_GUI_inGrid(element->grid, x, y) && TSMS_GUI_isInValidGrid(element)) {
			if (element->press == TSMS_PRESS_STATE_PRESS_AND_RELEASE && now - element->lastUpdate > TSMS_GUI_DOUBLE_PRESS_TIME)
				element->press = 0;
			if (state == TSMS_TOUCH_STATE_PRESS) {
				TSMS_INT_LIST_add(element->points, id);
				if (element->press == TSMS_PRESS_STATE_PRESS_AND_RELEASE) {
					if (element->doublePressCallback != TSMS_NULL)
						element->doublePressCallback(element, x, y, element->doublePressHandler);
					element->press = TSMS_PRESS_STATE_DOUBLE_PRESS;
				}
				if (element->press == 0) {
					if (element->pressCallback != TSMS_NULL)
						element->pressCallback(element, x, y, element->pressHandler);
					element->press = TSMS_PRESS_STATE_PRESS;
				}
			} else if (state == TSMS_TOUCH_STATE_LONG_PRESS) {
				if (element->longPressCallback != TSMS_NULL) {
					element->longPressCallback(element, x, y, element->longPressHandler);
					element->press = TSMS_PRESS_STATE_LONG_PRESS;
				}
			}
			element->lastUpdate = now;
			break;
		}
	}
}

TSMS_INLINE void __tsms_internal_cancel_previous_render(pGuiElement element, bool parentFlag, pLock lock) {
	if (element->requestRender || parentFlag) {
		TSMS_STYLE style = element->computedStyle;
		for (TSMS_POS i = 0; i < element->renderOperations->length; i++) {
			pRenderOperation operation = element->renderOperations->list[i];
			TSMS_GUI_cancelRenderOperation(element, operation, style, lock);
			TSMS_GUI_releaseRenderOperation(operation);
		}
		TSMS_LIST_clear(element->renderOperations);
	}
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_cancel_previous_render(element->children->list[i], element->requestRender || parentFlag, lock);
}

TSMS_INLINE void __tsms_internal_compute_style(pGuiElement element) {
	TSMS_STYLE_getStyle(element);
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_compute_style(element->children->list[i]);
}

bool TSMS_GUI_inGrid(TSMS_GRID_INFO grid, uint16_t x, uint16_t y) {
	return x >= grid.x && x < grid.x + grid.width && y <= grid.y && y > grid.y - grid.height;
}

void TSMS_GUI_defaultStyleCallback(pMutableStyle style, TSMS_STYLE data, void * handler) {
	pGuiElement element = (pGuiElement) handler;
	element->requestRender = true;
    if (style->style.zIndex != data.zIndex) {
        bool flag = false;
        uint16_t minZIndex = min(style->style.zIndex, data.zIndex);
        uint16_t maxZIndex = max(style->style.zIndex, data.zIndex);
		pGui gui = TSMS_GUI_getGui(element);
	    __tsms_internal_compute_style(gui);
	    TSMS_ALGORITHM_sort(gui->touchableList, __tsms_internal_compare_z_index);
        for (TSMS_POS i = 0; i < element->gui->list->length; i++) {
            pGuiElement child = element->gui->list->list[i];
            if (child == element)
                continue;
            TSMS_STYLE childStyle = child->computedStyle;
            if (TSMS_between(minZIndex, childStyle.zIndex, maxZIndex)) {
                flag = true;
                break;
            }
        }
        if (flag) {
            for (TSMS_POS i = 0; i < element->gui->list->length; i++) {
                pGuiElement child = element->gui->list->list[i];
                if (child == element)
                    continue;
                TSMS_STYLE childStyle = child->computedStyle;
                if (childStyle.zIndex >= minZIndex)
                    child->requestRender = true;
            }
        }
    }
}

pGui TSMS_GUI_getGui(pGuiElement element) {
	if (element == TSMS_NULL)
		return TSMS_NULL;
	if (element->gui != TSMS_NULL)
		return element->gui;
	return TSMS_GUI_getGui(element->parent);
}

bool TSMS_GUI_isInvalidGrid(TSMS_GRID_INFO grid) {
	return grid.width == 0 && grid.displayType == TSMS_STYLE_DISPLAY_BLOCK;
}

bool TSMS_GUI_isInValidGrid(pGuiElement element) {
	if (element == TSMS_NULL)
		return false;
	if (TSMS_GUI_isInvalidGrid(element->grid))
		return false;
	if (element->parent == TSMS_NULL)
		return true;
	return TSMS_GUI_isInValidGrid(element->parent);
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
	} else {
		uint16_t screenWidth = TSMS_GUI_getGui(element)->display->screen->width;
		if (boxWidth + x > screenWidth || y - boxHeight < 0)
			return TSMS_GUI_INVALID_GRID;
		return (TSMS_GRID_INFO) {x, y, boxWidth, boxHeight, style.zIndex, TSMS_STYLE_DISPLAY_BLOCK};
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
	TSMS_STYLE style = TSMS_STYLE_DEFAULT;
	style.width = display->screen->width;
	style.height = display->screen->height;
	gui->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(gui->style, TSMS_GUI_defaultStyleCallback, gui);
	gui->lastStyle = style;
	gui->computedStyle = style;
	gui->requestRender = true;
	gui->grid = TSMS_GUI_INVALID_GRID;
	gui->lastGrid = TSMS_GUI_INVALID_GRID;
	gui->gui = gui;
	gui->level = 0;
	gui->renderOperations = TSMS_LIST_create(10);
	gui->ignoreInvalidGrid = true;

	gui->display = display;
	gui->list = TSMS_LIST_create(10);
	gui->touchableList = TSMS_LIST_create(10);
	gui->lock = TSMS_LOCK_create();
	TSMS_TOUCH_setCallback(display->touch, __tsms_internal_touch_callback, gui);
	return gui;
}

TSMS_RESULT TSMS_GUI_release(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_TOUCH_setCallback(gui->display->touch, TSMS_NULL, TSMS_NULL);
	TSMS_LIST_release(gui->list);
	TSMS_LIST_release(gui->touchableList);
	TSMS_LOCK_release(gui->lock);
	return TSMS_GUI_releaseGuiElement(gui);
}

TSMS_RESULT TSMS_GUI_add(pGuiElement parent, pGuiElement element) {
	if (parent == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	if (parent->children == TSMS_NULL)
		return TSMS_ERROR;
	pGui gui = TSMS_GUI_getGui(parent);
	if (gui != TSMS_NULL) {
		TSMS_LOCK_lock(gui->lock);
		element->parent = parent;
		element->gui = gui;
		element->level = parent->level + 1;
		TSMS_RESULT result = TSMS_LIST_add(parent->children, element);
		TSMS_LIST_clear(gui->list);
		TSMS_LOCK_unlock(gui->lock);
		return result;
	}
	element->parent = parent;
	element->gui = gui;
	element->level = parent->level + 1;
	return TSMS_LIST_add(parent->children, element);
}

TSMS_RESULT TSMS_GUI_remove(pGuiElement parent, pGuiElement element) {
	if (parent == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	if (parent->children == TSMS_NULL)
		return TSMS_ERROR;
	pGui gui = TSMS_GUI_getGui(parent);
	if (gui != TSMS_NULL) {
		TSMS_LOCK_lock(gui->lock);
		TSMS_RESULT result = TSMS_LIST_removeElement(parent->children, element) == -1 ? TSMS_FAIL : TSMS_SUCCESS;
		TSMS_LIST_clear(gui->list);
		TSMS_LOCK_unlock(gui->lock);
		return result;
	}
	return TSMS_LIST_removeElement(parent->children, element) == -1 ? TSMS_FAIL : TSMS_SUCCESS;
}

TSMS_RESULT TSMS_GUI_draw(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	if (gui->list->length == 0)
		__tsms_internal_add_render_entity(gui, gui);
	TSMS_LOCK_lock(gui->lock);
	gui->preRender(gui, 0, gui->display->screen->height, gui->display->screen->width, gui->display->screen->height);
	__tsms_internal_check_render(gui);
	__tsms_internal_request_render(gui, false);
	pLock lock = TSMS_SEQUENCE_PRIORITY_LOCK_lock(gui->display->screen->lock, TSMS_NULL, 0);
	__tsms_internal_cancel_previous_render(gui, false, lock);
	TSMS_ALGORITHM_sort(gui->list, __tsms_internal_compare_grid_render_info);
	TSMS_SIZE renderRange;
	for (renderRange = 0; renderRange < gui->list->length; renderRange++) {
		pGuiElement element = gui->list->list[renderRange];
		if (!element->requestRender)
			break;
	}
	if (lock != TSMS_NULL) {
		for (TSMS_POS i = 0; i < renderRange; i++) {
			pGuiElement element = gui->list->list[i];
			element->render(element, lock);
		}
		TSMS_SEQUENCE_PRIORITY_LOCK_unlock(gui->display->screen->lock, lock);
	}
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	TSMS_LOCK_unlock(gui->lock);
	return result;
}


// do not check grid is valid, because it must be valid to call this function
TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element, TSMS_STYLE style, pLock lock) {
	TSMS_COLOR parentColor = element->parent == TSMS_NULL ? *style.backgroundColor : *element->parent->computedStyle.backgroundColor;
	TSMS_GRID_INFO grid = element->grid;

	if (!TSMS_UTIL_equalsColor(*style.margin.color,parentColor)) {
		// render margin
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, grid.x, grid.y, grid.width,
		                            style.margin.top,
		                            *style.margin.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x, grid.y, grid.width, style.margin.top));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, grid.x,
		                            grid.y - grid.height + style.margin.bottom,
		                            grid.width, style.margin.bottom, *style.margin.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x, grid.y - grid.height + style.margin.bottom,
		                                                     grid.width,
		                                                     style.margin.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, grid.x, grid.y - style.margin.top,
		                            style.margin.left,
		                            grid.height - style.margin.top - style.margin.bottom, *style.margin.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x, grid.y - style.margin.top, style.margin.left,
		                                                     grid.height - style.margin.top - style.margin.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, grid.x + grid.width - style.margin.right,
		                            grid.y - style.margin.top, style.margin.right,
		                            grid.height - style.margin.top - style.margin.bottom, *style.margin.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + grid.width - style.margin.right,
		                                                     grid.y - style.margin.top, style.margin.right,
		                                                     grid.height - style.margin.top - style.margin.bottom));
	}

	if (!TSMS_UTIL_equalsColor(*style.border.color,parentColor)) {
		// render border
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, grid.x + style.margin.left,
		                            grid.y - style.margin.top, grid.width - style.margin.left - style.margin.right,
		                            style.border.top, *style.border.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left, grid.y - style.margin.top,
		                                                     grid.width - style.margin.left - style.margin.right,
		                                                     style.border.top));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left,
		                            grid.y - grid.height + style.margin.bottom + style.border.bottom,
		                            grid.width - style.margin.left - style.margin.right,
		                            style.border.bottom, *style.border.color, lock);
		TSMS_LIST_add(element->renderOperations, TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left,
		                                                                                grid.y - grid.height +
		                                                                                style.margin.bottom +
		                                                                                style.border.bottom,
		                                                                                grid.width - style.margin.left -
		                                                                                style.margin.right,
		                                                                                style.border.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left,
		                            grid.y - style.margin.top,
		                            style.border.left,
		                            grid.height - style.margin.top - style.margin.bottom, *style.border.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left, grid.y - style.margin.top,
		                                                     style.border.left,
		                                                     grid.height - style.margin.top - style.margin.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + grid.width - style.margin.right - style.border.right,
		                            grid.y - style.margin.top,
		                            style.border.right,
		                            grid.height - style.margin.top - style.margin.bottom, *style.border.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(
				              grid.x + grid.width - style.margin.right - style.border.right,
				              grid.y - style.margin.top, style.border.right,
				              grid.height - style.margin.top - style.margin.bottom));
	}


	if (!TSMS_UTIL_equalsColor(*style.border.color,parentColor)) {
		// render padding
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left + style.border.left,
		                            grid.y - style.margin.top - style.border.top,
		                            grid.width - style.margin.left - style.margin.right - style.border.left -
		                            style.border.right,
		                            style.padding.top, *style.padding.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left + style.border.left,
		                                                     grid.y - style.margin.top - style.border.top,
		                                                     grid.width - style.margin.left - style.margin.right -
		                                                     style.border.left - style.border.right,
		                                                     style.padding.top));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left + style.border.left,
		                            grid.y - grid.height + style.margin.bottom + style.border.bottom +
		                            style.padding.bottom,
		                            grid.width - style.margin.left - style.margin.right - style.border.left -
		                            style.border.right,
		                            style.padding.bottom, *style.padding.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left + style.border.left,
		                                                     grid.y - grid.height + style.margin.bottom +
		                                                     style.border.bottom + style.padding.bottom,
		                                                     grid.width - style.margin.left - style.margin.right -
		                                                     style.border.left - style.border.right,
		                                                     style.padding.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left + style.border.left,
		                            grid.y - style.margin.top - style.border.top,
		                            style.padding.left,
		                            grid.height - style.margin.top - style.margin.bottom - style.border.top -
		                            style.border.bottom,
		                            *style.padding.color, lock);
		TSMS_LIST_add(element->renderOperations,
		              TSMS_GUI_createFillRectRenderOperation(grid.x + style.margin.left + style.border.left,
		                                                     grid.y - style.margin.top - style.border.top,
		                                                     style.padding.left,
		                                                     grid.height - style.margin.top - style.margin.bottom -
		                                                     style.border.top - style.border.bottom));
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + grid.width - style.margin.right - style.border.right - style.padding.right,
		                            grid.y - style.margin.top - style.border.top,
		                            style.padding.right,
		                            grid.height - style.margin.top - style.margin.bottom - style.border.top -
		                            style.border.bottom,
		                            *style.padding.color, lock);
		TSMS_LIST_add(element->renderOperations, TSMS_GUI_createFillRectRenderOperation(
				grid.x + grid.width - style.margin.right - style.border.right - style.padding.right,
				grid.y - style.margin.top - style.border.top, style.padding.right,
				grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom));
	}
	if (!TSMS_UTIL_equalsColor(parentColor, *style.backgroundColor)) {
		// render background color
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen,
		                            grid.x + style.margin.left + style.border.left + style.padding.left,
		                            grid.y - style.margin.top - style.border.top - style.padding.top,
		                            grid.width - style.margin.left - style.margin.right - style.border.left -
		                            style.border.right - style.padding.left - style.padding.right,
		                            grid.height - style.margin.top - style.margin.bottom - style.border.top -
		                            style.border.bottom - style.padding.top - style.padding.bottom,
		                            *style.backgroundColor, lock);
		TSMS_LIST_add(element->renderOperations, TSMS_GUI_createFillRectRenderOperation(
				grid.x + style.margin.left + style.border.left + style.padding.left,
				grid.y - style.margin.top - style.border.top - style.padding.top,
				grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right -
				style.padding.left - style.padding.right,
				grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom -
				style.padding.top - style.padding.bottom));
	}
	return TSMS_SUCCESS;
}

bool TSMS_GUI_equalsGrid(TSMS_GRID_INFO grid1, TSMS_GRID_INFO grid2) {
	return grid1.x == grid2.x && grid1.y == grid2.y && grid1.width == grid2.width && grid1.height == grid2.height;
}

pRenderOperation TSMS_GUI_createCharRenderOperation(uint16_t x, uint16_t y, TSMS_FONT_TYPE fontType, void *font, char c, TSMS_FONT_SIZE size) {
	pRenderOperation operation = malloc(sizeof(tRenderOperation));
	operation->type = TSMS_RENDER_OPERATION_TYPE_CHAR;
	operation->size = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(TSMS_FONT_TYPE) + sizeof(void *) + sizeof(char) + sizeof(TSMS_FONT_SIZE);
	operation->data = malloc(operation->size);
	uint16_t offset = 0;
	memcpy(operation->data + offset, &x, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(operation->data + offset, &y, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(operation->data + offset, &fontType, sizeof(TSMS_FONT_TYPE));
	offset += sizeof(TSMS_FONT_TYPE);
	memcpy(operation->data + offset, &font, sizeof(void *));
	offset += sizeof(void *);
	memcpy(operation->data + offset, &c, sizeof(char));
	offset += sizeof(char);
	memcpy(operation->data + offset, &size, sizeof(TSMS_FONT_SIZE));
	return operation;
}

pRenderOperation TSMS_GUI_createFillRectRenderOperation(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	pRenderOperation operation = malloc(sizeof(tRenderOperation));
	operation->type = TSMS_RENDER_OPERATION_TYPE_FILL_RECT;
	operation->size = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t);
	operation->data = malloc(operation->size);
	uint16_t offset = 0;
	memcpy(operation->data + offset, &x, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(operation->data + offset, &y, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(operation->data + offset, &width, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(operation->data + offset, &height, sizeof(uint16_t));
	return operation;
}

TSMS_RESULT TSMS_GUI_cancelRenderOperation(pGuiElement element, pRenderOperation operation, TSMS_STYLE style, pLock lock) {
	if (operation == TSMS_NULL)
		return TSMS_ERROR;
	uint16_t offset = 0;
	TSMS_COLOR parentColor = element->parent == TSMS_NULL ? *style.backgroundColor : *element->parent->computedStyle.backgroundColor;
	if (operation->type == TSMS_RENDER_OPERATION_TYPE_CHAR) {
		uint16_t x = *((uint16_t *) (operation->data + offset));
		offset += sizeof(uint16_t);
		uint16_t y = *((uint16_t *) (operation->data + offset));
		offset += sizeof(uint16_t);
		TSMS_FONT_TYPE fontType = *((TSMS_FONT_TYPE *) (operation->data + offset));
		offset += sizeof(TSMS_FONT_TYPE);
		void *font = *((void **) (operation->data + offset));
		offset += sizeof(void *);
		char c = *((char *) (operation->data + offset));
		offset += sizeof(char);
		TSMS_FONT_SIZE size = *((TSMS_FONT_SIZE *) (operation->data + offset));
		TSMS_SCREEN_drawCharTopLeft(TSMS_GUI_getGui(element)->display->screen, x, y, fontType, font, c, *style.backgroundColor, size, lock);
	} else if (operation->type == TSMS_RENDER_OPERATION_TYPE_FILL_RECT) {
        uint16_t x = *((uint16_t *) (operation->data + offset));
        offset += sizeof(uint16_t);
        uint16_t y = *((uint16_t *) (operation->data + offset));
        offset += sizeof(uint16_t);
        uint16_t width = *((uint16_t *) (operation->data + offset));
        offset += sizeof(uint16_t);
        uint16_t height = *((uint16_t *) (operation->data + offset));
        TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGui(element)->display->screen, x, y, width, height, parentColor, lock);
    }
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_GUI_releaseRenderOperation(pRenderOperation operation) {
	if (operation == TSMS_NULL)
		return TSMS_ERROR;
	free(operation->data);
	free(operation);
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_GUI_releaseGuiElement(pGuiElement element) {
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_LIST_release(element->children);
	TSMS_LIST_release(element->renderOperations);
	TSMS_MUTABLE_STYLE_release(element->style);
	free(element);
	return TSMS_SUCCESS;
}