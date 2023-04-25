#include "tsms_component_text.h"
#include "tsms_font.h"
#include "tsms_display.h"

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_text_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	pText text = (pText) element;
	pString t = TSMS_MUTABLE_get(text->text);
	if (t->length == 0)
		return element->grid = TSMS_GUI_calcGrid(element, x, y, 0, 0, parentWidth, parentHeight);
	uint16_t maxWidth = 0;
	uint16_t currentRowWidth = 0;
	uint16_t maxHeight = 0;
	uint16_t currentColumnHeight = 0;
	for (TSMS_POS i = 0; i < t->length; i++) {
		TSMS_FONT_DATA font = TSMS_FONT_resolve(text->style.font.type, text->style.font.font, t->cStr[i]);
		if (font.type == TSMS_FONT_TYPE_INVALID)
			return element->grid = TSMS_GUI_calcGrid(element, x, y, 0, 0, parentWidth, parentHeight);
		uint16_t width = font.width * text->style.font.size;
		uint16_t height = font.height * text->style.font.size;
		if (width + currentRowWidth > parentWidth || height + currentColumnHeight > parentHeight) {
			currentColumnHeight += maxHeight;
			maxHeight = 0;
			maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
			currentRowWidth = 0;
			if (width + currentRowWidth > parentWidth || height + currentColumnHeight > parentHeight)
				break;
		}
		currentRowWidth += width;
		maxHeight = height > maxHeight ? height : maxHeight;
	}
	if (currentRowWidth > 0)
		maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
	return element->grid = TSMS_GUI_calcGrid(element, x, y, maxWidth, maxHeight + currentColumnHeight, parentWidth, parentHeight);
}

TSMS_INLINE TSMS_RESULT __tsms_internal_text_render(pGuiElement element) {
	TSMS_GUI_renderStyle(element);
	pText text = (pText) element;
	pString t = TSMS_MUTABLE_get(text->text);
	if (t->length == 0)
		return TSMS_SUCCESS;
	TSMS_GRID_INFO grid = element->grid;
	uint16_t maxWidth = 0;
	uint16_t currentRowWidth = 0;
	uint16_t maxHeight = 0;
	uint16_t currentColumnHeight = 0;
	// TODO: add cache
	for (TSMS_POS i = 0; i < t->length;i++) {
		TSMS_FONT_DATA font = TSMS_FONT_resolve(text->style.font.type, text->style.font.font, t->cStr[i]);
		if (font.type == TSMS_FONT_TYPE_INVALID)
			return TSMS_SUCCESS;
		uint16_t width = font.width * text->style.font.size;
		uint16_t height = font.height * text->style.font.size;
		if (width + currentRowWidth > grid.width || height + currentColumnHeight > grid.height) {
			currentColumnHeight += maxHeight;
			maxHeight = 0;
			maxWidth = currentRowWidth > maxWidth ? currentRowWidth : maxWidth;
			currentRowWidth = 0;
			if (width + currentRowWidth > grid.width || height + currentColumnHeight > grid.height)
				break;
		}
		TSMS_SCREEN_drawCharTopLeft(TSMS_GUI_getGUI(element)->display->screen, grid.x + currentRowWidth, grid.y + currentColumnHeight,
		                            text->style.font.type, text->style.font.font, t->cStr[i], text->style.font.color,
		                            text->style.font.size, TSMS_NULL);
		currentRowWidth += width;
		maxHeight = height > maxHeight ? height : maxHeight;
	}
	text->requestRender = false;
	text->firstRender = false;
	text->lastStyle = text->style;
	text->lastGrid = text->grid;
	return TSMS_SUCCESS;
}

TSMS_INLINE void __tsms_internal_text_callback(pMutable mutable, void * data, void * handler) {
	pText text = (pText) handler;
	if (!TSMS_STRING_equals(mutable->data, data))
		text->requestRender = true;
}

pText TSMS_TEXT_createWithStyle(TSMS_STYLE style, pMutable text) {
	pText t = (pText) malloc(sizeof(tText));
	if (t == TSMS_NULL){
		tString temp = TSMS_STRING_temp("malloc failed for text");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	t->preRender = __tsms_internal_text_pre_render;
	t->render = __tsms_internal_text_render;
	t->parent = TSMS_NULL;
	t->children = TSMS_NULL;
	t->style = style;
	t->lastStyle = style;
	t->requestRender = true;
	t->firstRender = true;
	t->grid = TSMS_GUI_INVALID_GRID;
	t->lastGrid = TSMS_GUI_INVALID_GRID;
	t->gui = TSMS_NULL;
	t->level = 0;

	TSMS_MUTABLE_setSetterCallback(text,__tsms_internal_text_callback, t);
	t->text = text;
	return t;
}

pText TSMS_TEXT_create(pMutable text) {
	return TSMS_TEXT_createWithStyle(TSMS_STYLE_DEFAULT, text);
}