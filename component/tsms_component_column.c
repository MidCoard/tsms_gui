#include "tsms_component_column.h"

TSMS_INLINE TSMS_GRID_INFO __tsms_internal_column_pre_render(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	pColumn column = element;
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	if (style.display == TSMS_STYLE_DISPLAY_NONE)
		return element->grid = TSMS_GUI_calcGrid(element, style, x, y, 0, 0, parentWidth, parentHeight);
	uint16_t elementWidth = TSMS_STYLE_elementWidth(style, parentWidth);
	uint16_t elementHeight = TSMS_STYLE_elementHeight(style, parentHeight);
	int relativeCount = 0;
	for (TSMS_POS i = 0; i < element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		TSMS_STYLE childStyle = TSMS_STYLE_getStyle(child);
		if (childStyle.position == TSMS_STYLE_POSITION_RELATIVE)
			relativeCount += column->list->list[i];
	}
	uint16_t childHeight = elementHeight / (float) relativeCount;
	uint16_t maxWidth = 0;
	int previous = 0;
	for (TSMS_POS i = 0; i< element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		uint16_t offset = (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) ? childHeight * previous : 0;
		TSMS_GRID_INFO childGrid = child->preRender(child, x + TSMS_STYLE_left(style), y - offset - TSMS_STYLE_top(style), elementWidth, childHeight);
		if (TSMS_GUI_isInvalidGrid(childGrid) && child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE)
			return element->grid = TSMS_GUI_INVALID_GRID;
		if (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
			maxWidth = max(maxWidth, childGrid.width);
			previous += column->list->list[i];
		}
	}
	return element->grid = TSMS_GUI_calcGrid(element, style, x, y, maxWidth, elementHeight, parentWidth, parentHeight);
}

pColumn TSMS_COLUMN_create() {
	return TSMS_COLUMN_createWithStyle(TSMS_STYLE_DEFAULT_COLUMN);
}

pColumn TSMS_COLUMN_createWithStyle(TSMS_STYLE style) {
	pColumn column = (pColumn)malloc(sizeof(tColumn));
	if (column == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for guiColumn");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	column->type = TSMS_GUI_TYPE_COLUMN;
	column->preRender = __tsms_internal_column_pre_render;
	column->render = TSMS_CONTAINER_render;
	column->parent = TSMS_NULL;
	column->children = TSMS_LIST_create(10);
	column->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(column->style, TSMS_GUI_defaultStyleCallback, column);
	column->lastStyle = style;
	column->computedStyle = style;
	column->requestRender = true;
	column->grid = TSMS_GUI_INVALID_GRID;
	column->lastGrid = TSMS_GUI_INVALID_GRID;
	column->gui = TSMS_NULL;
	column->level = 0;
	column->renderOperations = TSMS_LIST_create(10);
	column->ignoreInvalidGrid = true;

	column->list = TSMS_INT_LIST_create(10);
	return column;
}

TSMS_RESULT TSMS_COLUMN_add(pColumn column, pGuiElement element) {
	return TSMS_COLUMN_addWithWeight(column, element, 1);
}

TSMS_RESULT TSMS_COLUMN_addWithWeight(pColumn column, pGuiElement element, int weight) {
	if (column == TSMS_NULL)
		return TSMS_ERROR;
	if (element == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_GUI_add(column, element);
	TSMS_INT_LIST_add(column->list, weight);
	return TSMS_SUCCESS;
}

TSMS_RESULT TSMS_COLUMN_release(pColumn column) {
	if (column == TSMS_NULL)
		return TSMS_ERROR;
	TSMS_INT_LIST_release(column->list);
	return TSMS_CONTAINER_release(column);
}