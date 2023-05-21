#include "tsms_component_label.h"

TSMS_INLINE void __tsms_internal_label_release(pGuiTouchableElement element, void* handler) {
	pLabel label = (pLabel) element;
	if (label->callback != TSMS_NULL)
		label->callback(label, label->handler);
}

TSMS_INLINE void __tsms_internal_label_press(pGuiTouchableElement element, void* handler) {
}

pLabel TSMS_LABEL_createWithStyle(TSMS_STYLE style, pText text) {
	pLabel label = (pLabel) malloc(sizeof(tLabel));
	if (label == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for guiLabel");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	label->type = TSMS_GUI_TYPE_TOUCHABLE;
	label->preRender = TSMS_CONTAINER_preRender;
	label->render = TSMS_CONTAINER_render;
	label->parent = TSMS_NULL;
	label->children = TSMS_LIST_create(1);
	label->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(label->style, TSMS_GUI_defaultStyleCallback, label);
	label->lastStyle = style;
	label->computedStyle = style;
	label->requestRender = true;
	label->grid = TSMS_GUI_INVALID_GRID;
	label->lastGrid = TSMS_GUI_INVALID_GRID;
	label->gui = TSMS_NULL;
	label->level = 0;
	label->renderOperations = TSMS_LIST_create(10);

	label->releaseCallback = __tsms_internal_label_release;
	label->releaseHandler = label;

	label->pressCallback = __tsms_internal_label_press;
	label->pressHandler = label;

	label->longPressCallback = TSMS_NULL;
	label->longPressHandler = TSMS_NULL;

	label->doublePressCallback = TSMS_NULL;
	label->doublePressHandler = TSMS_NULL;
	label->press = 0;
	label->points = TSMS_INT_LIST_create(1);
	label->lastUpdate = 0;

	label->callback = TSMS_NULL;
	label->handler = TSMS_NULL;
	label->text = text;

	TSMS_GUI_add(label, text);
	return label;
}

pLabel TSMS_LABEL_create(pText text) {
	return TSMS_LABEL_createWithStyle(TSMS_STYLE_DEFAULT_LABEL, text);
}

TSMS_RESULT TSMS_LABEL_onClick(pLabel label, TSMS_TOUCHABLE_CALLBACK callback, void * handler) {
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