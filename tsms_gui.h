#ifndef TSMS_GUI_H
#define TSMS_GUI_H

typedef struct TSMS_GUI tGui;
typedef tGui * pGui;

typedef struct TSMS_GUI_ELEMENT tGuiElement;
typedef tGuiElement * pGuiElement;

#include "tsms_def.h"

typedef TSMS_RESULT (*TSMS_GUI_RENDER_FUNCTION)(pGuiElement element);

struct TSMS_GUI {
	TSMS_GUI_RENDER_FUNCTION render;
	pGuiElement parent;
	TSMS_LP children;
	TSMS_DPHP display;
};

struct TSMS_GUI_ELEMENT {
	TSMS_GUI_RENDER_FUNCTION render;
	pGuiElement parent;
	TSMS_LP children;
};

pGui TSMS_GUI_create(TSMS_DPHP display);

TSMS_RESULT TSMS_GUI_add(pGui gui, pGuiElement element);

TSMS_RESULT TSMS_GUI_draw(pGui gui);

#endif //TSMS_GUI_H
