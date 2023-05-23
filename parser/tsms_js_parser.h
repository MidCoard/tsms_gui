#ifndef TSMS_JS_PARSER_H
#define TSMS_JS_PARSER_H

typedef struct TSMS_JS_NODE tJsNode;
typedef tJsNode * pJsNode;

#include "tsms_string.h"

struct TSMS_JS_NODE {
	pString code;
};

pJsNode TSMS_JS_parse(pString js);

pString TSMS_JS_compile(pJsNode js);

TSMS_RESULT TSMS_JS_release(pJsNode node);

#endif //TSMS_JS_PARSER_H
