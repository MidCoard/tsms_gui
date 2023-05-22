#include "tsms_gui_parser.h"
#include "tsms_html_parser.h"

pGui TSMS_GUI_parse(TSMS_DPHP display, pString html, pString css, pString js) {
	pGui gui = TSMS_GUI_create(display);
	pHtml htmlObject = TSMS_HTML_parse(html);

	return gui;
}