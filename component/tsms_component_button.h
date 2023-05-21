#ifndef TSMS_COMPONENT_BUTTON_H
#define TSMS_COMPONENT_BUTTON_H

#include "tsms_component_text.h"
#include "tsms_component_label.h"

typedef struct TSMS_BUTTON tButton;
typedef tButton * pButton;

struct TSMS_BUTTON {
	TSMS_EXTEND_GUI_LABEL_ELEMENT
};

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text);

pButton TSMS_BUTTON_create(pText text);

TSMS_RESULT TSMS_BUTTON_onClick(pButton button, TSMS_TOUCHABLE_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_BUTTON_release(pButton button);

#endif //TSMS_COMPONENT_BUTTON_H
