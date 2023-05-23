#include "tsms_html_parser.h"
#include "tsms.h"

TSMS_INLINE void __tsms_internal_release_node(pHtmlNode node) {
	TSMS_STRING_release(node->tag);
	TSMS_MAP_release(node->attributes);
	if (node->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < node->children->length; i++)
			__tsms_internal_release_node(node->children->list[i]);
	TSMS_LIST_release(node->children);
	free(node);
}

TSMS_INLINE void __tsms_internal_release(pHtml html) {
	__tsms_internal_release_node(html->root);
	free(html);
}

TSMS_INLINE pHtmlNode __tsms_internal_create_node(pString tag) {
	pHtmlNode node = (pHtmlNode) TSMS_malloc(sizeof(tHtmlNode));
	if (node == TSMS_NULL)
		return TSMS_NULL;
	node->tag = tag;
	node->attributes = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                                   (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	node->children = TSMS_LIST_create(10);
	if (node->children == TSMS_NULL || node->attributes == TSMS_NULL) {
		__tsms_internal_release_node(node);
		return TSMS_NULL;
	}
	return node;
}

pHtml TSMS_HTML_parse(pString html) {
	pHtml htmlObject = (pHtml) TSMS_malloc(sizeof(tHtml));
	if (htmlObject == TSMS_NULL)
		return TSMS_NULL;
	htmlObject->root = __tsms_internal_create_node(TSMS_STRING_static("root"));
	if (htmlObject->root == TSMS_NULL) {
		__tsms_internal_release(htmlObject);
		return TSMS_NULL;
	}
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