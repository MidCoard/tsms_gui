#include "tsms_gui_style.h"
#include "tsms_util.h"
#include "tsms_font.h"

TSMS_STYLE TSMS_STYLE_DEFAULT;

TSMS_RESULT TSMS_GUI_STYLE_init() {
	TSMS_STYLE_DEFAULT = (TSMS_STYLE){{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {255, 255, 255}
			, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, {TSMS_ARRAY_FONT, 1, TSMS_FONT_defaultFont}
	, TSMS_STYLE_POSITION_RELATIVE, 0 , 0 ,0};
	return TSMS_SUCCESS;
}

uint16_t TSMS_STYLE_Y_ATTACHMENT(TSMS_STYLE style) {
	return style.margin.top + style.padding.top + style.border.top + style.margin.bottom + style.padding.bottom + style.border.bottom;
}

uint16_t TSMS_STYLE_X_ATTACHMENT(TSMS_STYLE style) {
	return style.margin.left + style.padding.left + style.border.left + style.margin.right + style.padding.right + style.border.right;
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