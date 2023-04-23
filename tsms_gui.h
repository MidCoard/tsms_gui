#ifndef TSMS_GUI_H
#define TSMS_GUI_H

#define TSMS_EXTEND_GUI_ELEMENT \
	TSMS_GUI_RENDER_FUNCTION preRender;   \
	TSMS_GUI_RENDER_FUNCTION render; \
	pGuiElement parent; \
	TSMS_LP children; \
	TSMS_STYLE style; \
	bool requestRender; \
	TSMS_GRID_INFO grid;

typedef struct TSMS_GUI tGui;
typedef tGui * pGui;

typedef struct TSMS_GUI_ELEMENT tGuiElement;
typedef tGuiElement * pGuiElement;

#include "tsms_def.h"
#include "tsms_gui_style.h"

typedef TSMS_RESULT (*TSMS_GUI_RENDER_FUNCTION)(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight);

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	// x y should be top left
} TSMS_GRID_INFO;

struct TSMS_GUI {
	TSMS_EXTEND_GUI_ELEMENT
	TSMS_DPHP display;
};

struct TSMS_GUI_ELEMENT {
	TSMS_EXTEND_GUI_ELEMENT
};

TSMS_RESULT TSMS_GUI_calcGridInfo(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight);

pGui TSMS_GUI_create(TSMS_DPHP display);

TSMS_RESULT TSMS_GUI_add(pGui gui, pGuiElement element);

TSMS_RESULT TSMS_GUI_draw(pGui gui);

#endif //TSMS_GUI_H
