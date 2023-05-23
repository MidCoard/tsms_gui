#ifndef TSMS_COMPONENT_LABEL_H
#define TSMS_COMPONENT_LABEL_H

#define TSMS_EXTEND_GUI_LABEL_ELEMENT \
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT \
	pText text;                       \
	TSMS_GUI_TOUCHABLE_CALLBACK callback; \
	void * handler;

#define TSMS_INIT_GUI_LABEL_ELEMENT(element, type0, preRender0, render0, style0, gui0, pressCallback0, pressHandler0, releaseCallback0, releaseHandler0, longPressCallback0, longPressHandler0, doublePressCallback0, doublePressHandler0, text0) \
	TSMS_INIT_GUI_TOUCHABLE_ELEMENT(element, type0, preRender0, render0, style0, gui0, pressCallback0, pressHandler0, releaseCallback0, releaseHandler0, longPressCallback0, longPressHandler0, doublePressCallback0, doublePressHandler0) \
	element->text = text0; \
	element->callback = TSMS_NULL; \
	element->handler = TSMS_NULL; \
	TSMS_GUI_add(element, text0);

typedef struct TSMS_LABEL tLabel;
typedef tLabel * pLabel;

#include "tsms_gui.h"
#include "tsms_component_text.h"

struct TSMS_LABEL {
	TSMS_EXTEND_GUI_LABEL_ELEMENT
};

pLabel TSMS_LABEL_createWithStyle(TSMS_STYLE style, pText text);

pLabel TSMS_LABEL_create(pText text);

TSMS_RESULT TSMS_LABEL_onClick(pLabel label, TSMS_GUI_TOUCHABLE_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_LABEL_release(pLabel label);


#endif //TSMS_COMPONENT_LABEL_H
