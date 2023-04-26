#ifndef TSMS_COMPONENT_BUTTON_H
#define TSMS_COMPONENT_BUTTON_H

#include "tsms_component_text.h"

typedef struct TSMS_BUTTON tButton;
typedef tButton * pButton;

struct TSMS_BUTTON {
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT

	pText text;
};

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pText text, TSMS_GUI_TOUCHABLE_CALLBACK callback,
                                    void *handler);

pButton TSMS_BUTTON_create(pText text, TSMS_GUI_TOUCHABLE_CALLBACK callback, void * handler);

#endif //TSMS_COMPONENT_BUTTON_H
