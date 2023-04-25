#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"
#include "tsms_lock.h"
#include "tsms_math.h"
#include "tsms_algorithm.h"

TSMS_GRID_INFO TSMS_GUI_INVALID_GRID = {TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, 0};

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

TSMS_INLINE uint16_t TSMS_STYLE_elementWidth(TSMS_STYLE style, uint16_t parentWidth) {
	uint16_t maxWidth = style.maxWidth == TSMS_STYLE_AUTO ? parentWidth - TSMS_STYLE_X_ATTACHMENT(style) : style.maxWidth;
	return min(maxWidth, parentWidth - TSMS_STYLE_X_ATTACHMENT(style));
}

TSMS_INLINE uint16_t TSMS_STYLE_elementHeight(TSMS_STYLE style, uint16_t parentHeight) {
	uint16_t maxHeight = style.maxHeight == TSMS_STYLE_AUTO ? parentHeight - TSMS_STYLE_Y_ATTACHMENT(style) : style.maxHeight;
	return min(maxHeight, parentHeight - TSMS_STYLE_Y_ATTACHMENT(style));
}

TSMS_INLINE void __tsms_internal_request_render(pGuiElement element, bool parentFlag, uint8_t zIndex) {
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_request_render(element->children->list[i], element->requestRender || parentFlag, max(zIndex, style.zIndex));
	if (parentFlag && style.zIndex <= zIndex)
		element->requestRender = false;
	TSMS_LIST_add(TSMS_GUI_getGUI(element)->list, element);
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

TSMS_RESULT TSMS_GUI_defaultRender(pGuiElement element) {
	TSMS_STYLE style = element->computedStyle;
	TSMS_GUI_renderStyle(element, style);

	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			child->render(child);
		}

	element->requestRender = false;
	element->firstRender = false;
	element->lastGrid = element->grid;
	element->lastStyle = style;
	return TSMS_SUCCESS;
}

TSMS_GRID_INFO TSMS_GUI_defaultPreRender(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	TSMS_STYLE style = element->computedStyle;
	if (element->children == TSMS_NULL || element->children->length == 0) {
		uint16_t width = TSMS_STYLE_getBoxWidth(style, 0, parentWidth);
		uint16_t height = TSMS_STYLE_getBoxHeight(style, 0, parentHeight);
		// no more children to render, return the grid
		return element->grid = TSMS_GUI_calcGrid(element, style, x, y, width, height, parentWidth, parentHeight);
	}
	pGuiElement child = element->children->list[0];
	TSMS_STYLE childStyle = child->computedStyle;
	uint16_t elementWidth = TSMS_STYLE_elementWidth(style, parentWidth);
	uint16_t elementHeight = TSMS_STYLE_elementHeight(style, parentHeight); // not include attachment
	uint16_t boxWidth = 0;
	uint16_t boxHeight = 0;
	if (style.gridType == TSMS_GRID_TYPE_DEFAULT) {
		TSMS_GRID_INFO result = child->preRender(child, x + TSMS_STYLE_left(style), y - TSMS_STYLE_top(style), elementWidth, elementHeight);
		if (result.x == TSMS_STYLE_AUTO && childStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
			// if the first child is absolute, don't care the result, because invalid grid will not render
			uint16_t width = TSMS_STYLE_getBoxWidth(style, 0, parentWidth);
			uint16_t height = TSMS_STYLE_getBoxHeight(style, 0, parentHeight);
			// the first child is relative and render failed, return the grid
			return element->grid = TSMS_GUI_calcGrid(element,style, x, y, width, height, parentWidth, parentHeight);
		}
		if (result.x == TSMS_STYLE_AUTO) {
			result.x = x;
			result.y = y;
			result.width = 0;
			result.height = 0;
			result.zIndex = 0;
		}
		uint16_t maxWidth = 0;
		uint16_t currentRowWidth = result.width;
		uint16_t maxHeight = result.height;
		uint16_t currentColumnHeight = 0;
		for (TSMS_POS i = 1; i < element->children->length; i++) {
			child = element->children->list[i];
			result = child->preRender(child, result.x + result.width + TSMS_STYLE_left(style), result.y -
					TSMS_STYLE_top(style), elementWidth - currentRowWidth, elementHeight - currentColumnHeight);
			childStyle = child->computedStyle;
			if (result.x == TSMS_STYLE_AUTO && childStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
				// means TSMS_POSITION_RELATIVE
				currentColumnHeight += maxHeight;
				maxHeight = 0;
				maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
				currentRowWidth = 0;
				result = child->preRender(child, x + TSMS_STYLE_left(style), y - currentColumnHeight -
						TSMS_STYLE_top(style), elementWidth - currentRowWidth, elementHeight - currentColumnHeight);
				if (result.x == TSMS_STYLE_AUTO)
					break;
				else {
					// success
					currentRowWidth += result.width;
					maxHeight = result.height > maxHeight ? result.height : maxHeight;
				}
			} else if (childStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
				// success
				currentRowWidth += result.width;
				maxHeight = result.height > maxHeight ? result.height : maxHeight;
			}
			// ignore when position is absolute
		}
		if (currentRowWidth > 0)
			maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
		boxWidth = maxWidth;
		boxHeight = maxHeight + currentColumnHeight;
	}
	return element->grid = TSMS_GUI_calcGrid(element, style, x, y, boxWidth, boxHeight, parentWidth, parentHeight);
}

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, TSMS_STYLE style, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight) {
	if (element == TSMS_NULL)
		return TSMS_GUI_INVALID_GRID;
	boxWidth += TSMS_STYLE_X_ATTACHMENT(style);
	boxHeight += TSMS_STYLE_Y_ATTACHMENT(style);
	if (style.position == TSMS_STYLE_POSITION_RELATIVE) {
		if (boxWidth > parentWidth || boxHeight > parentHeight)
			return TSMS_GUI_INVALID_GRID;
		return (TSMS_GRID_INFO) {x, y, boxWidth, boxHeight, style.zIndex};
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
		return (TSMS_GRID_INFO) {left, top, boxWidth, boxHeight, style.zIndex};
	}
}

