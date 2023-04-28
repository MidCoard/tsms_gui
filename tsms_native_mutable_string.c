#include "tsms_native_mutable_string.h"

TSMS_INLINE void __tsms_internal_native_callback(pNativeMutable nativeMutable, void * buffer, void * handler) {
	pNativeMutableString nativeMutableString = (pNativeMutableString)handler;
	pString str = nativeMutable->data;
	nativeMutableString->str->size = str->length + 1;
	void * tempBuffer = realloc(nativeMutableString->str->buffer, nativeMutableString->str->size);
	if (tempBuffer == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("realloc failed for nativeMutableString buffer");
		TSMS_ERR_report(TSMS_ERROR_TYPE_REALLOC_FAILED, &temp);
		return;
	}
	memcpy(tempBuffer, str->cStr, nativeMutableString->str->size);
	nativeMutableString->str->buffer = tempBuffer;
	if (nativeMutableString->callback != TSMS_NULL)
		nativeMutableString->callback(nativeMutableString, nativeMutableString->handler);
}

TSMS_INLINE void __tsms_internal_str_callback(pNativeMutable nativeMutable, void * buffer, void * handler) {
	pNativeMutableString nativeMutableString = (pNativeMutableString)handler;
	if (nativeMutableString->callback != TSMS_NULL)
		nativeMutableString->callback(nativeMutableString, nativeMutableString->handler);
}

pNativeMutableString TSMS_NATIVE_MUTABLE_STRING_create(pString str) {
	pNativeMutableString nativeMutableString = malloc(sizeof(tNativeMutableString));
	if (nativeMutableString == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for nativeMutableString");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	nativeMutableString->native = TSMS_NATIVE_MUTABLE_create(str, sizeof (tString));
	TSMS_NATIVE_MUTABLE_setCallback(nativeMutableString->native, __tsms_internal_native_callback, nativeMutableString);
	nativeMutableString->str = TSMS_NATIVE_MUTABLE_create(str->cStr, str->length + 1);
	TSMS_NATIVE_MUTABLE_setCallback(nativeMutableString->str, __tsms_internal_str_callback, nativeMutableString);

	nativeMutableString->callback = TSMS_NULL;
	nativeMutableString->handler = TSMS_NULL;
	return nativeMutableString;
}

TSMS_RESULT TSMS_NATIVE_MUTABLE_STRING_release(pNativeMutableString nativeMutableString) {
	if (nativeMutableString == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_NATIVE_MUTABLE_release(nativeMutableString->native);
	TSMS_NATIVE_MUTABLE_release(nativeMutableString->str);
	free(nativeMutableString);
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_NATIVE_MUTABLE_STRING_setCallback(pNativeMutableString nativeMutableString, TSMS_NATIVE_MUTABLE_STRING_CALLBACK callback, void * handler) {
	if (nativeMutableString == TSMS_NULL)
		return TSMS_ERROR;
	nativeMutableString->callback = callback;
	nativeMutableString->handler = handler;
	return TSMS_SUCCESS;
}

