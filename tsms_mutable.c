#include "tsms_mutable.h"

pMutable TSMS_MUTABLE_create(void * data, TSMS_FREE_FUNCTION freeFunction) {
	pMutable mutable = (pMutable)malloc(sizeof(tMutable));
	if (mutable == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for mutable");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	mutable->data = data;
	mutable->freeFunction = freeFunction;
	mutable->setterCallback = TSMS_NULL;
	mutable->setterHandler = TSMS_NULL;
	return mutable;
}

TSMS_RESULT TSMS_MUTABLE_set(pMutable mutable, void * data) {
	if (mutable == TSMS_NULL)
		return TSMS_ERROR;
	if (mutable->data != data) {
		if (mutable->setterCallback != TSMS_NULL)
			mutable->setterCallback(mutable, data, mutable->setterHandler);
		if (mutable->freeFunction != TSMS_NULL)
			mutable->freeFunction(mutable->data);
		else
			free(mutable->data);
		mutable->data = data;
	}
	return TSMS_SUCCESS;
}

void * TSMS_MUTABLE_get(pMutable mutable) {
	if (mutable == TSMS_NULL)
		return TSMS_NULL;
	return mutable->data;
}

TSMS_RESULT TSMS_MUTABLE_setSetterCallback(pMutable mutable, TSMS_MUTABLE_CALLBACK callback, void * handler) {
	if (mutable == TSMS_NULL)
		return TSMS_ERROR;
	mutable->setterCallback = callback;
	mutable->setterHandler = handler;
	return TSMS_SUCCESS;
}