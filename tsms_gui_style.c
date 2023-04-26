#include "tsms_gui.h"
#include "tsms_gui_style.h"

TSMS_STYLE TSMS_STYLE_DEFAULT;

TSMS_STYLE TSMS_STYLE_DEFAULT_BUTTON;

TSMS_STYLE TSMS_STYLE_DEFAULT_TEXT;

TSMS_STYLE TSMS_STYLE_DEFAULT_CONTAINER;

TSMS_STYLE TSMS_STYLE_DEFAULT_ROW;

TSMS_STYLE TSMS_STYLE_DEFAULT_COLUMN;


TSMS_RESULT TSMS_GUI_STYLE_init() {
	TSMS_STYLE_DEFAULT = (TSMS_STYLE) {{0, 0, 0, 0, &TSMS_COLOR_WHITE}, {0, 0, 0, 0, &TSMS_COLOR_WHITE},
	                                   {0, 0, 0, 0, &TSMS_COLOR_WHITE}, &TSMS_COLOR_WHITE, &TSMS_COLOR_WHITE,
	                                   &TSMS_COLOR_WHITE, TSMS_STYLE_AUTO,
	                                   TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO,
	                                   TSMS_STYLE_AUTO,
	                                   {TSMS_FONT_TYPE_ARRAY, 1, TSMS_FONT_ARRAY_DEFAULT_FONT, &TSMS_COLOR_BLACK},
	                                   TSMS_STYLE_POSITION_RELATIVE, 0, 0, 0, TSMS_STYLE_DISPLAY_BLOCK
	};

	TSMS_STYLE_DEFAULT_BUTTON = (TSMS_STYLE) {
			{0, 2, 0, 2, &TSMS_COLOR_WHITE}, {2, 2, 2, 2, &TSMS_COLOR_BLACK}, {7, 7, 7, 7, &TSMS_COLOR_WHITE},
			&TSMS_COLOR_WHITE, &TSMS_COLOR_GRAY, &TSMS_COLOR_WHITE, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO,
			TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT,
			TSMS_STYLE_INHERIT, {TSMS_STYLE_FONT_TYPE_INHERIT, TSMS_STYLE_FONT_SIZE_INHERIT, TSMS_STYLE_FONT_INHERIT,
			                     TSMS_STYLE_COLOR_INHERT}, TSMS_STYLE_POSITION_RELATIVE, TSMS_STYLE_INHERIT, 0, 0,
			TSMS_STYLE_DISPLAY_BLOCK
	};

	TSMS_STYLE_DEFAULT_TEXT = (TSMS_STYLE) {
			{0, 0, 0, 0, &TSMS_COLOR_WHITE}, {0, 0, 0, 0, &TSMS_COLOR_WHITE}, {0, 0, 0, 0, &TSMS_COLOR_WHITE},
			TSMS_STYLE_COLOR_INHERT, &TSMS_COLOR_WHITE, &TSMS_COLOR_WHITE, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO,
			TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT,
			TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT,
			{TSMS_STYLE_FONT_TYPE_INHERIT, TSMS_STYLE_FONT_SIZE_INHERIT, TSMS_STYLE_FONT_INHERIT,
			 TSMS_STYLE_COLOR_INHERT},
			TSMS_STYLE_POSITION_RELATIVE, TSMS_STYLE_INHERIT, 0, 0,
			TSMS_STYLE_DISPLAY_BLOCK
	};

	TSMS_STYLE_DEFAULT_CONTAINER = (TSMS_STYLE) {
			{0, 0, 0, 0, &TSMS_COLOR_WHITE}, {0, 0, 0, 0, &TSMS_COLOR_WHITE}, {0, 0, 0, 0, &TSMS_COLOR_WHITE},
			&TSMS_COLOR_WHITE, &TSMS_COLOR_WHITE, &TSMS_COLOR_WHITE, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO,
			TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT,
			TSMS_STYLE_INHERIT, TSMS_STYLE_INHERIT,
			{TSMS_STYLE_FONT_TYPE_INHERIT, TSMS_STYLE_FONT_SIZE_INHERIT, TSMS_STYLE_FONT_INHERIT,
			 TSMS_STYLE_COLOR_INHERT},
			TSMS_STYLE_POSITION_RELATIVE, TSMS_STYLE_INHERIT, 0, 0,
			TSMS_STYLE_DISPLAY_BLOCK
	};

	TSMS_STYLE_DEFAULT_ROW = TSMS_STYLE_DEFAULT_CONTAINER;

	TSMS_STYLE_DEFAULT_COLUMN = TSMS_STYLE_DEFAULT_CONTAINER;

	return TSMS_SUCCESS;
}

uint16_t TSMS_STYLE_Y_ATTACHMENT(TSMS_STYLE style) {
	return style.margin.top + style.padding.top + style.border.top + style.margin.bottom + style.padding.bottom +
	       style.border.bottom;
}

uint16_t TSMS_STYLE_X_ATTACHMENT(TSMS_STYLE style) {
	return style.margin.left + style.padding.left + style.border.left + style.margin.right + style.padding.right +
	       style.border.right;
}

