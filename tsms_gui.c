#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"

TSMS_INLINE TSMS_RESULT __tsms_internal_default_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight) {
	element->preRender(element, x, y, totalWidth, totalHeight); // not return TSMS_SUCCESS don't care, invalid grid will not render

	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			child->render(child, x, y, totalWidth, totalHeight);
		}
	element->requestRender = false;
	return TSMS_SUCCESS;
}

TSMS_INLINE TSMS_RESULT __tsms_internal_default_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight) {
	if (element->children != TSMS_NULL) {
		if (element->children->length == 0)
			return TSMS_GUI_calcGridInfo(element, x , y, totalWidth, totalHeight);
		pGuiElement child = element->children->list[0];
		TSMS_RESULT result = child->preRender(child, x, y, totalWidth, totalHeight);
		if (result != TSMS_SUCCESS)
			return TSMS_GUI_calcGridInfo(element, x , y, totalWidth, totalHeight);
		TSMS_GRID_INFO  gridInfo = child->grid;
		uint16_t height = 0;
		uint16_t maxHeight = 0;
		uint16_t width = 0;
		for (TSMS_POS i = 1; i < element->children->length; i++) {
			child = element->children->list[i];
			result = child->preRender(child, gridInfo.x + gridInfo.width, gridInfo.y, totalWidth, totalHeight);
			if (result != TSMS_SUCCESS) {
				height += maxHeight;
				result = child->preRender(child, x, gridInfo.y + gridInfo.height, totalWidth, totalHeight);
				if (result != TSMS_SUCCESS)
					break;
			} else {
				gridInfo = child->grid;
				maxHeight = gridInfo.height > maxHeight ? gridInfo.height : maxHeight;
			}
		}
		height += maxHeight;
		element->style.width = width;
		element->style.height = height;
	}
	return TSMS_GUI_calcGridInfo(element, x ,y, totalWidth, totalHeight);
}

TSMS_INLINE bool __tsms_internal_request_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight) {
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			__tsms_internal_request_render(child, x, y, totalWidth, totalHeight);
		}
	if (element->requestRender)
		return element->render(element, x ,y, totalWidth, totalHeight);
	return false;
}

TSMS_RESULT TSMS_GUI_calcGridInfo(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight) {
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_STYLE style = element->style;
	int width = style.width;
	int height = style.height;
	if (width == TSMS_STYLE_AUTO) {
		width = style.minWidth;
		if (width == TSMS_STYLE_AUTO)
			width = 0;
	}
	if (height == TSMS_STYLE_AUTO) {
		height = style.minHeight;
		if (height == TSMS_STYLE_AUTO)
			height = 0;
	}
	width += style.padding.left + style.padding.right + style.border.left + style.border.right + style.margin.left + style.margin.right;
	height += style.padding.top + style.padding.bottom + style.border.top + style.border.bottom + style.margin.top + style.margin.bottom;
	if (width + x > totalWidth || height - y < 0)
		return TSMS_ERROR;
	element->grid = (TSMS_GRID_INFO){x, y, width, height};
	return TSMS_SUCCESS;
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
	// consider set grid info todo

	gui->display = display;
//	TSMS_TOUCH_setCallback(gui->display->touch, )
	return gui;
}

TSMS_RESULT TSMS_GUI_add(pGui gui, pGuiElement element) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	element->parent = gui;
	return TSMS_LIST_add(gui->children, element);
}

TSMS_RESULT TSMS_GUI_draw(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	return result | __tsms_internal_request_render(gui, 0, gui->display->screen->height, gui->display->screen->width, gui->display->screen->height);
}