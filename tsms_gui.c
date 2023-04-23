#include "tsms_gui.h"
#include "tsms_display.h"
#include "tsms_list.h"

TSMS_INLINE TSMS_RESULT __tsms_internal_default_render(pGuiElement element, uint16_t x, uint16_t y) {

	if (element->children != TSMS_NULL)
		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			if (child->render != TSMS_NULL)
				child->render(child, );
		}

	return TSMS_SUCCESS;
}

TSMS_INLINE TSMS_RESULT __tsms_internal_request_render(pGuiElement element) {
	if (element->children != TSMS_NULL) {

	}
}

pGui TSMS_GUI_create(TSMS_DPHP display) {
	pGui gui = (pGui) malloc(sizeof(tGui));
	if (gui == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for gui");
		TSMS_ERR_report(TSMS_ERR_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	gui->display = display;
	gui->parent = TSMS_NULL;
	gui->children = TSMS_LIST_create(10);
	gui->render = __tsms_internal_default_render;
//	TSMS_TOUCH_setCallback(gui->display->touch, )
	return gui;
}

TSMS_RESULT TSMS_GUI_add(pGui gui, pGuiElement element) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	element->parent = gui;
	return TSMS_LIST_add(gui->children, element);
}

TSMS_RESULT TSMS_GUI_draw(pGui gui) {
	if (gui == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_RESULT result = TSMS_DISPLAY_request(gui->display);
	if (gui->request)
}