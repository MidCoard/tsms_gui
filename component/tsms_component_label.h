#ifndef TSMS_COMPONENT_LABEL_H
#define TSMS_COMPONENT_LABEL_H

#define TSMS_EXTEND_GUI_LABEL_ELEMENT \
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT \
	pText text;                       \
	TSMS_TOUCHABLE_CALLBACK callback; \
	void * handler;

typedef struct TSMS_LABEL tLabel;
typedef tLabel * pLabel;

#include "tsms_gui.h"

typedef void(*TSMS_TOUCHABLE_CALLBACK)(pGuiTouchableElement, void * handler);

#include "tsms_component_text.h"

struct TSMS_LABEL {
	TSMS_EXTEND_GUI_LABEL_ELEMENT
};

pLabel TSMS_LABEL_createWithStyle(TSMS_STYLE style, pText text);

pLabel TSMS_LABEL_create(pText text);

TSMS_RESULT TSMS_LABEL_release(pLabel label);


#endif //TSMS_COMPONENT_LABEL_H
