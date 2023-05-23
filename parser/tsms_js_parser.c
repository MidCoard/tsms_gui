#include "tsms_js_parser.h"
#include "tsms.h"

pJsNode TSMS_JS_parse(pString js) {
	pJsNode node = TSMS_malloc(sizeof(tJsNode));
	if (node == TSMS_NULL)
		return TSMS_NULL;
	node->code = js;
	return node;
}

pString TSMS_JS_compile(pJsNode js) {
	pString cCode = TSMS_STRING_create();
	// todo
	return cCode;
}

TSMS_RESULT TSMS_JS_release(pJsNode node) {
	if (node == TSMS_NULL)
		return TSMS_ERROR;
	free(node);
	return TSMS_SUCCESS;
}