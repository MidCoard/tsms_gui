#ifndef TSMS_BUTTON_H
#define TSMS_BUTTON_H

#include "tsms_gui.h"

typedef struct TSMS_BUTTON tButton;
typedef tButton * pButton;

struct TSMS_BUTTON {
	TSMS_GUI_RENDER_FUNCTION render;
	pGuiElement parent;
	TSMS_LP children;
	TSMS_STYLE style;
};

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style);
pButton TSMS_BUTTON_create();

#endif //TSMS_BUTTON_H
