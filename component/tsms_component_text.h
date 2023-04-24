#ifndef TSMS_COMPONENT_TEXT_H
#define TSMS_COMPONENT_TEXT_H

typedef struct TSMS_TEXT tText;
typedef tText * pText;

#include "tsms_gui.h"
#include "tsms_mutable.h"

struct TSMS_TEXT {
	TSMS_EXTEND_GUI_ELEMENT

	pMutable text;
};

pText TSMS_TEXT_createWithStyle(TSMS_STYLE style, pMutable text);

pText TSMS_TEXT_create(pMutable text);

#endif //TSMS_COMPONENT_TEXT_H
