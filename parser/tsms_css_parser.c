#include "tsms_css_parser.h"
#include "tsms.h"

TSMS_INLINE pCssNode __tsms_internal_create_node(pString selector) {
	pCssNode node = TSMS_malloc(sizeof(tCssNode));
	if (node == TSMS_NULL)
		return TSMS_NULL;
	node->selector = selector;
	node->attributes = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                                   (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	if (node->attributes == TSMS_NULL) {
		TSMS_CSS_release(node);
		return TSMS_NULL;
	}
	return node;
}

TSMS_LP TSMS_CSS_parse(pString css) {
	TSMS_LP list = TSMS_LIST_create(10);
	if (list == TSMS_NULL)
		return TSMS_NULL;
	bool inQuote = false;
	bool inQuery = true;
	bool inEscape = false;
	pString temp = TSMS_STRING_create();
	pString key = TSMS_NULL;
	pCssNode current = TSMS_NULL;
	for (TSMS_POS i = 0; i < css->length; i++) {
		char c = css->cStr[i];
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
		} else if (c == '"') {
			inQuote = !inQuote;
			TSMS_STRING_appendChar(temp, c);
		} else if (inQuote) {
			TSMS_STRING_appendChar(temp, c);
		} else if (c == '\\') {
			inEscape = true;
		} else if (inQuery) {
			if (c == '{') {
				current = __tsms_internal_create_node(TSMS_STRING_trim(temp));
				TSMS_STRING_clear(temp);
				inQuery = false;
			} else {
				TSMS_STRING_appendChar(temp, c);
			}
		} else if (!inQuery) {
			if (c == '}') {
				TSMS_LIST_add(list, current);
				current = TSMS_NULL;
			} else if (c == ':') {
				key = TSMS_STRING_trim(temp);
				TSMS_STRING_clear(temp);
			} else if (c == ';') {
				pString value = TSMS_STRING_trim(temp);
				TSMS_MAP_put(current->attributes, key, value);
				key = TSMS_NULL;
				TSMS_STRING_clear(temp);
			} else {
				TSMS_STRING_appendChar(temp, c);
			}
		}
	}
	TSMS_STRING_release(temp);
	return list;
}

TSMS_RESULT TSMS_CSS_release(pCssNode node) {
	if (node == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_STRING_release(node->selector);
	if (node->attributes != TSMS_NULL) {
		TSMS_MI it = TSMS_MAP_iterator(node->attributes);
		while (TSMS_MAP_hasNext(&it)) {
			TSMS_ME entry = TSMS_MAP_next(&it);
			TSMS_STRING_release(entry.key);
			TSMS_STRING_release(entry.value);
		}
	}
	TSMS_MAP_release(node->attributes);
	free(node);
	return TSMS_SUCCESS;
}