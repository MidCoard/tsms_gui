#include "tsms_component_text.h"
#include "tsms_font.h"
#include "tsms_display.h"

struct _textGridInfo {
	uint16_t x;
	uint16_t y;
};

TSMS_INLINE struct _textGridInfo* __tsms_internal_create_text_grid_info(uint16_t x, uint16_t y) {
	struct _textGridInfo* info = malloc(sizeof(struct _textGridInfo));
	info->x = x;
	info->y = y;
	return info;
}

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_text_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	pText text = (pText) element;
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	TSMS_GUI_STYLE_RENDER
	pString t = text->text;
	if (t->length == 0)
		return element->grid = TSMS_GUI_calcGrid(element,style, x, y, 0, 0, parentWidth, parentHeight);
	uint16_t maxWidth = 0;
	uint16_t currentRowWidth = TSMS_STYLE_leftOffset(style);
	uint16_t maxHeight = 0;
	uint16_t currentColumnHeight = TSMS_STYLE_topOffset(style);
	for (TSMS_POS i = 0; i < text->list->length; i++)
		free(text->list->list[i]);
	TSMS_LIST_clear(text->list);
	for (TSMS_POS i = 0; i < t->length; i++) {
		TSMS_FONT_DATA font = TSMS_FONT_resolve(style.font.type, style.font.font, t->cStr[i]);
		if (font.type == TSMS_FONT_TYPE_INVALID)
			return element->grid = TSMS_GUI_INVALID_GRID;
		uint16_t width = font.width * style.font.size;
		uint16_t height = font.height * style.font.size;
		if (width + currentRowWidth > parentWidth || height + currentColumnHeight > parentHeight) {
			currentColumnHeight += maxHeight;
			maxHeight = 0;
			maxWidth = max(currentRowWidth, maxWidth);
			currentRowWidth = TSMS_STYLE_leftOffset(style);
			if (width + currentRowWidth > parentWidth || height + currentColumnHeight > parentHeight)
				return element->grid = TSMS_GUI_INVALID_GRID;
		}
		TSMS_LIST_add(text->list, __tsms_internal_create_text_grid_info(x + currentRowWidth, y - currentColumnHeight));
		currentRowWidth += width;
		maxHeight = max(height, maxHeight);
	}
	if (currentRowWidth > 0)
		maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
	return element->grid = TSMS_GUI_calcGrid(element, style, x, y, maxWidth, maxHeight + currentColumnHeight, parentWidth, parentHeight);
}

TSMS_INLINE TSMS_RESULT __tsms_internal_text_render(pGuiElement element, pLock lock) {
	TSMS_STYLE style = element->computedStyle;
	if (!TSMS_GUI_isInvalidGrid(element->grid) && element->grid.displayType == TSMS_STYLE_DISPLAY_BLOCK) {
		TSMS_GUI_renderStyle(element, style, lock);
		pText text = (pText) element;
		pString t = text->text;
		if (t->length == 0)
			return TSMS_SUCCESS;
		for (TSMS_POS i = 0; i < t->length; i++) {
			TSMS_FONT_DATA font = TSMS_FONT_resolve(style.font.type, style.font.font, t->cStr[i]);
			if (font.type == TSMS_FONT_TYPE_INVALID)
				return TSMS_SUCCESS;
			struct _textGridInfo* info = text->list->list[i];
			TSMS_SCREEN_drawCharTopLeft(TSMS_GUI_getGUI(element)->display->screen, info->x,
			                            info->y,
			                            style.font.type, style.font.font, t->cStr[i], *style.font.color,
			                            style.font.size, lock);
			TSMS_LIST_add(element->renderOperations,
                          TSMS_GUI_createCharRenderOperation(info->x, info->y, style.font.type, style.font.font,
                                                             t->cStr[i], style.font.size));
		}
	}

	element->requestRender = false;
	element->lastStyle = style;
	element->lastGrid = element->grid;
	return TSMS_SUCCESS;
}

TSMS_INLINE void __tsms_internal_text_callback(pNativeMutableString mutable, void * handler) {
	pText text = (pText) handler;
	text->requestRender = true;
}

pText TSMS_TEXT_createWithStyle(TSMS_STYLE style, pString text) {
	pText t = (pText) malloc(sizeof(tText));
	if (t == TSMS_NULL){
		tString temp = TSMS_STRING_temp("malloc failed for guiText");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	t->type = TSMS_GUI_TYPE_TEXT;
	t->preRender = __tsms_internal_text_pre_render;
	t->render = __tsms_internal_text_render;
	t->parent = TSMS_NULL;
	t->children = TSMS_NULL;
	t->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(t->style, TSMS_GUI_defaultStyleCallback, t);
	t->lastStyle = style;
	t->computedStyle = style;
	t->requestRender = true;
	t->grid = TSMS_GUI_INVALID_GRID;
	t->lastGrid = TSMS_GUI_INVALID_GRID;
	t->gui = TSMS_NULL;
	t->level = 0;
	t->renderOperations = TSMS_LIST_create(10);

	t->_native = TSMS_NATIVE_MUTABLE_STRING_create(text);
	TSMS_NATIVE_MUTABLE_STRING_setCallback(t->_native, __tsms_internal_text_callback, t);
	t->text = text;
	t->list = TSMS_LIST_create(10);
	return t;
}

pText TSMS_TEXT_create(pString text) {
	return TSMS_TEXT_createWithStyle(TSMS_STYLE_DEFAULT_TEXT, text);
}

TSMS_RESULT TSMS_TEXT_release(pText text) {
	if (text == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_NATIVE_MUTABLE_STRING_release(text->_native);
	TSMS_LIST_release(text->list);
	return TSMS_GUI_releaseGuiElement(text);
}