#ifndef TSMS_COMPONENT_INPUT_H
#define TSMS_COMPONENT_INPUT_H

#include "tsms_component_text.h"

typedef struct TSMS_INPUT tInput;
typedef tInput * pInput;

struct TSMS_INPUT {
	TSMS_EXTEND_GUI_ELEMENT

	pText text;
	pString value;
};



#endif //TSMS_COMPONENT_INPUT_H
