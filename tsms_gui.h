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
	TSMS_GRID_INFO grid; \
	TSMS_GRID_INFO lastGrid; \
	pGui gui; \
	uint8_t level;                 \
	TSMS_LP renderOperations;

#define TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT \
	TSMS_EXTEND_GUI_ELEMENT \
	TSMS_GUI_TOUCHABLE_CALLBACK pressCallback; \
	void * pressHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK releaseCallback; \
	void * releaseHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK longPressCallback; \
	void * longPressHandler; \
	TSMS_GUI_TOUCHABLE_CALLBACK doublePressCallback; \
	void * doublePressHandler; \
	uint8_t press; \
	TSMS_ILP points; \
	double lastUpdate;

#define TSMS_PRESS_STATE_PRESS 1
#define TSMS_PRESS_STATE_LONG_PRESS 2
#define TSMS_PRESS_STATE_PRESS_AND_RELEASE 4
#define TSMS_PRESS_STATE_DOUBLE_PRESS 8
#define TSMS_GUI_DOUBLE_PRESS_TIME 0.2

typedef enum {
	TSMS_GUI_TYPE_GUI = 0,TSMS_GUI_TYPE_CONTAINER, TSMS_GUI_TYPE_ROW, TSMS_GUI_TYPE_COLUMN, TSMS_GUI_TYPE_TOUCHABLE, TSMS_GUI_TYPE_TEXT
} TSMS_GUI_TYPE;

typedef enum {
	TSMS_RENDER_OPERATION_TYPE_FILL_RECT, TSMS_RENDER_OPERATION_TYPE_CHAR, TSMS_RENDER_OPERATION_TYPE_STRING
} TSMS_RENDER_OPERATION_TYPE;

typedef struct TSMS_GUI tGui;
typedef tGui * pGui;

typedef struct TSMS_GUI_ELEMENT tGuiElement;
typedef tGuiElement * pGuiElement;

typedef struct TSMS_GUI_TOUCHABLE_ELEMENT tGuiTouchableElement;
typedef tGuiTouchableElement * pGuiTouchableElement;

typedef struct TSMS_RENDER_OPERATION tRenderOperation;
typedef tRenderOperation * pRenderOperation;

typedef struct TSMS_GRID_INFO TSMS_GRID_INFO;

typedef void(*TSMS_GUI_TOUCHABLE_CALLBACK)(pGuiTouchableElement , void *);

#include "tsms_lock.h"

typedef TSMS_RESULT (*TSMS_GUI_RENDER_FUNCTION)(pGuiElement element, pLock lock);
typedef TSMS_GRID_INFO (*TSMS_GUI_PRE_RENDER_FUNCTION)(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

#include "tsms_native_mutable.h"
#include "tsms_mutable_style.h"
#include "tsms_display.h"
#include "tsms_int_list.h"

struct TSMS_GRID_INFO {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t zIndex;
	TSMS_STYLE_DISPLAY displayType;
	// x y should be top left
};

#include "tsms_component_container.h"

struct TSMS_RENDER_OPERATION {
	TSMS_RENDER_OPERATION_TYPE type;
	uint8_t size;
	uint8_t * data;
};


struct TSMS_GUI {
	TSMS_EXTEND_GUI_CONTAINER_ELEMENT
	TSMS_DPHP display;
	TSMS_LP list;
	TSMS_LP touchableList;
	pLock lock;
};

struct TSMS_GUI_ELEMENT {
	TSMS_EXTEND_GUI_ELEMENT
};

struct TSMS_GUI_TOUCHABLE_ELEMENT {
	TSMS_EXTEND_GUI_TOUCHABLE_ELEMENT
};

extern TSMS_GRID_INFO TSMS_GUI_INVALID_GRID;

bool TSMS_GUI_inGrid(TSMS_GRID_INFO grid, uint16_t x, uint16_t y);

void TSMS_GUI_defaultStyleCallback(pMutableStyle style, TSMS_STYLE data, void * handler);

pGui TSMS_GUI_getGUI(pGuiElement element);

bool TSMS_GUI_isInvalidGrid(TSMS_GRID_INFO grid);

TSMS_GRID_INFO TSMS_GUI_calcGrid(pGuiElement element, TSMS_STYLE style, uint16_t x, uint16_t y, uint16_t boxWidth, uint16_t boxHeight, uint16_t parentWidth, uint16_t parentHeight);

pGui TSMS_GUI_create(TSMS_DPHP display);

TSMS_RESULT TSMS_GUI_add(pGuiElement parent, pGuiElement element);

TSMS_RESULT TSMS_GUI_draw(pGui gui);

TSMS_RESULT TSMS_GUI_renderStyle(pGuiElement element, TSMS_STYLE style, pLock lock);

bool TSMS_GUI_equalsGrid(TSMS_GRID_INFO grid1, TSMS_GRID_INFO grid2);

pRenderOperation TSMS_GUI_createCharRenderOperation(uint16_t x, uint16_t y, TSMS_FONT_TYPE fontType, void *font, char c, TSMS_FONT_SIZE size);

pRenderOperation TSMS_GUI_createFillRectRenderOperation(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

TSMS_RESULT TSMS_GUI_cancelRenderOperation(pGuiElement element, pRenderOperation operation, TSMS_STYLE style, pLock lock);

TSMS_RESULT TSMS_GUI_releaseRenderOperation(pRenderOperation operation);

#endif //TSMS_GUI_H
