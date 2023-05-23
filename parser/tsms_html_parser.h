#ifndef TSMS_HTML_PARSER_H
#define TSMS_HTML_PARSER_H

typedef struct TSMS_HTML tHtml;
typedef tHtml* pHtml;

typedef struct TSMS_HTML_NODE tHtmlNode;
typedef tHtmlNode* pHtmlNode;

#include "tsms_string.h"
#include "tsms_list.h"
#include "tsms_map.h"

struct TSMS_HTML {
	pHtmlNode root;
	TSMS_MP ids;
};

struct TSMS_HTML_NODE {
	TSMS_MP attributes;
	TSMS_LP children;
	pString tag;
	pHtmlNode parent;
	TSMS_MP style;
};

void TSMS_HTML_release(pHtml html);

pHtml TSMS_HTML_parse(pString html);

TSMS_LP TSMS_HTML_getElementsBySelector(pHtml html, pString query);

pHtmlNode TSMS_HTML_getElementById(pHtml html, pString id);

#endif //TSMS_HTML_PARSER_H
