#include "tsms_gui_parser.h"
#include "tsms_html_parser.h"
#include "tsms_css_parser.h"
#include "tsms_js_parser.h"
#include "malloc.h"

TSMS_INLINE void __tsms_internal_init_style(pHtmlNode node, TSMS_LP cssNodes) {
	pString style = TSMS_MAP_get(node->attributes, TSMS_STRING_static("style"));
	if (style != TSMS_NULL) {
		TSMS_LP cssList = TSMS_CSS_parse(style);
		if (cssList->length == 1) {
			pCssNode cssNode = cssList->list[0];
			TSMS_LIST_add(cssNodes, cssNode);
			TSMS_MI it = TSMS_MAP_iterator(cssNode->attributes);
			while (TSMS_MAP_hasNext(&it)) {
				TSMS_ME entry = TSMS_MAP_next(&it);
				TSMS_MAP_put(node->style, entry.key, entry.value);
			}
		}
		TSMS_LIST_release(cssList);
	}
	for (TSMS_POS i = 0; i < node->children->length; i++)
		__tsms_internal_init_style(node->children->list[i], cssNodes);
}

TSMS_INLINE void __tsms_internal_generate_extern(pHtmlNode node, pString result) {
	if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("html"))) {
		TSMS_STRING_appendString(result, "extern pGui ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "gui");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("div"))) {
		TSMS_STRING_appendString(result, "extern pContainer ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "container");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("label"))) {
		TSMS_STRING_appendString(result, "extern pLabel ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "label");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("button"))) {
		TSMS_STRING_appendString(result, "extern pButton ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "button");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("row"))) {
		TSMS_STRING_appendString(result, "extern pRow ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "row");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("column"))) {
		TSMS_STRING_appendString(result, "extern pColumn ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "column");
		TSMS_STRING_appendString(result, ";\n");
	}

	for (TSMS_POS i = 0; i < node->children->length; i++)
		__tsms_internal_generate_extern(node->children->list[i], result);
}

TSMS_INLINE void __tsms_internal_generate(pHtmlNode node, pString result) {
	if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("html"))) {
		TSMS_STRING_appendString(result, "pGui ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "gui");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("div"))) {
		TSMS_STRING_appendString(result, "pContainer ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "container");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("label"))) {
		TSMS_STRING_appendString(result, "pLabel ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "label");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("button"))) {
		TSMS_STRING_appendString(result, "pButton ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "button");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("row"))) {
		TSMS_STRING_appendString(result, "pRow ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "row");
		TSMS_STRING_appendString(result, ";\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("column"))) {
		TSMS_STRING_appendString(result, "pColumn ");
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "column");
		TSMS_STRING_appendString(result, ";\n");
	}
	for (TSMS_POS i = 0; i < node->children->length; i++)
		__tsms_internal_generate(node->children->list[i], result);
}

TSMS_INLINE void __tsms_internal_generate_style(TSMS_MP map, pString name, pString result) {
	TSMS_MI it = TSMS_MAP_iterator(map);
	while (TSMS_MAP_hasNext(&it)) {
		TSMS_ME entry = TSMS_MAP_next(&it);
		if (TSMS_STRING_equals(entry.key, TSMS_STRING_static("background-color"))) {
			TSMS_STRING_append(result, name);
			TSMS_STRING_appendString(result, ".backgroundColor = ");
			TSMS_PAIR pair = TSMS_JS_eval(entry.value, );
			TSMS_JS_TYPE * typePtr = pair.key;
			TSMS_JS_TYPE type = *typePtr;
			if (type == TSMS_JS_TYPE_FUNCTION) {
				if (TSMS_STRING_equals(""))
			} else free(pair.value);
		}
	}
}

TSMS_INLINE void __tsms_internal_generate_impl(pHtmlNode node, pString result) {
	if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("html"))) {
		pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
		if (id != TSMS_NULL)
			TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
		else
			TSMS_STRING_appendString(result, "gui");
		TSMS_STRING_appendString(result, " = TSMS_GUI_create();\n");
	} else if (TSMS_STRING_equals(node->tag, TSMS_STRING_static("div"))) {
		pString ignoreInvalidGrid = TSMS_MAP_get(node->attributes, TSMS_STRING_static("ignore-invalid-grid"));
		bool ignoreInvalidGridBool = false;
		if (ignoreInvalidGrid != TSMS_NULL)
			ignoreInvalidGridBool = TSMS_JS_compileToBool(ignoreInvalidGrid);
		if (node->style->size == 0) {
			pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
			if (id != TSMS_NULL)
				TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
			else
				TSMS_STRING_appendString(result, "container");
			TSMS_STRING_appendString(result, " = TSMS_CONTAINER_create(");
			TSMS_STRING_appendBool(result, ignoreInvalidGridBool);
			TSMS_STRING_appendString(result, ");\n");
		} else {
			pString id = TSMS_MAP_get(node->attributes, TSMS_STRING_static("id"));
			TSMS_STRING_appendString(result, "TSMS_STYLE ");
			if (id != TSMS_NULL)
				TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
			else
				TSMS_STRING_appendString(result, "container");
			TSMS_STRING_appendString(result, "Style = TSMS_STYLE_DEFAULT_CONTAINER;\n");
			pString styleName = TSMS_STRING_create();
			if (id != TSMS_NULL)
				TSMS_STRING_append(styleName, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
			else
				TSMS_STRING_appendString(styleName, "container");
			TSMS_STRING_appendString(styleName, "Style");
			__tsms_internal_generate_style(node->style, styleName, result);
			if (id != TSMS_NULL)
				TSMS_STRING_append(result, TSMS_MAP_get(node->attributes, TSMS_STRING_static("id")));
			else
				TSMS_STRING_appendString(result, "container");
			TSMS_STRING_appendString(result, " = TSMS_CONTAINER_createWithStyle(");
			TSMS_STRING_append(result, styleName);
			TSMS_STRING_release(styleName);
			TSMS_STRING_appendString(result, ", ");
			TSMS_STRING_appendBool(result, ignoreInvalidGridBool);
			TSMS_STRING_appendString(result, ");\n");
		}
	}
}

TSMS_INLINE pString __tsms_internal_generate_c_header(pHtmlNode node) {
	pString result = TSMS_STRING_create();
	TSMS_STRING_appendString(result, "#ifndef TSMS_GUI_USER_H\n");
	TSMS_STRING_appendString(result, "#define TSMS_GUI_USER_H\n");
	TSMS_STRING_appendString(result, "#include \"tsms_gui.h\"\n");
	__tsms_internal_generate_extern(node, result);
	TSMS_STRING_appendString(result, "void TSMS_GUI_initUser();\n");
	TSMS_STRING_appendString(result, "#endif\n");
	return result;
}

TSMS_INLINE pString __tsms_internal_generate_c_source(pHtmlNode node) {
	pString result = TSMS_STRING_create();
	TSMS_STRING_appendString(result, "#include \"tsms_gui_user.h\"\n");
	__tsms_internal_generate(node, result);
	TSMS_STRING_appendString(result, "void TSMS_GUI_initUser() {\n");
	TSMS_STRING_appendString(result, "TSMS_GUI_init();\n");
	__tsms_internal_generate_impl(node, result);
	TSMS_STRING_appendString(result, "}\n");
	return result;
}

TSMS_PAIR TSMS_GUI_parse(pString html, pString css) {
	pHtml htmlObject = TSMS_HTML_parse(html);
	TSMS_LP cssNodes = TSMS_CSS_parse(css);
	for (TSMS_POS i = 0; i < cssNodes->length; i++) {
		pCssNode cssNode = cssNodes->list[i];
		TSMS_LP elements = TSMS_HTML_getElementsBySelector(htmlObject, cssNode->selector);
		TSMS_MI it = TSMS_MAP_iterator(cssNode->attributes);
		while (TSMS_MAP_hasNext(&it)) {
			TSMS_ME entry = TSMS_MAP_next(&it);
			for (TSMS_POS j = 0; j < elements->length; j++) {
				pHtmlNode element = elements->list[j];
				TSMS_MAP_put(element->style, entry.key, entry.value);
			}
		}
		TSMS_LIST_release(elements);
	}
	__tsms_internal_init_style(htmlObject->root, cssNodes);
	pString cHeader = __tsms_internal_generate_c_header(htmlObject->root);
	pString cSource = __tsms_internal_generate_c_source(htmlObject->root);
	for (TSMS_POS i = 0; i < cssNodes->length; i++)
		TSMS_CSS_release(cssNodes->list[i]);
	TSMS_LIST_release(cssNodes);
	TSMS_HTML_release(htmlObject);
	return TSMS_UTIL_pair(cHeader, cSource);
}