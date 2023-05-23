#ifndef TSMS_COMPONENT_CONTAINER_H
#define TSMS_COMPONENT_CONTAINER_H

#define TSMS_EXTEND_GUI_CONTAINER_ELEMENT \
	TSMS_EXTEND_GUI_ELEMENT \
	bool ignoreInvalidGrid;

#define TSMS_INIT_GUI_CONTAINER_ELEMENT(element, type0, preRender0, render0, style0, gui0, ignoreInvalidGrid0) \
	TSMS_INIT_GUI_ELEMENT(element, type0, preRender0, render0, style0, gui0) \
	element->ignoreInvalidGrid = ignoreInvalidGrid0;

typedef struct TSMS_CONTAINER tContainer;
typedef tContainer * pContainer;

#include "tsms_gui.h"

struct TSMS_CONTAINER {
	TSMS_EXTEND_GUI_CONTAINER_ELEMENT
};

TSMS_GRID_INFO TSMS_CONTAINER_preRender(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight);

TSMS_RESULT TSMS_CONTAINER_render(pGuiElement element, pLock lock);

pContainer TSMS_CONTAINER_create(bool ignoreInvalidGrid);

pContainer TSMS_CONTAINER_createWithStyle(TSMS_STYLE style, bool ignoreInvalidGrid);

TSMS_RESULT TSMS_CONTAINER_release(pContainer container);

#endif //TSMS_COMPONENT_CONTAINER_H
