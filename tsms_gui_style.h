#ifndef TSMS_GUI_STYLE_H
#define TSMS_GUI_STYLE_H

#define TSMS_STYLE_AUTO 0xFFFF

#include "tsms_util.h"

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
} TSMS_STYLE_MARGIN;

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
} TSMS_STYLE_PADDING;

typedef struct {
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t left;
} TSMS_STYLE_BORDER;

typedef struct {
	TSMS_FONT_TYPE type;
	TSMS_FONT_SIZE size;
	void * font;
} TSMS_STYLE_FONT;

typedef struct {
	TSMS_STYLE_MARGIN margin;
	TSMS_STYLE_PADDING padding;
	TSMS_STYLE_BORDER border;
	TSMS_COLOR backgroundColor;
	uint16_t width;
	uint16_t height;
	uint16_t maxWidth;
	uint16_t maxHeight;
	uint16_t minWidth;
	uint16_t minHeight;
	TSMS_STYLE_FONT font;
} TSMS_STYLE;

extern TSMS_STYLE TSMS_STYLE_DEFAULT;


#endif //TSMS_GUI_STYLE_H
