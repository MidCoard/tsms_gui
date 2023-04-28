#ifndef TSMS_NATIVE_MUTABLE_H
#define TSMS_NATIVE_MUTABLE_H

typedef struct TSMS_NATIVE_MUTABLE tNativeMutable;
typedef tNativeMutable * pNativeMutable;

typedef void(*TSMS_NATIVE_MUTABLE_CALLBACK)(pNativeMutable, void *, void * handler);

#include "tsms_def.h"
#include "tsms_function_def.h"

struct TSMS_NATIVE_MUTABLE {
	void * data;
	TSMS_SIZE size;
	void * buffer;
	TSMS_NATIVE_MUTABLE_CALLBACK callback;
	void *handler;
};

pNativeMutable TSMS_NATIVE_MUTABLE_create(void * data, TSMS_SIZE size);

TSMS_RESULT TSMS_NATIVE_MUTABLE_release(pNativeMutable nativeMutable);

TSMS_RESULT TSMS_NATIVE_MUTABLE_setCallback(pNativeMutable nativeMutable, TSMS_NATIVE_MUTABLE_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_NATIVE_MUTABLE_refresh();


#endif //TSMS_NATIVE_MUTABLE_H
