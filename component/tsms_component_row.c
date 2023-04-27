#include "tsms_component_row.h"
#include "tsms_component_container.h"

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_row_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	pRow row = element;
	TSMS_STYLE style = element->computedStyle;
	if (style.display == TSMS_STYLE_DISPLAY_NONE)
		return element->grid = TSMS_GUI_calcGrid(element, style, x, y, 0, 0, parentWidth, parentHeight);
	uint16_t elementWidth = TSMS_STYLE_elementWidth(style, parentWidth);
	uint16_t elementHeight = TSMS_STYLE_elementHeight(style, parentHeight);
	int relativeCount = 0;
	for (TSMS_POS i = 0; i < element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		if (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE)
			relativeCount += row->list->list[i];
	}
	uint16_t childWidth = elementWidth / (float) relativeCount;
	uint16_t maxHeight = 0;
	int previous = 0;
	for (TSMS_POS i = 0; i< element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		uint16_t offset = (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) ? childWidth * previous : 0;
		TSMS_GRID_INFO childGrid = child->preRender(child, x + offset + TSMS_STYLE_left(style), y - TSMS_STYLE_top(style), childWidth, elementHeight);
		if (TSMS_GUI_isInvalidGrid(childGrid) && child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE)
			return element->grid = TSMS_GUI_INVALID_GRID;
		if (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
			maxHeight = max(maxHeight, childGrid.height);
			previous += row->list->list[i];
		}
	}
	return element->grid = TSMS_GUI_calcGrid(element, style, x, y, elementWidth, maxHeight, parentWidth, parentHeight);
}

pRow TSMS_ROW_create() {
	return TSMS_ROW_createWithStyle(TSMS_STYLE_DEFAULT_ROW);
}

pRow TSMS_ROW_createWithStyle(TSMS_STYLE style) {
	pRow row = malloc(sizeof(tRow));
	if (row == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for tRow");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}

	row->type = TSMS_GUI_TYPE_ROW;
	row->preRender = __tsms_internal_row_pre_render;
	row->render = TSMS_CONTAINER_render;
	row->parent = TSMS_NULL;
	row->children = TSMS_LIST_create(10);
	row->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(row->style, TSMS_GUI_defaultStyleCallback, row);
	row->lastStyle = style;
	row->computedStyle = style;
	row->requestRender = true;
	row->forceRender = true;
	row->grid = TSMS_GUI_INVALID_GRID;
	row->lastGrid = TSMS_GUI_INVALID_GRID;
	row->gui = TSMS_NULL;
	row->level = 0;
	row->renderOperations = TSMS_LIST_create(10);

	row->list = TSMS_INT_LIST_create(10);
	return row;
}

TSMS_RESULT TSMS_ROW_add(pRow row, pGuiElement element) {
	return TSMS_ROW_addWithWeight(row, element, 1);
}

TSMS_RESULT TSMS_ROW_addWithWeight(pRow row, pGuiElement element, int weight) {
	if (row == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_GUI_add(row, element);
	TSMS_INT_LIST_add(row->list, weight);
	return TSMS_SUCCESS;
}