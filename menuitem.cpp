#include "menuitem.h"

MenuItemBuilder::MenuItemBuilder() {}

void MenuItemBuilder::draw_text(const char* text)
{
	size_t len = strlen(text);
	for (size_t i=0;i<len;i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
}
