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
};

struct TSMS_HTML_NODE {
	TSMS_MP attributes;
	TSMS_LP children;
	pString tag;
	pHtmlNode parent;
};

pHtml TSMS_HTML_parse(pString html);

#endif //TSMS_HTML_PARSER_H
