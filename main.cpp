#include <GL/glut.h> 

#include <iostream>
#include <list>
#include <iterator>
#include <tuple>

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> // for usleep

// game objects
#include "playfield.h"
#include "blockbuilder.h"

//#include "load_and_bind_texture.h"

using namespace std;

// playfield global vars
TetrisPlayfield playfield;

bool g_spinning = false;
int g_spin = 0;
bool update_locked = true;

// block global vars
BlockBuilder builder;

bool dropping = false;
float drop = 0.0f;
float speed_up = 0.0f;
float drop_rate = 0.02f + speed_up;
float move_h = 0.0f;
float move_v = 0.0f;
float move_rate = 0.07f;
float rotate = 0.0f;

int block_choice = 0;
//std::cout << "Added transform x: " << get<0>(*it) << " y: " << get<1>(*it) << std::endl;

enum game_state {GAME_MENU=0, GAME_INTRO=1, GAME_ACTIVE=2, 
				GAME_WIN=3, GAME_LOSE=4};

enum pieces_t {
	O_BLOCK=0, T_BLOCK=1, S_BLOCK=2,
	Z_BLOCK=3, I_BLOCK=4, L_BLOCK=5,
	J_BLOCK=6, TYPES=7
};

size_t g_pieces = 0;

void make_pieces()
{
	g_pieces = glGenLists(TYPES);
	
	glNewList(g_pieces + O_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.o_block_vertices);
	glEndList();
	
	glNewList(g_pieces + T_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.t_block_vertices);
	glEndList();
	
	glNewList(g_pieces + S_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.s_block_vertices);
	glEndList();
	
	glNewList(g_pieces + Z_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.z_block_vertices);
	glEndList();
	
	glNewList(g_pieces + I_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.i_block_vertices);
	glEndList();
	
	glNewList(g_pieces + L_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.l_block_vertices);
	glEndList();
	
	glNewList(g_pieces + J_BLOCK, GL_COMPILE);
		builder.make_large_block(builder.j_block_vertices);
	glEndList();
}

void idle()
{
    usleep(50000); // in microseconds
    if (g_spinning) {
    	g_spin += 1;	
	}
	if (dropping) {
		if(drop > -1.8f) drop = drop - drop_rate;		
	}
	glutPostRedisplay();
}

void create_playfield()
{	
	// draw floor
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			for (size_t i=0;i<4;i++)
			{
				glVertex3fv(playfield.playfield_floor[i]);
			}
		glEnd();
	glPopMatrix();
	
	// draw playfield wire frame
	glPushAttrib(GL_CURRENT_BIT);
			glColor3f(1.0f, 1.0f, 1.0f);
			for (size_t s=0;s<5;s++)
			{
				glBegin(GL_LINE_LOOP);
				for (size_t v=0;v<4;v++)
					glVertex3fv(playfield.playfield_vertices[s][v]);
				glEnd();
			}
    glPopAttrib();
    
    // add locked in blocks
    if(update_locked) {
    	playfield.update_transforms();
    	update_locked = false;
	}

	list<tuple<float, float>>::iterator it;
	for(it = playfield.transforms.begin(); it != playfield.transforms.end(); ++it)
	{
		glPushMatrix();
			glTranslatef(get<0>(*it), get<1>(*it), 0.0f);
			builder.make_block(1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, 0.0f);
		glPopMatrix();
	}
}

void collision_check() 
{
	int index_h = abs(int((move_h/0.07f) + 5));
	float collision_height = playfield.drop_stop[index_h];
	if(drop <= collision_height)
	{
		// update playfield state
		int index_v = abs(int(((collision_height + 0.4f)/0.07) + 20));
		playfield.occupied[index_v][index_h] = true;
		playfield.drop_stop[index_h] = collision_height + 0.07;
		update_locked = true;
		
		// reset block position
		drop = 0.0f;
		move_h = 0.0f;
		//block_choice = std::rand() % (7 +1);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	// position and orient camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0.25, 2.5, // eye position
			  0, 0, 0.0, // reference point
			  0, 1, 0  // up vector
	);

	glPushMatrix();
		glRotatef(g_spin, 0, 1, 0);

		// draw tetris play area
		create_playfield();
		
		glTranslatef(0.0f + move_h, 1.1f + drop, 0.0f);
		//std::cout << "move: " << move << " drop: " << drop << std::endl;
		// drop touches floor at dropped -1.8
		// move -0.35 - 0.28
		glRotatef(rotate, 0, 0, 1);
		glCallList(g_pieces + block_choice);

	glPopMatrix();
	
	collision_check();

	glutSwapBuffers(); 
}

void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': exit(1); break;

		case ' ': 
				g_spinning = !g_spinning;
				break;
		case 'd':
				dropping = !dropping;
				break;
		case 'r':
				if(rotate == 270) {
					rotate = 0;
				} else {
					rotate = 90 + rotate;
				}
	}
	glutPostRedisplay();
}

void special(int key, int, int)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			if (move_h > -0.34) move_h =  move_h + (move_rate * -1.0f); 
			break;
		case GLUT_KEY_RIGHT:
			if (move_h < 0.20) move_h = move_h + move_rate; 
			break;
		case GLUT_KEY_UP: 
			move_v = move_v + (move_rate * -1.0f);
			break;
		case GLUT_KEY_DOWN:
			speed_up = 0.05f; 
			break;
	}
	glutPostRedisplay();
}

void special_up(int key, int, int)
{
	switch(key)
	{
		case GLUT_KEY_DOWN:
			speed_up = 0.0f;
			break;
	}
	glutPostRedisplay();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(40.0, 1.0f, 1.0, 5.0);

	glutPostRedisplay();
}

void init()
{

	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {0.5, 0.5, 0.5, 1.0};

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH); 

	// turn on blending and set a blending function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	make_pieces();

	GLenum error = glGetError();
	if (error!=GL_NO_ERROR)
		fprintf(stderr, "GL error %s\n", gluErrorString(error));
	
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH); 
	glutInitWindowSize(640, 640); 
	glutInitWindowPosition(50, 50); 
	glutCreateWindow("Tetris"); 

	init(); 

	glutKeyboardFunc(keyboard); 
	glutSpecialFunc(special);
	glutSpecialUpFunc(special_up);
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	
	glutIdleFunc(idle);

	glutMainLoop(); 

	return 0; 
}

