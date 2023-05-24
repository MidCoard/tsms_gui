#include "tsms_gui_parser.h"
#include "tsms_html_parser.h"
#include "tsms_css_parser.h"
#include "tsms_js_parser.h"
#include "malloc.h"

TSMS_INLINE void __tsms_internal_init_style(pHtmlNode node, TSMS_LP cssNodes) {
	pString style = TSMS_MAP_get(node->attributes, TSMS_STRING_static("style"));
	if (style != TSMS_NULL) {
		TSMS_LP cssList = TSMS_CSS_parse(style);
		if (cssList->length == 1) {
			pCssNode cssNode = cssList->list[0];
			TSMS_LIST_add(cssNodes, cssNode);
			TSMS_MI it = TSMS_MAP_iterator(cssNode->attributes);
			while (TSMS_MAP_hasNext(&it)) {
				TSMS_ME entry = TSMS_MAP_next(&it);
				TSMS_MAP_put(node->style, entry.key, entry.value);
			}
		}
		TSMS_LIST_release(cssList);
	}
	for (TSMS_POS i = 0; i < node->children->length; i++)
		__tsms_internal_init_style(node->children->list[i], cssNodes);
}

TSMS_INLINE void __tsms_internal_generate_html(pHtmlNode node) {

}

pString TSMS_GUI_parse(pString html, pString css) {
	pHtml htmlObject = TSMS_HTML_parse(html);
	TSMS_LP cssNodes = TSMS_CSS_parse(css);
	for (TSMS_POS i = 0; i < cssNodes->length; i++) {
		pCssNode cssNode = cssNodes->list[i];
		TSMS_LP elements = TSMS_HTML_getElementsBySelector(htmlObject, cssNode->selector);
		TSMS_MI it = TSMS_MAP_iterator(cssNode->attributes);
		while (TSMS_MAP_hasNext(&it)) {
			TSMS_ME entry = TSMS_MAP_next(&it);
			for (TSMS_POS j = 0; j < elements->length; j++) {
				pHtmlNode element = elements->list[j];
				TSMS_MAP_put(element->style, entry.key, entry.value);
			}
		}
		TSMS_LIST_release(elements);
	}
	__tsms_internal_init_style(htmlObject->root, cssNodes);
	__tsms_internal_generate_html(htmlObject->root);
	for (TSMS_POS i = 0; i < cssNodes->length; i++)
		TSMS_CSS_release(cssNodes->list[i]);
	TSMS_LIST_release(cssNodes);
	TSMS_HTML_release(htmlObject);
	return TSMS_STRING_create();
}