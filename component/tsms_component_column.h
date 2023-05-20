#ifndef TSMS_COMPONENT_COLUMN_H
#define TSMS_COMPONENT_COLUMN_H

typedef struct TSMS_COLUMN tColumn;
typedef tColumn * pColumn;

#include "tsms_gui.h"
#include "tsms_component_container.h"

struct TSMS_COLUMN {
	TSMS_EXTEND_GUI_CONTAINER_ELEMENT

	TSMS_ILP list;
};

pColumn TSMS_COLUMN_create();

pColumn TSMS_COLUMN_createWithStyle(TSMS_STYLE style);

TSMS_RESULT TSMS_COLUMN_add(pColumn column, pGuiElement element);

TSMS_RESULT TSMS_COLUMN_addWithWeight(pColumn column, pGuiElement element, int weight);

#endif //TSMS_COMPONENT_COLUMN_H
