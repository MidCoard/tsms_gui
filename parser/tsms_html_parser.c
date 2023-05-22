#include "tsms_html_parser.h"

pHtml TSMS_HTML_parse(pString html) {
	pHtml htmlObject = (pHtml) malloc(sizeof(tHtml));
	if (htmlObject == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for htmlObject");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	pString temp = TSMS_STRING_create();
	for (TSMS_POS i = 0; i < html->length; i++) {
		char c = html->cStr[i];


	}

	return htmlObject;
}