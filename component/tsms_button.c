#include "tsms_button.h"
#include "tsms_list.h"

TSMS_INLINE TSMS_RESULT __tsms_internal_button_render(pGuiElement element) {
	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			if (child->render != TSMS_NULL)
				child->render(child);
		}
	return TSMS_SUCCESS;
}

pButton TSMS_BUTTON_createWithStyle(TSMS_STYLE style) {
	pButton button = (pButton) malloc(sizeof(tButton));
	if (button == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for button");
		TSMS_ERR_report(TSMS_ERR_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	button->parent = TSMS_NULL;
	button->children = TSMS_NULL;
	button->render = __tsms_internal_button_render;
	button->style = style;
	return button;
}

pButton TSMS_BUTTON_create(Mu) {
	return TSMS_BUTTON_createWithStyle(TSMS_STYLE_DEFAULT);
}