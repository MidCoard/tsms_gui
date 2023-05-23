#include "tsms_component_row.h"
#include "tsms_component_container.h"

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_row_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	pRow row = element;
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	TSMS_GUI_STYLE_RENDER(element)
	uint16_t elementWidth = TSMS_STYLE_elementWidth(style, parentWidth);
	uint16_t elementHeight = TSMS_STYLE_elementHeight(style, parentHeight);
	int relativeCount = 0;
	for (TSMS_POS i = 0; i < element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		TSMS_STYLE childStyle = TSMS_STYLE_getStyle(child);
		if (childStyle.position == TSMS_STYLE_POSITION_RELATIVE)
			relativeCount += row->list->list[i];
	}
	uint16_t childWidth = elementWidth / (float) relativeCount;
	uint16_t maxHeight = 0;
	int previous = 0;
	for (TSMS_POS i = 0; i< element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		uint16_t offset = (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) ? childWidth * previous : 0;
		TSMS_GRID_INFO childGrid = child->preRender(child, x + offset + TSMS_STYLE_leftOffset(style), y -
				TSMS_STYLE_topOffset(style), childWidth, elementHeight);
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
	pRow row = TSMS_malloc(sizeof(tRow));
	if (row == TSMS_NULL)
		return TSMS_NULL;
	TSMS_INIT_GUI_CONTAINER_ELEMENT(row, TSMS_GUI_TYPE_ROW, __tsms_internal_row_pre_render, TSMS_CONTAINER_render, style, TSMS_NULL, true);
	row->list = TSMS_INT_LIST_create(10);
	if (row->style == TSMS_NULL || row->children == TSMS_NULL || row->renderOperations == TSMS_NULL || row->list == TSMS_NULL) {
		TSMS_ROW_release(row);
		return TSMS_NULL;
	}
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

TSMS_RESULT TSMS_ROW_release(pRow row) {
	if (row == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_INT_LIST_release(row->list);
	return TSMS_CONTAINER_release(row);
}