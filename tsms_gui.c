#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"
#include "tsms_lock.h"
#include "tsms_math.h"
#include "tsms_algorithm.h"

typedef enum {
	TSMS_GRID_NO_UPDATE = 0, TSMS_GRID_UPDATE_ABSOLUTE = 1, TSMS_GRID_UPDATE_RELATIVE = 2, TSMS_GRID_UPDATE_ALL = 3
} TSMS_GRID_UPDATE_TYPE;

typedef struct {
	TSMS_GRID_UPDATE_TYPE type;
	uint8_t minZIndex;
	uint8_t maxZIndex;
} TSMS_GRID_UPDATE_INFO;

TSMS_GRID_INFO TSMS_GUI_INVALID_GRID = {TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, 0};

TSMS_INLINE int __tsms_internal_compare_grid_render_info(void * a, void * b) {
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

TSMS_INLINE uint16_t __tsms_internal_calc_parent_width(TSMS_STYLE style, uint16_t parentWidth) {
	uint16_t maxWidth = style.maxWidth == TSMS_STYLE_AUTO ? 0 : style.maxWidth;
	return min(maxWidth, parentWidth);
}

TSMS_INLINE uint16_t __tsms_internal_calc_parent_height(TSMS_STYLE style, uint16_t parentHeight) {
	uint16_t maxHeight = style.maxHeight == TSMS_STYLE_AUTO ? 0 : style.maxHeight;
	return min(maxHeight, parentHeight);
}

TSMS_INLINE TSMS_RESULT __tsms_internal_default_render(pGuiElement element) {
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			child->render(child);
		}
	element->requestRender = false;
	return TSMS_SUCCESS;
}

TSMS_INLINE void __tsms_internal_default_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	if (element->children == TSMS_NULL || element->children->length == 0) {
		uint16_t width = TSMS_STYLE_getBoxWidth(element->style, 0, parentWidth);
		uint16_t height = TSMS_STYLE_getBoxHeight(element->style, 0, parentHeight);
		// no more children to render, return the grid
		element->grid = TSMS_GUI_calcGrid(element, x, y, width, height, parentWidth, parentHeight);
		return;
	}
	pGuiElement child = element->children->list[0];
	uint16_t elementWidth = __tsms_internal_calc_parent_width(element->style, parentWidth);
	uint16_t elementHeight = __tsms_internal_calc_parent_height(element->style, parentHeight); // not include attachment
	uint16_t boxWidth = 0;
	uint16_t boxHeight = 0;
	if (element->style.gridType == TSMS_GRID_TYPE_DEFAULT) {
		TSMS_GRID_INFO result = child->preRender(child, x, y, elementWidth, elementHeight);
		if (result.x == TSMS_STYLE_AUTO && child->style.position == TSMS_STYLE_POSITION_RELATIVE) {
			// if the first child is absolute, don't care the result, because invalid grid will not render
			uint16_t width = TSMS_STYLE_getBoxWidth(element->style, 0, parentWidth);
			uint16_t height = TSMS_STYLE_getBoxHeight(element->style, 0, parentHeight);
			// the first child is relative and render failed, return the grid
			element->grid = TSMS_GUI_calcGrid(element, x, y, width, height, parentWidth, parentHeight);
			return;
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
			result = child->preRender(child, result.x + result.width, result.y, elementWidth - currentRowWidth, elementHeight - currentColumnHeight);
			if (result.x == TSMS_STYLE_AUTO && child->style.position == TSMS_STYLE_POSITION_RELATIVE) {
				// means TSMS_POSITION_RELATIVE
				currentColumnHeight += maxHeight;
				maxHeight = 0;
				maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
				currentRowWidth = 0;
				result = child->preRender(child, x, y - currentColumnHeight, elementWidth - currentRowWidth, elementHeight - currentColumnHeight);
				if (result.x == TSMS_STYLE_AUTO)
					break;
				else {
					// success
					currentRowWidth += result.width;
					maxHeight = result.height > maxHeight ? result.height : maxHeight;
				}
			} else if (child->style.position == TSMS_STYLE_POSITION_RELATIVE) {
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
	element->grid = TSMS_GUI_calcGrid(element, x, y, boxWidth, boxHeight, parentWidth, parentHeight);
}

TSMS_INLINE void __tsms_internal_request_render(pGuiElement element, bool parentFlag, uint8_t zIndex) {
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++)
			__tsms_internal_request_render(element->children->list[i], element->requestRender || parentFlag, max(zIndex, element->style.zIndex));
	if (parentFlag && element->style.zIndex <= zIndex)
		element->requestRender = false;
}

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight) {
	if (element == TSMS_NULL)
		return TSMS_GUI_INVALID_GRID;
	TSMS_STYLE style = element->style;
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
		uint16_t screenWidth = element->gui->display->screen->width;
		uint16_t screenHeight = element->gui->display->screen->height;
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
		TSMS_ERR_report(TSMS_ERR_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	gui->preRender = __tsms_internal_default_pre_render;
	gui->render = __tsms_internal_default_render;
	gui->parent = TSMS_NULL;
	gui->children = TSMS_LIST_create(10);
	gui->style = TSMS_STYLE_DEFAULT;
	gui->requestRender = true;
	gui->grid = TSMS_GUI_INVALID_GRID;
	gui->gui = gui;
	gui->level = 0;

	gui->display = display;
	gui->lock = TSMS_LOCK_create();
	gui->list = TSMS_LIST_create(10);
//	TSMS_TOUCH_setCallback(gui->display->touch, )
	return gui;
}

TSMS_RESULT TSMS_GUI_add(pGui gui, pGuiElement element) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	element->parent = gui;
	element->gui = gui;
	return TSMS_LIST_add(gui->children, element);
}

TSMS_RESULT TSMS_GUI_draw(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_LOCK_lock(gui->lock);
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
			element->render(element, element->grid);
	}
	TSMS_LIST_clear(gui->list);
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	TSMS_LOCK_unlock(gui->lock);
	return result;
}