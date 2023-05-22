#ifndef TSMS_MUTABLE_STYLE_H
#define TSMS_MUTABLE_STYLE_H

#include "tsms_gui_style.h"

typedef struct TSMS_MUTABLE_STYLE tMutableStyle;
typedef tMutableStyle * pMutableStyle;

typedef void(*TSMS_MUTABLE_STYLE_CALLBACK)(pMutableStyle, TSMS_STYLE, void * handler);

struct TSMS_MUTABLE_STYLE {
	TSMS_STYLE style;
	TSMS_MUTABLE_STYLE_CALLBACK callback;
	void *handler;
};

pMutableStyle TSMS_MUTABLE_STYLE_create(TSMS_STYLE style);

TSMS_RESULT TSMS_MUTABLE_STYLE_set(pMutableStyle mutableStyle, TSMS_STYLE style);

TSMS_STYLE TSMS_MUTABLE_STYLE_get(pMutableStyle mutableStyle);

TSMS_RESULT TSMS_MUTABLE_STYLE_setCallback(pMutableStyle mutableStyle, TSMS_MUTABLE_STYLE_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_MUTABLE_STYLE_setFont(pMutableStyle mutableStyle, TSMS_STYLE_FONT font);

TSMS_RESULT TSMS_MUTABLE_STYLE_setDisplayType(pMutableStyle mutableStyle, TSMS_STYLE_DISPLAY displayType);

TSMS_RESULT TSMS_MUTABLE_STYLE_release(pMutableStyle mutableStyle);

TSMS_RESULT TSMS_MUTABLE_STYLE_setPosition(pMutableStyle mutableStyle, TSMS_STYLE_POSITION position);

TSMS_RESULT TSMS_MUTABLE_STYLE_setLeft(pMutableStyle mutableStyle, uint16_t left);

TSMS_RESULT TSMS_MUTABLE_STYLE_setTop(pMutableStyle mutableStyle, uint16_t top);

TSMS_RESULT TSMS_MUTABLE_STYLE_setZIndex(pMutableStyle mutableStyle, uint16_t zIndex);

TSMS_RESULT TSMS_MUTABLE_STYLE_setAbsolutePosition(pMutableStyle mutableStyle, uint16_t left, uint16_t top, uint16_t zIndex);


#endif //TSMS_MUTABLE_STYLE_H
