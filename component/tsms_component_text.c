#include "tsms_component_text.h"

pText TSMS_TEXT_createWithStyle(TSMS_STYLE style, pMutable text) {
	pText t = (pText) malloc(sizeof(tText));
	if (t == TSMS_NULL){
		tString temp = TSMS_STRING_temp("malloc failed for text");
		TSMS_ERR_report(TSMS_ERR_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	t->preRender = TSMS_NULL;
	t->render = TSMS_NULL;
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

	t->text = text;
	return t;
}

pText TSMS_TEXT_create(pMutable text) {
	return TSMS_TEXT_createWithStyle(TSMS_STYLE_DEFAULT, text);
}