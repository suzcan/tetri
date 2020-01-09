#ifndef __MENUITEMSH__
#define __MENUITEMSH__

#include <GL/glut.h>

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <iostream>

class MenuItemBuilder {
	public:
		MenuItemBuilder();
		
		// arrow vertices to choose between items
		// vertices for title
		void draw_text(const char* text);
};

#endif
