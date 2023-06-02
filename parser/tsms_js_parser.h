#ifndef TSMS_JS_PARSER_H
#define TSMS_JS_PARSER_H

#include "tsms_string.h"
#include "tsms_util.h"

typedef struct TSMS_JS_CONTEXT tJSContext;
typedef tJSContext* pJSContext;

typedef struct TSMS_JS_FUNCTION_DECLARATION tJSFunctionDeclaration;
typedef tJSFunctionDeclaration* pJSFunctionDeclaration;

typedef struct TSMS_JS_FUNCTION_CALL tJSFunctionCall;
typedef tJSFunctionCall* pJSFunctionCall;

typedef enum {
	TSMS_JS_TYPE_UNDEFINED = 0,
	TSMS_JS_TYPE_NULL,
	TSMS_JS_TYPE_BOOL,
	TSMS_JS_TYPE_NUMBER,
	TSMS_JS_TYPE_STRING,
	TSMS_JS_TYPE_OBJECT,
	TSMS_JS_TYPE_FUNCTION
} TSMS_JS_TYPE;

struct TSMS_JS_FUNCTION_DECLARATION {
	pString name;
	TSMS_LP paramTypes;
	TSMS_LP paramNames;
};

struct TSMS_JS_FUNCTION_CALL {
	pJSFunctionDeclaration declaration;
	TSMS_LP paramValues;
};

struct TSMS_JS_CONTEXT {
	TSMS_LP declarations;
};

pString TSMS_JS_compile(pString js);

TSMS_PAIR TSMS_JS_eval(pString code, pJSContext context);

bool TSMS_JS_compileToBool(pString js);

#endif //TSMS_JS_PARSER_H
