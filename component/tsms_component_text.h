#ifndef TSMS_COMPONENT_TEXT_H
#define TSMS_COMPONENT_TEXT_H

typedef struct TSMS_TEXT tText;
typedef tText * pText;

#include "tsms_gui.h"
#include "tsms_native_mutable_string.h"

struct TSMS_TEXT {
	TSMS_EXTEND_GUI_ELEMENT

	pString text;
	TSMS_LP list;
};

pText TSMS_TEXT_createWithStyle(TSMS_STYLE style, pString text);

pText TSMS_TEXT_create(pString text);

#endif //TSMS_COMPONENT_TEXT_H
