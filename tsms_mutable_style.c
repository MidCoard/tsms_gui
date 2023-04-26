#include "tsms_mutable_style.h"

pMutableStyle TSMS_MUTABLE_STYLE_create(TSMS_STYLE style) {
	pMutableStyle mutableStyle = (pMutableStyle)malloc(sizeof(tMutableStyle));
	if (mutableStyle == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for mutable style");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	mutableStyle->style = style;
	mutableStyle->callback = TSMS_NULL;
	mutableStyle->handler = TSMS_NULL;
	return mutableStyle;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_set(pMutableStyle mutableStyle, TSMS_STYLE style) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (!TSMS_STYLE_equals(mutableStyle->style, style)) {
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, style, mutableStyle->handler);
		mutableStyle->style = style;
	}
	return TSMS_SUCCESS;
}

TSMS_STYLE TSMS_MUTABLE_STYLE_get(pMutableStyle mutableStyle) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_STYLE_DEFAULT;
	return mutableStyle->style;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setCallback(pMutableStyle mutableStyle, TSMS_MUTABLE_STYLE_CALLBACK callback, void * handler) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	mutableStyle->callback = callback;
	mutableStyle->handler = handler;
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setFont(pMutableStyle mutableStyle, TSMS_STYLE_FONT font) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (!TSMS_STYLE_equalsFont(mutableStyle->style.font, font)) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.font = font;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setDisplayType(pMutableStyle mutableStyle, TSMS_STYLE_DISPLAY displayType) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.display != displayType) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.display = displayType;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}