uint16_t TSMS_STYLE_getBoxWidth(TSMS_STYLE style, uint16_t width, uint16_t parentWidth) {
	if (style.width != TSMS_STYLE_AUTO)
		width = style.width;
	if (style.minWidth != TSMS_STYLE_AUTO && width < style.minWidth)
		width = style.minWidth;
	if (style.maxWidth != TSMS_STYLE_AUTO && width > style.maxWidth)
		width = style.maxWidth;
	if (width + TSMS_STYLE_X_ATTACHMENT(style) > parentWidth)
		width = parentWidth - TSMS_STYLE_X_ATTACHMENT(style);
	return width;
}

uint16_t TSMS_STYLE_getBoxHeight(TSMS_STYLE style, uint16_t height, uint16_t parentHeight) {
	if (style.height != TSMS_STYLE_AUTO)
		height = style.height;
	if (style.minHeight != TSMS_STYLE_AUTO && height < style.minHeight)
		height = style.minHeight;
	if (style.maxHeight != TSMS_STYLE_AUTO && height > style.maxHeight)
		height = style.maxHeight;
	if (height + TSMS_STYLE_Y_ATTACHMENT(style) > parentHeight)
		height = parentHeight - TSMS_STYLE_Y_ATTACHMENT(style);
	return height;
}

uint16_t TSMS_STYLE_left(TSMS_STYLE style) {
	return style.margin.left + style.padding.left + style.border.left;
}

uint16_t TSMS_STYLE_top(TSMS_STYLE style) {
	return style.margin.top + style.padding.top + style.border.top;
}

bool TSMS_STYLE_equals(TSMS_STYLE style1, TSMS_STYLE style2) {
	uint8_t *style1Pointer = (uint8_t *) &style1;
	uint8_t *style2Pointer = (uint8_t *) &style2;
	return memcmp(style1Pointer, style2Pointer, sizeof(TSMS_STYLE)) == 0;
}

TSMS_STYLE TSMS_STYLE_getStyle(pGuiElement element) {
	if (element->parent == TSMS_NULL)
		return element->computedStyle = TSMS_MUTABLE_STYLE_get(element->style);
	TSMS_STYLE parentStyle = element->parent->computedStyle;
	TSMS_STYLE style = TSMS_MUTABLE_STYLE_get(element->style);
	if (style.width == TSMS_STYLE_INHERIT)
		style.width = parentStyle.width;
	if (style.height == TSMS_STYLE_INHERIT)
		style.height = parentStyle.height;
	if (style.minWidth == TSMS_STYLE_INHERIT)
		style.minWidth = parentStyle.minWidth;
	if (style.minHeight == TSMS_STYLE_INHERIT)
		style.minHeight = parentStyle.minHeight;
	if (style.maxWidth == TSMS_STYLE_INHERIT)
		style.maxWidth = parentStyle.maxWidth;
	if (style.maxHeight == TSMS_STYLE_INHERIT)
		style.maxHeight = parentStyle.maxHeight;
	if (style.font.type == TSMS_STYLE_FONT_TYPE_INHERIT)
		style.font = parentStyle.font;
	if (style.font.size == TSMS_STYLE_FONT_SIZE_INHERIT)
		style.font.size = parentStyle.font.size;
	if (style.font.font == TSMS_STYLE_FONT_INHERIT)
		style.font.font = parentStyle.font.font;
	if (style.font.color == TSMS_STYLE_COLOR_INHERT)
		style.font.color = parentStyle.font.color;
	if (style.position == TSMS_STYLE_POSITION_INHERIT)
		style.position = parentStyle.position;
	if (style.backgroundColor == TSMS_STYLE_COLOR_INHERT)
		style.backgroundColor = parentStyle.backgroundColor;
	if (style.zIndex == TSMS_STYLE_INHERIT)
		style.zIndex = parentStyle.zIndex;
	if (parentStyle.display == TSMS_STYLE_DISPLAY_NONE)
		style.display = TSMS_STYLE_DISPLAY_NONE;
	return element->computedStyle = style;
}

bool TSMS_STYLE_equalsFont(TSMS_STYLE_FONT font1, TSMS_STYLE_FONT font2) {
	uint8_t *font1Pointer = (uint8_t *) &font1;
	uint8_t *font2Pointer = (uint8_t *) &font2;
	return memcmp(font1Pointer, font2Pointer, sizeof(TSMS_STYLE_FONT)) == 0;
}

uint16_t TSMS_STYLE_elementWidth(TSMS_STYLE style, uint16_t parentWidth) {
	if (style.width == TSMS_STYLE_AUTO) {
		uint16_t maxWidth =
				style.maxWidth == TSMS_STYLE_AUTO ? parentWidth - TSMS_STYLE_X_ATTACHMENT(style) : style.maxWidth;
		return min(maxWidth, parentWidth - TSMS_STYLE_X_ATTACHMENT(style));
	}
	return min(style.width, parentWidth - TSMS_STYLE_X_ATTACHMENT(style));
}

uint16_t TSMS_STYLE_elementHeight(TSMS_STYLE style, uint16_t parentHeight) {
	if (style.height == TSMS_STYLE_AUTO) {
		uint16_t maxHeight =
				style.maxHeight == TSMS_STYLE_AUTO ? parentHeight - TSMS_STYLE_Y_ATTACHMENT(style) : style.maxHeight;
		return min(maxHeight, parentHeight - TSMS_STYLE_Y_ATTACHMENT(style));
	}
	return min(style.height, parentHeight - TSMS_STYLE_Y_ATTACHMENT(style));
}