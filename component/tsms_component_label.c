#include "tsms_component_label.h"

TSMS_INLINE void __tsms_internal_label_release(pGuiTouchableElement element, uint16_t x, uint16_t y, void* handler) {
	pLabel label = (pLabel) element;
	if (label->callback != TSMS_NULL)
		label->callback(label, x, y, label->handler);
}

TSMS_INLINE void __tsms_internal_label_press(pGuiTouchableElement element, uint16_t x, uint16_t y, void* handler) {
}

pLabel TSMS_LABEL_createWithStyle(TSMS_STYLE style, pText text) {
	pLabel label = (pLabel) TSMS_malloc(sizeof(tLabel));
	if (label == TSMS_NULL)
		return TSMS_NULL;
	TSMS_INIT_GUI_LABEL_ELEMENT(label, TSMS_GUI_TYPE_TOUCHABLE, TSMS_CONTAINER_preRender, TSMS_CONTAINER_render, style, TSMS_NULL, __tsms_internal_label_press, label, __tsms_internal_label_release, label, TSMS_NULL, TSMS_NULL, TSMS_NULL, TSMS_NULL, text)
	if (label->style == TSMS_NULL || label->children == TSMS_NULL || label->renderOperations == TSMS_NULL || label->points == TSMS_NULL) {
		TSMS_LABEL_release(label);
		return TSMS_NULL;
	}
	return label;
}

pLabel TSMS_LABEL_create(pText text) {
	return TSMS_LABEL_createWithStyle(TSMS_STYLE_DEFAULT_LABEL, text);
}

TSMS_RESULT TSMS_LABEL_onClick(pLabel label, TSMS_GUI_TOUCHABLE_CALLBACK callback, void * handler) {
	if (label == TSMS_NULL)
		return TSMS_ERROR;
	label->callback = callback;
	label->handler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_LABEL_release(pLabel label) {
	if (label == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_INT_LIST_release(label->points);
	return TSMS_GUI_releaseGuiElement(label);
}