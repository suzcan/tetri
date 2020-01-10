#include "menuitem.h"

MenuItemBuilder::MenuItemBuilder() {}

void MenuItemBuilder::draw_text(const char* text)
{
	size_t len = strlen(text);
	for (size_t i=0;i<len;i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
}

void MenuItemBuilder::draw_arrow()
{	
	glBegin(GL_QUADS); 
		glVertex2f(-0.8f, -0.59f);
		glVertex2f(-0.7f, -0.59f);
		glVertex2f(-0.7f, -0.54f);
		glVertex2f(-0.8f, -0.54f);
	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex2f(-0.7f, -0.64f);
		glVertex2f(-0.7f, -0.49f);
		glVertex2f(-0.60f, -0.575f);
	glEnd();
}