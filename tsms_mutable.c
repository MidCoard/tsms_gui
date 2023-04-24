#include "tsms_mutable.h"

pMutable TSMS_MUTABLE_create(void * data) {
	pMutable mutable = (pMutable)malloc(sizeof(tMutable));
	if (mutable == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for mutable");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	mutable->data = data;
	mutable->setterCallback = TSMS_NULL;
	mutable->getterCallback = TSMS_NULL;
	return mutable;
}

TSMS_RESULT TSMS_MUTABLE_set(pMutable mutable, void * data) {
	if (mutable == TSMS_NULL)
		return TSMS_ERROR;
	if (mutable->data != data) {
		if (mutable->setterCallback != TSMS_NULL)
			mutable->setterCallback(mutable, data, mutable->setterHandler);
		mutable->data = data;
	}
	return TSMS_SUCCESS;
}

void * TSMS_MUTABLE_get(pMutable mutable) {
	if (mutable == TSMS_NULL)
		return TSMS_NULL;
	if (mutable->getterCallback != TSMS_NULL)
		mutable->getterCallback(mutable, mutable->getterHandler);
	return mutable->data;
}

TSMS_RESULT TSMS_MUTABLE_setSetterCallback(pMutable mutable, TSMS_MUTABLE_SETTER_CALLBACK callback, void * handler) {
	if (mutable == TSMS_NULL)
		return TSMS_ERROR;
	mutable->setterCallback = callback;
	mutable->setterHandler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_setGetterCallback(pMutable mutable, TSMS_MUTABLE_GETTER_CALLBACK callback, void * handler) {
	if (mutable == TSMS_NULL)
		return TSMS_ERROR;
	mutable->getterCallback = callback;
	mutable->getterHandler = handler;
	return TSMS_SUCCESS;
}
