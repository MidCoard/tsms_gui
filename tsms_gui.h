#ifndef TSMS_GUI_H
#define TSMS_GUI_H

#define TSMS_EXTEND_GUI_ELEMENT \
    TSMS_GUI_TYPE type;  \
	TSMS_GUI_PRE_RENDER_FUNCTION preRender;   \
	TSMS_GUI_RENDER_FUNCTION render; \
	pGuiElement parent; \
	TSMS_LP children; \
	pMutableStyle style; \
	TSMS_STYLE lastStyle; \
	TSMS_STYLE computedStyle; \
	bool requestRender; \
	bool firstRender; \
	TSMS_GRID_INFO grid; \
	TSMS_GRID_INFO lastGrid; \
	pGui gui; \
	uint8_t level;

#define TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT \
	TSMS_EXTEND_GUI_ELEMENT \
	TSMS_GUI_TOUCHABLE_CALLBACK pressCallback; \
	void * pressHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK releaseCallback; \
	void * releaseHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK longPressCallback; \
	void * longPressHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK doublePressCallback; \
	void * doublePressHandler;
typedef enum {
	TSMS_GUI_TYPE_GUI, TSMS_GUI_TYPE_TOUCHABLE, TSMS_GUI_TYPE_TEXT
} TSMS_GUI_TYPE;

typedef struct TSMS_GUI tGui;
typedef tGui * pGui;

typedef struct TSMS_GUI_ELEMENT tGuiElement;
typedef tGuiElement * pGuiElement;

typedef struct TSMS_GUI_TOUCHABLE_ELEMENT tGuiTouchableElement;
typedef tGuiTouchableElement * pGuiTouchableElement;

typedef void(*TSMS_GUI_TOUCHABLE_CALLBACK)(pGuiTouchableElement , void *);

#include "tsms_lock.h"

typedef TSMS_RESULT (*TSMS_GUI_RENDER_FUNCTION)(pGuiElement element, pLock lock);


#include "tsms_native_mutable.h"
#include "tsms_mutable_style.h"
#include "tsms_lock.h"
#include "tsms_display.h"
#include "tsms_set.h"

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t zIndex;
	TSMS_DISPLAY_TYPE displayType;
	// x y should be top left
} TSMS_GRID_INFO;

extern TSMS_GRID_INFO TSMS_GUI_INVALID_GRID;

typedef TSMS_GRID_INFO (*TSMS_GUI_PRE_RENDER_FUNCTION)(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

struct TSMS_GUI {
	TSMS_EXTEND_GUI_ELEMENT
	TSMS_DPHP display;
	TSMS_LP list;
	TSMS_LP touchableList;
};

struct TSMS_GUI_ELEMENT {
	TSMS_EXTEND_GUI_ELEMENT
};

struct TSMS_GUI_TOUCHABLE_ELEMENT {
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT
};

typedef TSMS_GRID_INFO * pGridInfo;

bool TSMS_GUI_inGrid(TSMS_GRID_INFO grid, uint16_t x, uint16_t y);

void TSMS_GUI_defaultStyleUpdateCallback(pMutableStyle style, TSMS_STYLE data, void * handler);

pGui TSMS_GUI_getGUI(pGuiElement element);

TSMS_RESULT TSMS_GUI_defaultRender(pGuiElement element, pLock lock);

bool TSMS_GUI_isInvalidGrid(TSMS_GRID_INFO grid);

TSMS_GRID_INFO TSMS_GUI_defaultPreRender(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, TSMS_STYLE style, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight);

pGui TSMS_GUI_create(TSMS_DPHP display);

TSMS_RESULT TSMS_GUI_add(pGuiElement parent, pGuiElement element);

TSMS_RESULT TSMS_GUI_draw(pGui gui);

TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element, TSMS_STYLE style, pLock lock);

bool TSMS_GUI_equalsGrid(TSMS_GRID_INFO grid1, TSMS_GRID_INFO grid2);

#endif //TSMS_GUI_H
