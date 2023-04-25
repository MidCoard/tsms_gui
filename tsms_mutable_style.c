#include "tsms_mutable_style.h"

pMutableStyle TSMS_MUTABLE_STYLE_create(TSMS_STYLE style) {
	pMutableStyle mutableStyle = (pMutableStyle)malloc(sizeof(tMutableStyle));
	if (mutableStyle == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for mutable style");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	mutableStyle->style = style;
	mutableStyle->setterCallback = TSMS_NULL;
	mutableStyle->getterCallback = TSMS_NULL;
	mutableStyle->setterHandler = TSMS_NULL;
	mutableStyle->getterHandler = TSMS_NULL;
	return mutableStyle;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_set(pMutableStyle mutableStyle, TSMS_STYLE style) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (!TSMS_STYLE_equals(mutableStyle->style, style)) {
		if (mutableStyle->setterCallback != TSMS_NULL)
			mutableStyle->setterCallback(mutableStyle, style, mutableStyle->setterHandler);
		mutableStyle->style = style;
	}
	return TSMS_SUCCESS;
}

TSMS_STYLE TSMS_MUTABLE_STYLE_get(pMutableStyle mutableStyle) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_STYLE_DEFAULT;
	if (mutableStyle->getterCallback != TSMS_NULL)
		mutableStyle->getterCallback(mutableStyle, mutableStyle->getterHandler);
	return mutableStyle->style;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setSetterCallback(pMutableStyle mutableStyle, TSMS_MUTABLE_STYLE_SETTER_CALLBACK callback, void * handler) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	mutableStyle->setterCallback = callback;
	mutableStyle->setterHandler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setGetterCallback(pMutableStyle mutableStyle, TSMS_MUTABLE_STYLE_GETTER_CALLBACK callback, void * handler) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	mutableStyle->getterCallback = callback;
	mutableStyle->getterHandler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setFont(pMutableStyle mutableStyle, TSMS_STYLE_FONT font) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (!TSMS_STYLE_equalsFont(mutableStyle->style.font, font)) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.font = font;
		if (mutableStyle->setterCallback != TSMS_NULL)
			mutableStyle->setterCallback(mutableStyle, newStyle, mutableStyle->setterHandler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}