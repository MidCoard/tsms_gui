#ifndef TSMS_CSS_PARSER_H
#define TSMS_CSS_PARSER_H

typedef struct TSMS_CSS_NODE tCssNode;
typedef tCssNode * pCssNode;

#include "tsms_string.h"
#include "tsms_map.h"

struct TSMS_CSS_NODE {
	pString selector;
	TSMS_MP attributes;
};

TSMS_LP TSMS_CSS_parse(pString css);

TSMS_RESULT TSMS_CSS_release(pCssNode node);

#endif //TSMS_CSS_PARSER_H
