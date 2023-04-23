#include "tsms_gui_style.h"
#include "tsms_util.h"
#include "tsms_font.h"

TSMS_STYLE TSMS_STYLE_DEFAULT;

TSMS_RESULT TSMS_GUI_STYLE_init() {
	TSMS_STYLE_DEFAULT = (TSMS_STYLE){{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {255, 255, 255}
			, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, TSMS_STYLE_AUTO, {TSMS_ARRAY_FONT, 1, TSMS_FONT_defaultFont}};
	return TSMS_SUCCESS;
}
