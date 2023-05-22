#ifndef TSMS_HTML_PARSER_H
#define TSMS_HTML_PARSER_H

typedef struct TSMS_HTML tHtml;
typedef tHtml* pHtml;

typedef struct TSMS_HTML_Node tHtmlNode;

#include "tsms_string.h"

struct TSMS_HTML {
};

pHtml TSMS_HTML_parse(pString html);

#endif //TSMS_HTML_PARSER_H
