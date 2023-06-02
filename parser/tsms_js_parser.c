#include "tsms_js_parser.h"
#include "tsms.h"

pString TSMS_JS_compile(pString code) {
	pString cCode = TSMS_STRING_create();
	// todo
	return cCode;
}

TSMS_PAIR TSMS_JS_eval(pString code, pJSContext context) {
	// ignore context
	TSMS_JS_TYPE type = TSMS_JS_TYPE_UNDEFINED;
	bool inQuote = false;
	bool inEscape = false;
	bool inBracket = false;
	pString temp = TSMS_STRING_create();
	pString key = TSMS_NULL;
	for (TSMS_POS i = 0; i < code->length; i++) {
		char c = code->cStr[i];
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
		} else if (c == '\\') {
			inEscape = true;
		} else if (c == '"') {
			inQuote = !inQuote;
			if (!inBracket)
				if (type == TSMS_JS_TYPE_UNDEFINED)
					type = TSMS_JS_TYPE_STRING;
		} else if (c == '{' && !inQuote) {
			inBracket = true;
			if (type == TSMS_JS_TYPE_UNDEFINED)
				type = TSMS_JS_TYPE_OBJECT;
		} else if (c == '}' && !inQuote) {
			if (key != TSMS_NULL) {

			}
			inBracket = false;
		} else if (inQuote || c != ' ') {
			TSMS_STRING_appendChar(temp, c);
		}
	}
	if (inQuote) {
		type = TSMS_JS_TYPE_UNDEFINED;
		TSMS_STRING_release(temp);
		return TSMS_UTIL_pair(&type, TSMS_NULL);
	}
	void * value;
	if (type == TSMS_JS_TYPE_UNDEFINED) {
		if (TSMS_STRING_equalsIgnoreCase(temp, TSMS_STRING_static("true"))) {
			type = TSMS_JS_TYPE_BOOL;
			value = TSMS_malloc(sizeof(bool));
			*((bool *) value) = true;
		} else if (TSMS_STRING_equalsIgnoreCase(temp, TSMS_STRING_static("false"))) {
			type = TSMS_JS_TYPE_BOOL;
			value = TSMS_malloc(sizeof(bool));
			*((bool *) value) = false;
		} else if (TSMS_STRING_equalsIgnoreCase(temp, TSMS_STRING_static("null"))) {
			type = TSMS_JS_TYPE_NULL;
			value = TSMS_NULL;
		} else if (TSMS_STRING_equalsIgnoreCase(temp, TSMS_STRING_static("undefined"))) {
			type = TSMS_JS_TYPE_UNDEFINED;
			value = TSMS_NULL;
		} else if (TSMS_STRING_isNumber(temp)) {
			type = TSMS_JS_TYPE_NUMBER;
			value = TSMS_malloc(sizeof(double));
			*((double *) value) = TSMS_STRING_toDouble(temp);
		} else if (context != TSMS_NULL) {
			// todo
			for (TSMS_POS i = 0; i < context->declarations->length; i ++) {
				pJSFunctionDeclaration declaration = context->declarations->list[i];

			}
			value = TSMS_NULL;
		} else
			value = TSMS_NULL;
	} else if (type == TSMS_JS_TYPE_STRING)
		value = TSMS_STRING_clone(temp);
	else if (type == TSMS_JS_TYPE_OBJECT)
		value = TSMS_NULL;
	TSMS_STRING_release(temp);
	return TSMS_UTIL_pair(&type, value);
}

bool TSMS_JS_compileToBool(pString js) {
	TSMS_PAIR pair = TSMS_JS_eval(js, TSMS_NULL);
	TSMS_JS_TYPE * typePointer = pair.key;
	TSMS_JS_TYPE type = *typePointer;
	if (type == TSMS_JS_TYPE_BOOL) {
		bool * boolPointer = pair.value;
		bool result = *boolPointer;
		free(pair.value);
		return result;
	} else if (type == TSMS_JS_TYPE_NUMBER) {
		double * numberPointer = pair.value;
		double result = *numberPointer;
		free(pair.value);
		return result != 0;
	} else if (type == TSMS_JS_TYPE_UNDEFINED  || type == TSMS_JS_TYPE_NULL)
		return false;
	free(pair.value);
	return true;
}