void TSMS_GUI_addRenderEntity(pGui gui, pGuiElement element) {
	TSMS_LIST_add(gui->list, element);
}

pGui TSMS_GUI_create(TSMS_DPHP display) {
	pGui gui = (pGui) malloc(sizeof(tGui));
	if (gui == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for gui");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	gui->preRender = TSMS_GUI_defaultPreRender;
	gui->render = TSMS_GUI_defaultRender;
	gui->parent = TSMS_NULL;
	gui->children = TSMS_LIST_create(10);
	gui->style = TSMS_MUTABLE_STYLE_create(TSMS_STYLE_DEFAULT);
	TSMS_MUTABLE_STYLE_setSetterCallback(gui->style, TSMS_GUI_defaultStyleUpdateCallback, gui);
	gui->lastStyle = TSMS_STYLE_DEFAULT;
	gui->computedStyle = TSMS_STYLE_DEFAULT;
	gui->requestRender = true;
	gui->firstRender = true;
	gui->grid = TSMS_GUI_INVALID_GRID;
	gui->lastGrid = TSMS_GUI_INVALID_GRID;
	gui->gui = gui;
	gui->level = 0;

	gui->display = display;
	gui->lock = TSMS_LOCK_create();
	gui->list = TSMS_LIST_create(10);
	TSMS_LIST_add(gui->list, gui);
//	TSMS_TOUCH_setCallback(gui->display->touch, )
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
	TSMS_LOCK_lock(gui->lock);
	TSMS_LIST_clear(gui->list);
	__tsms_internal_request_render(gui, false, 0);
	gui->preRender(gui, 0, gui->display->screen->height, gui->display->screen->width, gui->display->screen->height);
	TSMS_ALGORITHM_sort(gui->list, __tsms_internal_compare_grid_render_info);
	TSMS_SIZE renderRange;
	for (renderRange = 0; renderRange < gui->list->length; renderRange++) {
		pGuiElement element = gui->list->list[renderRange];
		if (!element->requestRender)
			break;
	}
	for (TSMS_POS i = 0; i < renderRange; i++) {
		pGuiElement element = gui->list->list[i];
		if (element->requestRender)
			element->render(element);
	}
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	TSMS_LOCK_unlock(gui->lock);
	return result;
}

TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element, TSMS_STYLE style) {
	TSMS_GRID_INFO grid = element->grid;
	bool ignoreMargin = false;
	bool ignoreBorder = false;
	bool ignorePadding = false;

	// lack z index support

	if (element->firstRender) {
		ignoreMargin = false;
		ignoreBorder = false;
		ignorePadding = false;
	}
	// render margin
	if (!ignoreMargin) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y, grid.width, style.margin.top,
		                     style.margin.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y - grid.height + style.margin.bottom,
		                     grid.width, style.margin.bottom, style.margin.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x, grid.y - style.margin.top, style.margin.left,
		                     grid.height - style.margin.top - style.margin.bottom, style.margin.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x + grid.width - style.margin.right,
		                     grid.y - style.margin.top, style.margin.right,
		                     grid.height - style.margin.top - style.margin.bottom, style.margin.color, TSMS_NULL);
	}

	// render border
	if (!ignoreBorder) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x + style.margin.left,
		                     grid.y - style.margin.top, grid.width - style.margin.left - style.margin.right,
		                     style.border.top, style.border.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left,
		                     grid.y - grid.height + style.margin.bottom + style.border.bottom,
		                     grid.width - style.margin.left - style.margin.right,
		                     style.border.bottom, style.border.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left,
		                     grid.y - style.margin.top,
		                     style.border.left,
		                     grid.height - style.margin.top - style.margin.bottom, style.border.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + grid.width - style.margin.right - style.border.right,
		                     grid.y - style.margin.top,
		                     style.border.right,
		                     grid.height - style.margin.top - style.margin.bottom, style.border.color, TSMS_NULL);
	}

	if (!ignorePadding) {
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - style.margin.top - style.border.top,
		                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right,
		                     style.padding.top, style.padding.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - grid.height + style.margin.bottom + style.border.bottom + style.padding.bottom,
		                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right,
		                     style.padding.bottom, style.padding.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + style.margin.left + style.border.left,
		                     grid.y - style.margin.top - style.border.top,
		                     style.padding.left,
		                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom,
		                     style.padding.color, TSMS_NULL);
		TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
		                     grid.x + grid.width - style.margin.right - style.border.right - style.padding.right,
		                     grid.y - style.margin.top - style.border.top,
		                     style.padding.right,
		                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom,
		                     style.padding.color, TSMS_NULL);
	}

	// render background color
	TSMS_SCREEN_fillRectTopLeft(TSMS_GUI_getGUI(element)->display->screen,
	                     grid.x + style.margin.left + style.border.left + style.padding.left,
	                     grid.y - style.margin.top - style.border.top - style.padding.top,
	                     grid.width - style.margin.left - style.margin.right - style.border.left - style.border.right - style.padding.left - style.padding.right,
	                     grid.height - style.margin.top - style.margin.bottom - style.border.top - style.border.bottom - style.padding.top - style.padding.bottom,
	                     style.backgroundColor, TSMS_NULL);
	return TSMS_SUCCESS;
}