#ifndef __MENUITEMSH__
#define __MENUITEMSH__

#include <GL/glut.h>

#include <GL/glut.h> 

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <iostream>

class MenuItemBuilder {
	public:
		MenuItemBuilder();

		// title
		// arrow to choose between items
		void draw_arrow();

		// text drawer
		void draw_text(const char* text);
};

#endif
