#include "tsms_html_parser.h"

TSMS_INLINE pHtmlNode __tsms_internal_create_node(pString tag) {
	pHtmlNode node = (pHtmlNode) malloc(sizeof(tHtmlNode));
	if (node == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for node");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	node->tag = tag;
	node->attributes = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                                   (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	if (node->attributes == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for node attributes");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);

		return TSMS_NULL;
	}
	node->children = TSMS_LIST_create(10);
	if (node->children == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for node children");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	return node;
}

pHtml TSMS_HTML_parse(pString html) {
	pHtml htmlObject = (pHtml) malloc(sizeof(tHtml));
	if (htmlObject == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for htmlObject");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	htmlObject->root = __tsms_internal_create_node(TSMS_STRING_static("root"));
	pString temp = TSMS_STRING_create();
	bool inTag = false;
	bool inValue = false;
	bool inEscape = false;
	bool inQuote = false;
	for (TSMS_POS i = 0; i < html->length; i++) {
		char c = html->cStr[i];
		if (inEscape) {
			switch (c) {
				case '\'':
				case '"':
				case '\\':
					TSMS_STRING_appendChar(temp, c);
					break;
				case 'n':
					TSMS_STRING_appendChar(temp, '\n');
					break;
				case 'r':
					TSMS_STRING_appendChar(temp, '\r');
					break;
				case 't':
					TSMS_STRING_appendChar(temp, '\t');
					break;
				case 'b':
					TSMS_STRING_appendChar(temp, '\b');
					break;
				case 'f':
					TSMS_STRING_appendChar(temp, '\f');
					break;
				case 'v':
					TSMS_STRING_appendChar(temp, '\v');
					break;
				case '0':
					TSMS_STRING_appendChar(temp, '\0');
					break;
				case 'a':
					TSMS_STRING_appendChar(temp, '\a');
					break;
				default:
					TSMS_STRING_appendChar(temp, c);
					break;
			}
			inEscape = false;
		} else if (!inTag && !inValue) {
			if (c == '<')
				inTag = true;
		} else if (inTag) {
			if (c == '\\') {
				inEscape = true;
			} else if (c == '>') {

				inTag = false;
			} else if (c == ' ') {
				inValue = true;
			} else {
				TSMS_STRING_appendChar(temp, c);
			}
		}

	}

	return htmlObject;
}