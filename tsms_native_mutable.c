#include "tsms_native_mutable.h"
#include "tsms.h"

static TSMS_LP _nativeMutableList = TSMS_NULL;

TSMS_RESULT TSMS_NATIVE_MUTABLE_init() {
	_nativeMutableList = TSMS_LIST_create(10);
	return TSMS_SUCCESS;
}

pNativeMutable TSMS_NATIVE_MUTABLE_create(void * data, TSMS_SIZE size) {
	pNativeMutable nativeMutable = (pNativeMutable) TSMS_malloc(sizeof(tNativeMutable));
	if (nativeMutable == TSMS_NULL)
		return TSMS_NULL;
	TSMS_LIST_add(_nativeMutableList, nativeMutable);
	nativeMutable->data = data;
	nativeMutable->size = size;
	nativeMutable->buffer = TSMS_malloc(size);
	if (nativeMutable->buffer == TSMS_NULL) {
		TSMS_NATIVE_MUTABLE_release(nativeMutable);
		return TSMS_NULL;
	}
	memcpy(nativeMutable->buffer, data, size);
	return nativeMutable;
}

TSMS_RESULT TSMS_NATIVE_MUTABLE_release(pNativeMutable nativeMutable) {
	if (nativeMutable == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_LIST_removeElement(_nativeMutableList, nativeMutable);
	free(nativeMutable->buffer);
	free(nativeMutable);
	return TSMS_SUCCESS;
}


TSMS_RESULT TSMS_NATIVE_MUTABLE_setCallback(pNativeMutable nativeMutable, TSMS_NATIVE_MUTABLE_CALLBACK callback, void * handler) {
	if (nativeMutable == TSMS_NULL)
		return TSMS_ERROR;
	nativeMutable->callback = callback;
	nativeMutable->handler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_NATIVE_MUTABLE_refresh() {
	for (TSMS_SIZE i = 0; i < _nativeMutableList->length; i++) {
		pNativeMutable nativeMutable = _nativeMutableList->list[i];
		if (memcmp(nativeMutable->data, nativeMutable->buffer, nativeMutable->size) != 0) {
			if (nativeMutable->callback != TSMS_NULL)
				nativeMutable->callback(nativeMutable, nativeMutable->buffer, nativeMutable->handler);
			memcpy(nativeMutable->buffer, nativeMutable->data, nativeMutable->size);
		}
	}
	return TSMS_SUCCESS;
}



