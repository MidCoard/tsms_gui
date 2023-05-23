#include "tsms_mutable_style.h"
#include "tsms.h"

pMutableStyle TSMS_MUTABLE_STYLE_create(TSMS_STYLE style) {
	pMutableStyle mutableStyle = (pMutableStyle) TSMS_malloc(sizeof(tMutableStyle));
	if (mutableStyle == TSMS_NULL)
		return TSMS_NULL;
	mutableStyle->style = style;
	mutableStyle->callback = TSMS_NULL;
	mutableStyle->handler = TSMS_NULL;
	return mutableStyle;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_set(pMutableStyle mutableStyle, TSMS_STYLE style) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (!TSMS_STYLE_equals(mutableStyle->style, style)) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style = style;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
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
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.font = font;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setDisplayType(pMutableStyle mutableStyle, TSMS_STYLE_DISPLAY display) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.display != display) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.display = display;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
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
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.position = position;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setLeft(pMutableStyle mutableStyle, uint16_t left) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.left != left) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.left = left;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setTop(pMutableStyle mutableStyle, uint16_t top) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.top != top) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.top = top;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle , mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setZIndex(pMutableStyle mutableStyle, uint16_t zIndex) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.zIndex != zIndex) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.zIndex = zIndex;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_MUTABLE_STYLE_setAbsolutePosition(pMutableStyle mutableStyle, uint16_t left, uint16_t top, uint16_t zIndex) {
	if (mutableStyle == TSMS_NULL)
		return TSMS_ERROR;
	if (mutableStyle->style.left != left ||
		mutableStyle->style.top != top ||
		mutableStyle->style.zIndex != zIndex) {
		TSMS_STYLE oldStyle = mutableStyle->style;
		mutableStyle->style.left = left;
		mutableStyle->style.top = top;
		mutableStyle->style.zIndex = zIndex;
		if (mutableStyle->callback != TSMS_NULL)
			mutableStyle->callback(mutableStyle, oldStyle, mutableStyle->style, mutableStyle->handler);
	}
	return TSMS_SUCCESS;
}