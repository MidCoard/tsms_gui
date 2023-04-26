#include "tsms_component_button.h"
#include "tsms_list.h"

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text, TSMS_GUI_TOUCHABLE_CALLBACK callback,
                                    void *handler) {
	pButton button = (pButton) malloc(sizeof(tButton));
	if (button == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for button");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	button->type = TSMS_GUI_TYPE_TOUCHABLE;
	button->preRender = TSMS_GUI_defaultPreRender;
	button->render = TSMS_GUI_defaultRender;
	button->parent = TSMS_NULL;
	button->children = TSMS_LIST_create(1);
	button->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(button->style, TSMS_GUI_defaultStyleUpdateCallback, button);
	button->lastStyle = style;
	button->computedStyle = style;
	button->requestRender = true;
	button->firstRender = true;
	button->grid = TSMS_GUI_INVALID_GRID;
	button->lastGrid = TSMS_GUI_INVALID_GRID;
	button->gui = TSMS_NULL;
	button->level = 0;

	button->releaseCallback = callback;
	button->releaseHandler = handler;

	button->pressCallback = TSMS_NULL;
	button->pressHandler = TSMS_NULL;

	button->longPressCallback = TSMS_NULL;
	button->longPressHandler = TSMS_NULL;

	button->doublePressCallback = TSMS_NULL;
	button->doublePressHandler = TSMS_NULL;


	button->text = text;

	TSMS_GUI_add(button, text);
	return button;
}

pButton TSMS_BUTTON_create(pText text, TSMS_GUI_TOUCHABLE_CALLBACK callback, void * handler) {
	return TSMS_BUTTON_createWithStyle(TSMS_STYLE_DEFAULT_BUTTON, text, callback, handler);
}