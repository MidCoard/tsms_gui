#include "tsms_component_container.h"

TSMS_INLINE bool __tsms_internal_isIgnoreInvalidGrid(pGuiElement element) {
	if (element->type == TSMS_GUI_TYPE_CONTAINER || element->type == TSMS_GUI_TYPE_GUI || element->type == TSMS_GUI_TYPE_COLUMN || element->type == TSMS_GUI_TYPE_ROW)
		return ((pContainer)element)->ignoreInvalidGrid;
	return false;
}

TSMS_GRID_INFO TSMS_CONTAINER_preRender(pGuiElement element, uint16_t x, uint16_t y, uint16_t parentWidth, uint16_t parentHeight) {
	TSMS_STYLE style = TSMS_STYLE_getStyle(element);
	TSMS_GUI_STYLE_RENDER
	uint16_t elementWidth = TSMS_STYLE_elementWidth(style, parentWidth);
	uint16_t elementHeight = TSMS_STYLE_elementHeight(style, parentHeight);
	TSMS_GRID_INFO grid = (TSMS_GRID_INFO) {x + TSMS_STYLE_leftOffset(style), y - TSMS_STYLE_topOffset(style), 0, 0, 0, TSMS_STYLE_DISPLAY_BLOCK};
	uint16_t maxWidth = 0;
	uint16_t currentRowWidth = 0;
	uint16_t maxHeight = 0;
	uint16_t currentColumnHeight = 0;
	for (TSMS_POS i = 0; i< element->children->length; i++) {
		pGuiElement child = element->children->list[i];
		TSMS_GRID_INFO previous = grid;
		grid = child->preRender(child, grid.x + grid.width, grid.y, elementWidth - currentRowWidth, elementHeight - currentColumnHeight);
		if (TSMS_GUI_isInvalidGrid(grid) && child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
			currentColumnHeight += maxHeight;
			maxHeight = 0;
			maxWidth = max(maxWidth, currentRowWidth);
			currentRowWidth = 0;
			grid = child->preRender(child, x, y - currentColumnHeight, elementWidth, elementHeight - currentColumnHeight);
			if (TSMS_GUI_isInvalidGrid(grid))
				if (__tsms_internal_isIgnoreInvalidGrid(element)) {
					grid = previous;
					continue;
				}
				else return element->grid = TSMS_GUI_INVALID_GRID;
			currentRowWidth += grid.width;
			maxHeight = max(maxHeight, grid.height);
		} else if (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE) {
			currentRowWidth += grid.width;
			maxHeight = max(maxHeight, grid.height);
		} else
			grid = previous;
	}
	if (currentRowWidth > 0)
		maxWidth = max(maxWidth, currentRowWidth);
	return element->grid = TSMS_GUI_calcGrid(element, style, x, y, style.width == TSMS_STYLE_AUTO ? maxWidth : elementWidth, style.height == TSMS_STYLE_AUTO ? maxHeight + currentColumnHeight : elementHeight, parentWidth, parentHeight );
}

TSMS_RESULT TSMS_CONTAINER_render(pGuiElement element, pLock lock) {
	TSMS_STYLE style = element->computedStyle;
	if (!TSMS_GUI_isInvalidGrid(element->grid) && element->grid.displayType != TSMS_STYLE_DISPLAY_NONE) {
		TSMS_GUI_renderStyle(element, style, lock);

		for (TSMS_POS i = 0; i < element->children->length; i++) {
			pGuiElement child = element->children->list[i];
			if (child->computedStyle.position == TSMS_STYLE_POSITION_RELATIVE)
				child->render(child, lock);
		}
	}
	element->requestRender = false;
	element->lastGrid = element->grid;
	return TSMS_SUCCESS;
}

pContainer TSMS_CONTAINER_create(bool ignoreInvalidGrid) {
	return TSMS_CONTAINER_createWithStyle(TSMS_STYLE_DEFAULT_CONTAINER, ignoreInvalidGrid);
}

pContainer TSMS_CONTAINER_createWithStyle(TSMS_STYLE style, bool ignoreInvalidGrid) {
	pContainer container = (pContainer) malloc(sizeof(tContainer));
	if (container == TSMS_NULL) {
		tString temp = TSMS_STRING_temp("malloc failed for guiContainer");
		TSMS_ERR_report(TSMS_ERROR_TYPE_MALLOC_FAILED, &temp);
		return TSMS_NULL;
	}
	container->type = TSMS_GUI_TYPE_CONTAINER;
	container->preRender = TSMS_CONTAINER_preRender;
	container->render = TSMS_CONTAINER_render;
	container->parent = TSMS_NULL;
	container->children = TSMS_LIST_create(10);
	container->style = TSMS_MUTABLE_STYLE_create(style);
	TSMS_MUTABLE_STYLE_setCallback(container->style, TSMS_GUI_defaultStyleCallback, container);
	container->computedStyle = style;
	container->requestRender = true;
	container->grid = TSMS_GUI_INVALID_GRID;
	container->lastGrid = TSMS_GUI_INVALID_GRID;
	container->gui = TSMS_NULL;
	container->level = 0;
	container->renderOperations = TSMS_LIST_create(10);


	container->ignoreInvalidGrid = ignoreInvalidGrid;
	return container;
}

TSMS_RESULT TSMS_CONTAINER_release(pContainer container) {
	if (container == TSMS_NULL)
		return TSMS_ERROR;
	return TSMS_GUI_releaseGuiElement(container);
}