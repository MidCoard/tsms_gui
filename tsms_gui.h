#ifndef TSMS_GUI_H
#define TSMS_GUI_H

#define TSMS_EXTEND_GUI_ELEMENT \
	TSMS_GUI_PRE_RENDER_FUNCTION preRender;   \
	TSMS_GUI_RENDER_FUNCTION render; \
	pGuiElement parent; \
	TSMS_LP children; \
	TSMS_STYLE style; \
	TSMS_STYLE lastStyle; \
	bool requestRender; \
	bool firstRender; \
	TSMS_GRID_INFO grid; \
	TSMS_GRID_INFO lastGrid; \
	pGui gui; \
	uint8_t level;

typedef struct TSMS_GUI tGui;
typedef tGui * pGui;

typedef struct TSMS_GUI_ELEMENT tGuiElement;
typedef tGuiElement * pGuiElement;

#include "tsms_def.h"
#include "tsms_gui_style.h"
#include "tsms_lock.h"
#include "tsms_display.h"

typedef TSMS_RESULT (*TSMS_GUI_RENDER_FUNCTION)(pGuiElement element);

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint8_t zIndex;
	// x y should be top left
} TSMS_GRID_INFO;

extern TSMS_GRID_INFO TSMS_GUI_INVALID_GRID;

typedef TSMS_GRID_INFO (*TSMS_GUI_PRE_RENDER_FUNCTION)(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

struct TSMS_GUI {
	TSMS_EXTEND_GUI_ELEMENT
	TSMS_DPHP display;
	pLock lock;
	TSMS_LP list;
// we support 100 elements render
};

struct TSMS_GUI_ELEMENT {
	TSMS_EXTEND_GUI_ELEMENT
};

typedef TSMS_GRID_INFO * pGridInfo;

TSMS_RESULT TSMS_GUI_defaultRender(pGuiElement element);

TSMS_GRID_INFO TSMS_GUI_defaultPreRender(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight);

void TSMS_GUI_addRenderEntity(pGui gui, pGuiElement element);

pGui TSMS_GUI_create(TSMS_DPHP display);

TSMS_RESULT TSMS_GUI_add(pGuiElement parent, pGuiElement element);

TSMS_RESULT TSMS_GUI_draw(pGui gui);

TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element);

#endif //TSMS_GUI_H
