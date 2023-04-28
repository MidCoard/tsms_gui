#ifndef TSMS_NATIVE_MUTABLE_STRING_H
#define TSMS_NATIVE_MUTABLE_STRING_H

typedef struct TSMS_NATIVE_MUTABLE_STRING tNativeMutableString;
typedef tNativeMutableString * pNativeMutableString;

typedef void (*TSMS_NATIVE_MUTABLE_STRING_CALLBACK)(pNativeMutableString, void * handler);

#include "tsms_native_mutable.h"

struct TSMS_NATIVE_MUTABLE_STRING {
	pNativeMutable native;
	pNativeMutable str;
	TSMS_NATIVE_MUTABLE_STRING_CALLBACK callback;
	void *handler;
};

pNativeMutableString TSMS_NATIVE_MUTABLE_STRING_create(pString str);

TSMS_RESULT TSMS_NATIVE_MUTABLE_STRING_release(pNativeMutableString nativeMutableString);

TSMS_RESULT TSMS_NATIVE_MUTABLE_STRING_setCallback(pNativeMutableString nativeMutableString, TSMS_NATIVE_MUTABLE_STRING_CALLBACK callback, void * handler);

#endif //TSMS_NATIVE_MUTABLE_STRING_H
