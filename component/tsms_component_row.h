#ifndef TSMS_COMPONENT_ROW_H
#define TSMS_COMPONENT_ROW_H

typedef struct TSMS_ROW tRow;
typedef tRow * pRow;

#include "tsms_gui.h"
#include "tsms_int_list.h"
#include "tsms_component_container.h"

struct TSMS_ROW {
	TSMS_EXTEND_GUI_CONTAINER_ELEMENT

	TSMS_ILP list;
};

pRow TSMS_ROW_create();

pRow TSMS_ROW_createWithStyle(TSMS_STYLE style);

TSMS_RESULT TSMS_ROW_add(pRow row, pGuiElement element);

TSMS_RESULT TSMS_ROW_addWithWeight(pRow row, pGuiElement element, int weight);

TSMS_RESULT TSMS_ROW_release(pRow row);

#endif //TSMS_COMPONENT_ROW_H
