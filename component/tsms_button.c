#include "tsms_button.h"
#include "tsms_list.h"

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_button_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t totalWidth, uint16_t totalHeight) {

}

TSMS_INLINE TSMS_RESULT __tsms_internal_button_render(pGuiElement element) {
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			if (child->render != TSMS_NULL)
				child->render(child);
		}
	return TSMS_SUCCESS;
}

TSMS_INLINE void __tsms_internal_button_callback(pMutable mutable, void * data, void * handler) {
	pButton button = (pButton) handler;
	button->requestRender = true;
}

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style, pMutable text, TSMS_BUTTON_CALLBACK callback,
                                    void *handler) {
	pButton button = (pButton) malloc(sizeof(tButton));
	if (button == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for button");
		TSMS_ERR_report(TSMS_ERR_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	button->preRender = __tsms_internal_button_pre_render;
	button->render = __tsms_internal_button_render;
	button->parent = TSMS_NULL;
	button->children = TSMS_NULL;
	button->style = style;
	button->lastStyle = style;
	button->requestRender = true;
	button->firstRender = true;
	button->grid = TSMS_GUI_INVALID_GRID;
	button->lastGrid = TSMS_GUI_INVALID_GRID;
	button->gui = TSMS_NULL;
	button->level = 0;


	TSMS_MUTABLE_setSetterCallback(text,__tsms_internal_button_callback, button);
	button->text = text;
	button->callback = callback;
	button->handler = handler;
	button->press = false;
	return button;
}

pButton TSMS_BUTTON_create(pMutable text, TSMS_BUTTON_CALLBACK callback, void * handler) {
	return TSMS_BUTTON_createWithStyle(TSMS_STYLE_DEFAULT_BUTTON, text, callback, handler);
}