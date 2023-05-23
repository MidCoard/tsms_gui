#include "tsms_html_parser.h"
#include "tsms.h"

TSMS_INLINE void __tsms_internal_release_node(pHtmlNode node) {
	TSMS_STRING_release(node->tag);
	if (node->attributes != TSMS_NULL) {
		TSMS_MI it = TSMS_MAP_iterator(node->attributes);
		while (TSMS_MAP_hasNext(&it)) {
			TSMS_ME entry = TSMS_MAP_next(&it);
			TSMS_STRING_release(entry.key);
			TSMS_STRING_release(entry.value);
		}
	}
	TSMS_MAP_release(node->attributes);
	if (node->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < node->children->length; i++)
			__tsms_internal_release_node(node->children->list[i]);
	TSMS_MAP_release(node->style);
	TSMS_LIST_release(node->children);
	free(node);
}

TSMS_INLINE pHtmlNode __tsms_internal_create_node(pString tag) {
	pHtmlNode node = (pHtmlNode) TSMS_malloc(sizeof(tHtmlNode));
	if (node == TSMS_NULL)
		return TSMS_NULL;
	node->tag = tag;
	node->attributes = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                                   (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	node->children = TSMS_LIST_create(10);
	node->parent = TSMS_NULL;
	node->style = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                              (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	if (node->children == TSMS_NULL || node->attributes == TSMS_NULL || node->style == TSMS_NULL) {
		__tsms_internal_release_node(node);
		return TSMS_NULL;
	}
	return node;
}

TSMS_INLINE pHtmlNode __tsms_internal_parse_node(pString tagValue, pHtml html) {
	pString temp = TSMS_STRING_create();
	bool inQuote = false;
	bool inEscape = false;
	TSMS_LP list = TSMS_LIST_create(10);
	for (TSMS_POS i = 0; i < tagValue->length; i++) {
		char c = tagValue->cStr[i];
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
		} else if (c == ' ') {
			TSMS_LIST_add(list, TSMS_STRING_clone(temp));
			TSMS_STRING_clear(temp);
		} else if (c == '\\') {
			inEscape = true;
		} else {
			TSMS_STRING_appendChar(temp, c);
		}
	}
	if (inQuote || inEscape) {
		TSMS_STRING_release(temp);
		for (TSMS_POS i = 0; i < list->length; i++)
			TSMS_STRING_release(list->list[i]);
		TSMS_LIST_release(list);
		return TSMS_NULL;
	}
	if (temp->length > 0)
		TSMS_LIST_add(list, TSMS_STRING_clone(temp));
	TSMS_STRING_release(temp);
	if (list->length == 0) {
		TSMS_LIST_release(list);
		return TSMS_NULL;
	}
	pHtmlNode node = __tsms_internal_create_node(list->list[0]);
	for (TSMS_POS i = 1; i < list->length; i++) {
		pString attribute = list->list[i];
		TSMS_POS index = TSMS_STRING_indexOf(attribute, '=');
		TSMS_POS quoteIndex = TSMS_STRING_indexOf(attribute, '"');
		if ((index <= quoteIndex || quoteIndex == -1) && index != attribute->length - 1) {
			if (index == -1) {
				pString key = TSMS_STRING_subString(attribute, 0, attribute->length);
				if (TSMS_STRING_equals(key, TSMS_STRING_static("id")))
					TSMS_MAP_put(html->ids, TSMS_NULL, node);
				TSMS_MAP_put(node->attributes, key, TSMS_NULL);
			} else {
				pString key = TSMS_STRING_subString(attribute, 0, index);
				pString value = TSMS_STRING_subString(attribute, index + 1, attribute->length);
				if (TSMS_STRING_equals(key, TSMS_STRING_static("id")))
					TSMS_MAP_put(html->ids, value, node);
				TSMS_MAP_put(node->attributes, key, value);
			}
		}
		TSMS_STRING_release(attribute);
	}
	TSMS_LIST_release(list);
	return node;
}

void TSMS_HTML_release(pHtml html) {
	__tsms_internal_release_node(html->root);
	TSMS_MAP_release(html->ids);
	free(html);
}

pHtml TSMS_HTML_parse(pString html) {
	pHtml htmlObject = (pHtml) TSMS_malloc(sizeof(tHtml));
	if (htmlObject == TSMS_NULL)
		return TSMS_NULL;
	htmlObject->root = __tsms_internal_create_node(TSMS_STRING_static("root"));
	htmlObject->ids = TSMS_MAP_create(16, (TSMS_HASH_FUNCTION) TSMS_STRING_hash,
	                                  (TSMS_COMPARE_FUNCTION) TSMS_STRING_compare);
	if (htmlObject->root == TSMS_NULL) {
		TSMS_HTML_release(htmlObject);
		return TSMS_NULL;
	}
	pString temp = TSMS_STRING_create();
	pHtmlNode currentNode = htmlObject->root;
	bool inTag = false;
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
		} else if (c == '"') {
			inQuote = !inQuote;
			TSMS_STRING_appendChar(temp, c);
		} else if (inQuote) {
			TSMS_STRING_appendChar(temp, c);
		} else if (c == '\\') {
			inEscape = true;
		} else if (!inTag) {
			if (c == '<') {
				pString trim = TSMS_STRING_trim(temp);
				if (trim->length > 0) {
					pHtmlNode node = __tsms_internal_create_node(TSMS_STRING_createWithString("text"));
					TSMS_MAP_put(node->attributes, TSMS_STRING_createWithString("value"), TSMS_STRING_clone(trim));
				}
				TSMS_STRING_release(trim);
				TSMS_STRING_clear(temp);
				inTag = true;
			} else {
				TSMS_STRING_appendChar(temp, c);
			}
		} else if (inTag) {
			if (c == '>') {
				pString trim = TSMS_STRING_trim(temp);
				if (trim->length > 0) {
					pString tag = TSMS_STRING_createWithChar('/');
					TSMS_STRING_append(tag, currentNode->tag);
					if (TSMS_STRING_equals(tag, trim)) {
						if (currentNode->parent != TSMS_NULL) {
							currentNode = currentNode->parent;
						}
					} else {
						pHtmlNode node = __tsms_internal_parse_node(trim, htmlObject);
						if (node != TSMS_NULL) {
							TSMS_LIST_add(currentNode->children, node);
							node->parent = currentNode;
							currentNode = node;
						}
					}
				}
				TSMS_STRING_release(trim);
				TSMS_STRING_clear(temp);
				inTag = false;
			} else {
				TSMS_STRING_appendChar(temp, c);
			}
		}
	}
	TSMS_STRING_release(temp);

	return htmlObject;
}

TSMS_LP TSMS_HTML_getElementsBySelector(pHtml html, pString query) {
	TSMS_LP list = TSMS_LIST_create(10);
	if (list == TSMS_NULL)
		return TSMS_NULL;
	// analyse selector todo
	return list;
}

pHtmlNode TSMS_HTML_getElementById(pHtml html, pString id) {
	return TSMS_MAP_get(html->ids, id);
}