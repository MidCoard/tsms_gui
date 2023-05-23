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
	TSMS_INIT_GUI_LABEL_ELEMENT(button, TSMS_GUI_TYPE_TOUCHABLE, TSMS_CONTAINER_preRender, TSMS_CONTAINER_render, style, TSMS_NULL, __tsms_internal_button_press, button, __tsms_internal_button_release, button, TSMS_NULL, TSMS_NULL, TSMS_NULL, TSMS_NULL, text)
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