#include "tsms_component_button.h"

TSMS_INLINE void __tsms_internal_button_press(pGuiTouchableElement element, uint16_t x, uint16_t y, void* handler) {
	pButton button = (pButton) element;
	TSMS_STYLE style = TSMS_MUTABLE_STYLE_get(button->style);
	style.backgroundColor = style.pressedColor;
	style.padding.color = style.pressedColor;
	TSMS_MUTABLE_STYLE_set(button->style, style);
}

TSMS_INLINE void __tsms_internal_button_release(pGuiTouchableElement element, uint16_t x, uint16_t y, void* handler) {
	pButton button = (pButton) element;
	TSMS_STYLE style = TSMS_MUTABLE_STYLE_get(button->style);
	style.backgroundColor = style.releasedColor;
	style.padding.color = style.releasedColor;
	TSMS_MUTABLE_STYLE_set(button->style, style);
	if (button->callback != TSMS_NULL)
		button->callback(button, x, y, button->handler);
}

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text) {
	pButton button = (pButton) malloc(sizeof(tButton));
	if (button == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for guiButton");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	button->type = TSMS_GUI_TYPE_TOUCHABLE;
	button->preRender = TSMS_CONTAINER_preRender;
	button->render = TSMS_CONTAINER_render;
	button->parent = TSMS_NULL;
	button->children = TSMS_LIST_create(1);
	button->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(button->style, TSMS_GUI_defaultStyleCallback, button);
	button->computedStyle = style;
	button->requestRender = true;
	button->grid = TSMS_GUI_INVALID_GRID;
	button->lastGrid = TSMS_GUI_INVALID_GRID;
	button->gui = TSMS_NULL;
	button->level = 0;
	button->renderOperations = TSMS_LIST_create(10);

	button->releaseCallback = __tsms_internal_button_release;
	button->releaseHandler = button;

	button->pressCallback = __tsms_internal_button_press;
	button->pressHandler = button;

	button->longPressCallback = TSMS_NULL;
	button->longPressHandler = TSMS_NULL;

	button->doublePressCallback = TSMS_NULL;
	button->doublePressHandler = TSMS_NULL;
	button->press = 0;
	button->points = TSMS_INT_LIST_create(1);
	button->lastUpdate = 0;

	button->callback = TSMS_NULL;
	button->handler = TSMS_NULL;
	button->text = text;

	TSMS_GUI_add(button, text);
	return button;
}

pButton TSMS_BUTTON_create(pText text) {
	return TSMS_BUTTON_createWithStyle(TSMS_STYLE_DEFAULT_BUTTON, text);
}

TSMS_RESULT TSMS_BUTTON_onClick(pButton button, TSMS_GUI_TOUCHABLE_CALLBACK callback, void * handler) {
	if (button == TSMS_NULL)
		return TSMS_ERROR;
	button->callback = callback;
	button->handler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_BUTTON_release(pButton button) {
	if (button == TSMS_NULL)
		return TSMS_ERROR;
	return TSMS_LABEL_release(button);
}