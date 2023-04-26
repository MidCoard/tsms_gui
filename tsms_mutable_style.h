#ifndef TSMS_MUTABLE_STYLE_H
#define TSMS_MUTABLE_STYLE_H

#include "tsms_gui_style.h"

typedef struct TSMS_MUTABLE_STYLE tMutableStyle;
typedef tMutableStyle * pMutableStyle;

typedef void(*TSMS_MUTABLE_STYLE_CALLBACK)(pMutableStyle, TSMS_STYLE, void * handler);

struct TSMS_MUTABLE_STYLE {
	TSMS_STYLE style;
	TSMS_MUTABLE_STYLE_CALLBACK setterCallback;
	void *setterHandler;
	void *getterHandler;
};

pMutableStyle TSMS_MUTABLE_STYLE_create(TSMS_STYLE style);

TSMS_RESULT TSMS_MUTABLE_STYLE_set(pMutableStyle mutableStyle, TSMS_STYLE style);

TSMS_STYLE TSMS_MUTABLE_STYLE_get(pMutableStyle mutableStyle);

TSMS_RESULT TSMS_MUTABLE_STYLE_setCallback(pMutableStyle mutableStyle, TSMS_MUTABLE_STYLE_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_MUTABLE_STYLE_setFont(pMutableStyle mutableStyle, TSMS_STYLE_FONT font);

TSMS_RESULT TSMS_MUTABLE_STYLE_setDisplayType(pMutableStyle mutableStyle, TSMS_DISPLAY_TYPE displayType);


#endif //TSMS_MUTABLE_STYLE_H
