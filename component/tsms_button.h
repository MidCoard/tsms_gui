#ifndef TSMS_BUTTON_H
#define TSMS_BUTTON_H

#include "tsms_gui.h"
#include "tsms_mutable.h"

typedef struct TSMS_BUTTON tButton;
typedef tButton * pButton;

typedef void(*TSMS_BUTTON_CALLBACK)(pButton, void *);

struct TSMS_BUTTON {
	TSMS_EXTEND_GUI_ELEMENT

	pMutable text;
	TSMS_BUTTON_CALLBACK callback;
	void *handler;
};

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pMutable pMutable, TSMS_BUTTON_CALLBACK pFunction,
                                    void *pVoid);
pButton TSMS_BUTTON_create(pMutable text, TSMS_BUTTON_CALLBACK callback, void * handler);

#endif //TSMS_BUTTON_H
