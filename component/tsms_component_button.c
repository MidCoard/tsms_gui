#include "tsms_component_button.h"
#include "tsms_list.h"

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text, TSMS_BUTTON_CALLBACK callback,
                                    void *handler) {
	pButton button = (pButton) malloc(sizeof(tButton));
	if (button == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for button");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	button->preRender = TSMS_GUI_defaultPreRender;
	button->render = TSMS_GUI_defaultRender;
	button->parent = TSMS_NULL;
	button->children = TSMS_LIST_create(1);
	button->style = style;
	button->lastStyle = style;
	button->requestRender = true;
	button->firstRender = true;
	button->grid = TSMS_GUI_INVALID_GRID;
	button->lastGrid = TSMS_GUI_INVALID_GRID;
	button->gui = TSMS_NULL;
	button->level = 0;

	button->text = text;
	button->callback = callback;
	button->handler = handler;
	button->press = false;

	TSMS_GUI_add(button, text);
	return button;
}

pButton TSMS_BUTTON_create(pText text, TSMS_BUTTON_CALLBACK callback, void * handler) {
	return TSMS_BUTTON_createWithStyle(TSMS_STYLE_DEFAULT_BUTTON, text, callback, handler);
}