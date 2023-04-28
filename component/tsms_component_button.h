#ifndef TSMS_COMPONENT_BUTTON_H
#define TSMS_COMPONENT_BUTTON_H

#include "tsms_component_text.h"

typedef struct TSMS_BUTTON tButton;
typedef tButton * pButton;

typedef void(*TSMS_BUTTON_CALLBACK)(pButton, void * handler);

struct TSMS_BUTTON {
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT

	TSMS_BUTTON_CALLBACK callback;
	void * handler;
	pText text;
};

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text);

pButton TSMS_BUTTON_create(pText text);

TSMS_RESULT TSMS_BUTTON_onClick(pButton button, TSMS_BUTTON_CALLBACK callback, void * handler);

#endif //TSMS_COMPONENT_BUTTON_H
