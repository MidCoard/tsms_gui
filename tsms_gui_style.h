#ifndef TSMS_GUI_STYLE_H
#define TSMS_GUI_STYLE_H

#define TSMS_STYLE_AUTO 0xFFFF
#define TSMS_STYLE_INHERIT 0xFFFE
#define TSMS_STYLE_FONT_TYPE_INHERIT TSMS_FONT_TYPE_INVALID
#define TSMS_STYLE_FONT_SIZE_INHERIT 0xFF
#define TSMS_STYLE_FONT_INHERIT TSMS_NULL
#define TSMS_STYLE_GRID_TYPE_INHERIT 0xFF

typedef enum {
	TSMS_STYLE_POSITION_RELATIVE = 0, TSMS_STYLE_POSITION_ABSOLUTE, TSMS_STYLE_POSITION_INHERIT
} TSMS_STYLE_POSITION;

typedef enum {
	TSMS_GRID_TYPE_DEFAULT
} TSMS_GRID_TYPE;

typedef enum {
	TSMS_DISPLAY_TYPE_NONE = 0, TSMS_DISPLAY_TYPE_BLOCK
} TSMS_DISPLAY_TYPE;

typedef struct TSMS_GUI_ELEMENT* pGuiElement;

#include "tsms_util.h"
#include "tsms_font.h"

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
	TSMS_COLOR color;
} TSMS_STYLE_MARGIN;

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
	TSMS_COLOR color;
} TSMS_STYLE_PADDING;

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
	TSMS_COLOR color;
} TSMS_STYLE_BORDER;

typedef struct {
	TSMS_FONT_TYPE type;
	TSMS_FONT_SIZE size;
	void * font;
	TSMS_COLOR color;
} TSMS_STYLE_FONT;

typedef struct {
	TSMS_STYLE_MARGIN margin;
	TSMS_STYLE_BORDER border;
	TSMS_STYLE_PADDING padding;
	TSMS_COLOR backgroundColor;
	TSMS_COLOR pressedColor;
	TSMS_COLOR releasedColor;
	uint16_t width;
	uint16_t height;
	uint16_t maxWidth;
	uint16_t maxHeight;
	uint16_t minWidth;
	uint16_t minHeight;
	TSMS_STYLE_FONT font;
	TSMS_STYLE_POSITION position;
	uint16_t zIndex; //  todo not take into consideration
	uint16_t top;
	uint16_t left;
	TSMS_GRID_TYPE gridType;
	TSMS_DISPLAY_TYPE displayType;
} TSMS_STYLE;

extern TSMS_STYLE TSMS_STYLE_DEFAULT;

extern TSMS_STYLE TSMS_STYLE_DEFAULT_BUTTON;

extern TSMS_STYLE TSMS_STYLE_DEFAULT_TEXT;

uint16_t TSMS_STYLE_Y_ATTACHMENT(TSMS_STYLE style);

uint16_t TSMS_STYLE_X_ATTACHMENT(TSMS_STYLE style);

uint16_t TSMS_STYLE_getBoxWidth(TSMS_STYLE style, uint16_t width, uint16_t parentWidth);

uint16_t TSMS_STYLE_getBoxHeight(TSMS_STYLE style, uint16_t height, uint16_t parentHeight);

uint16_t TSMS_STYLE_left(TSMS_STYLE style);

uint16_t TSMS_STYLE_top(TSMS_STYLE style);

bool TSMS_STYLE_equals(TSMS_STYLE style1, TSMS_STYLE style2);

TSMS_STYLE TSMS_STYLE_getStyle(pGuiElement element);

bool TSMS_STYLE_equalsFont(TSMS_STYLE_FONT font1, TSMS_STYLE_FONT font2);

#endif //TSMS_GUI_STYLE_H
