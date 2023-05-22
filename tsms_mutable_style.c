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

TSMS_RESULT TSMS_MUTABLE_STYLE_release(pMutableStyle mutableStyle) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	free(mutableStyle);
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setPosition(pMutableStyle mutableStyle, TSMS_STYLE_POSITION position) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.position != position) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.position = position;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setLeft(pMutableStyle mutableStyle, uint16_t left) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.left != left) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.left = left;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setTop(pMutableStyle mutableStyle, uint16_t top) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.top != top) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.top = top;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setZIndex(pMutableStyle mutableStyle, uint16_t zIndex) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.zIndex != zIndex) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.zIndex = zIndex;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setAbsolutePosition(pMutableStyle mutableStyle, uint16_t left, uint16_t top, uint16_t zIndex) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.left != left ||
		mutableStyle->style.top != top ||
		mutableStyle->style.zIndex != zIndex) {
		TSMS_STYLE newStyle = mutableStyle->style;
		newStyle.left = left;
		newStyle.top = top;
		newStyle.zIndex = zIndex;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, newStyle, mutableStyle->handler);
		mutableStyle->style = newStyle;
	}
	return TSMS_SUCCESS;
}