#include <GL/glut.h> 

#include <iostream>
#include <list>
#include <iterator>
#include <tuple>

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> // for usleep
#include <math.h>
#include <random>
#include <string.h>

#define PI 3.14159265

// game objects
#include "playfield.h"
#include "blockbuilder.h"
#include "menuitem.h"

//#include "load_and_bind_texture.h"

using namespace std;

// playfield global vars
TetrisPlayfield playfield;

bool g_spinning = false;
int g_spin = 0;
bool update_locked = true;

// menu global vars
MenuItemBuilder menu;

// block global vars
BlockBuilder builder;

bool dropping = false;
float drop = 0.0f;
float speed_up = 1.0f;
float drop_rate = 0.02f * speed_up;
float move_h = 0.0f;
float move_rate = 0.07f;
float rotate = 0.0f;

list<int> next_blocks;
int curr_block = 0;

// random block generation variables
const int range_from = 0;
const int range_to = 6;
std::random_device rand_dev;
std::mt19937 generator(rand_dev());
std::uniform_int_distribution<int> distr(range_from, range_to);

int game_curr_state = 2;
enum game_state {GAME_MENU=0, GAME_INTRO=1, GAME_ACTIVE=2, 
				GAME_WIN=3, GAME_LOSE=4};

enum pieces_t {
	O_BLOCK=0, T_BLOCK=1, S_BLOCK=2,
	Z_BLOCK=3, I_BLOCK=4, L_BLOCK=5,
	J_BLOCK=6, TYPES=7
};

size_t g_pieces = 0;
unsigned int g_bitmap_text_handle = 0;

int get_new_block() {	
	return distr(generator);
}


float to_rad(float degree) {
	return (degree * PI/ 180.0);
}

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
    
    // draw the up next block
    glPushMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
			for (size_t s=0;s<5;s++)
			{
				glBegin(GL_LINE_LOOP);
				for (size_t v=0;v<4;v++)
					glVertex3fv(playfield.playfield_up_next[s][v]);
				glEnd();
			}
    glPopMatrix();
    
    // draw upcoming blocks
    float shift = 0.0f;
    list<int>::iterator itt;
    for(itt = next_blocks.begin(); itt != next_blocks.end(); ++itt)
    {
    	glPushMatrix();
    		glTranslatef(0.68f, 0.47f - shift, 0.0f);
    		glRotatef(90.0f, 0, 0, 1);
	        glCallList(g_pieces + *itt);
    	glPopMatrix();
    	shift = shift + 0.25;
	}
    
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

float curr_block_pos[4][2] = {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}};

void update_block_pos(float block_vertices[4][2])
{
	float x0 = move_h;
	float y0 = drop;
	curr_block_pos[0][0] = x0;
	curr_block_pos[0][1] = y0;
	for(size_t i = 1; i < 4; i++)
	{
		curr_block_pos[i][0] = curr_block_pos[i - 1][0] + block_vertices[i][0];
		curr_block_pos[i][1] = curr_block_pos[i - 1][1] + block_vertices[i][1];
	}
	if (rotate > 0)
	{
		// get new points location by calculating rotation transformation
		for(size_t i = 1; i < 4; i++)
		{	
			float sin_calc = sin(to_rad(rotate));
			float cos_calc = cos(to_rad(rotate));
			if(sin_calc < 1 && sin_calc > -1) sin_calc = 0.0f;
			if(cos_calc < 1 && cos_calc > -1) cos_calc = 0.0f;

			
			float x = curr_block_pos[i][0];
			float y = curr_block_pos[i][1];
			
			float x_calc = cos_calc * (x - x0) - sin_calc * (y-y0) + x0;
			float y_calc = sin_calc * (x-x0) + cos_calc * (y-y0) + y0;
			
			curr_block_pos[i][0] = x_calc;
			curr_block_pos[i][1] = y_calc;
		}
	}
}

void get_block_positions()
{
	switch(curr_block)
	{
		case(O_BLOCK):
			update_block_pos(builder.o_block_vertices);
			break;
		case(T_BLOCK): 
			update_block_pos(builder.t_block_vertices);
			break;
		case(S_BLOCK): 
			update_block_pos(builder.s_block_vertices);
			break;
		case(Z_BLOCK): 
			update_block_pos(builder.z_block_vertices);
			break;
		case(I_BLOCK): 
			update_block_pos(builder.i_block_vertices);
			break;
		case(L_BLOCK): 
			update_block_pos(builder.l_block_vertices);
			break;
		case(J_BLOCK): 
			update_block_pos(builder.j_block_vertices);
			break;
	}
}

/*
	Checks what rows need to be cleared and clears them 
	by updating the occupied array
*/
void clear_row_check()
{
	bool recheck = true;
	while(recheck)
	{
		recheck = false;
		for(size_t i = 0; i < 20; i++)
		{
			bool clear_row = true;
			for(size_t j = 0; j < 10 && clear_row; j++)
			{
				if(!playfield.occupied[i][j]) 
				{
					clear_row = false; 	
				}
			}
			if(clear_row) 
			{
				recheck = true;
				for(size_t k = i; k < 20; k++)
				{
					for(size_t z = 0; z < 10; z++)
					{
						playfield.occupied[k][z] = playfield.occupied[k+1][z];
					}
				}
			}
		}
	}
}


/*
	Checks if the current block will hit another block
	to be locked into the playfield
*/
void collision_check() 
{
	get_block_positions();
	bool collided = false;
	for(size_t i = 0; i < 4; i++) 
	{
		int index_h = playfield.get_index_h(curr_block_pos[i][0], builder.block_size);
		int index_v = playfield.get_index_v(curr_block_pos[i][1], builder.block_size);
		
		if(playfield.occupied[index_v - 1][index_h] || curr_block_pos[i][1] <= playfield.initial_stop) 
		{
			// if collision is true get coordinates of all blocks of the figure to lock in
			for(size_t j = 0; j < 4; j++)
			{
				index_h = playfield.get_index_h(curr_block_pos[j][0], builder.block_size);
				index_v = playfield.get_index_v(curr_block_pos[j][1], builder.block_size);
				
				playfield.occupied[index_v][index_h] = true;	
			}
			
			update_locked = true;
			collided = true;
			break;
		}
	}
	if(collided == true)
	{
		// reset the current block
		drop = 0.0f;
		move_h = 0.0f;
		rotate = 0.0f;
		// get next block and get a new random block
		curr_block = next_blocks.front();
		next_blocks.pop_front();
		next_blocks.push_back(get_new_block());
		// checks if the new block clears any rows
		clear_row_check();
	}
}

void create_menu()
{
	
}

void create_instructions()
{
	
}

void fireworks()
{
	
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

	switch(game_curr_state)
	{
		case(GAME_MENU):
			// TODO: 
			// ADD TETRIS TITLE 
			// SELECT BETWEEN STARTING GAME OR READING INSTRUCTIONS
			gluOrtho2D(0, 1000, 0, 1000);
			glColor3f(1.0f, 1.0f, 1.0f); 
			glPushMatrix();
				glTranslatef(100.0f, 5.0f, 1.0f);
				glScalef(0.9f, 0.9f, 1.0f); 
				menu.draw_text("Start Game");
			glPopMatrix();
			glPushMatrix();
				glTranslatef(105.0f, -100.0f, 1.0f);
				glScalef(0.9f, 0.9f, 1.0f);
				menu.draw_text("View Instructions");
			glPopMatrix();
			break;
		case(GAME_INTRO):
			// TODO:
			// ADD INSTRUCTIONS AND A BACK BOTTOM
			// WIN CONDITION 
			break;
		case(GAME_ACTIVE):
			//TODO: 
			// SET DROP TO AUTOMATIC
			// POSSIBLY ADD PAUSE
			// INCREASE SPEED EACH TIME A ROW IS CLEARED
			glPushMatrix();
				glRotatef(g_spin, 0, 1, 0);
				
				//draw tetris play area
				create_playfield();
				
				//block moving
				glTranslatef(0.0f + move_h, 1.1f + drop, 0.0f);
				glRotatef(rotate, 0, 0, 1);
				glCallList(g_pieces + curr_block);
			glPopMatrix();
			
			collision_check();
			break;
		case(GAME_WIN):
			// WIN HAVING CLEARED 25 ROWS
			// YOU WIN text
			// FIREWORK PARTICLES
			break;
		case(GAME_LOSE):
			// LOSE HAVING GONE OUT OF THE TOP
			// GAME OVER TEXT
			break;
	}

	glutSwapBuffers(); 
}

/*
	Checks if the next move attempted is within the bounds
	of the playfield.
*/
bool valid_move() 
{
	get_block_positions();
	for(size_t i = 0; i < 4; i++) 
	{
		int index_h = playfield.get_index_h(curr_block_pos[i][0], builder.block_size);
		int index_v = playfield.get_index_v(curr_block_pos[i][1], builder.block_size);
		if (playfield.occupied[index_v][index_h] || curr_block_pos[i][0] < -0.36 || curr_block_pos[i][0] > 0.29) return false;
	}
	return true;
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
				int curr = rotate;
				if(rotate == 270) {
					rotate = 0;
					if(!valid_move()) rotate = curr;
				} else {
					rotate = 90 + rotate;
					if(!valid_move()) rotate = curr;
				}
				break;
	}
	glutPostRedisplay();
}

void special(int key, int, int)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			move_h =  move_h + (move_rate * -1.0f);
			if (!valid_move()) move_h = move_h - (move_rate * -1.0f);
			break;
		case GLUT_KEY_RIGHT:
			move_h = move_h + move_rate;
			if (!valid_move()) move_h = move_h - move_rate; 
			break;
		case GLUT_KEY_UP: 
			break;
		case GLUT_KEY_DOWN:
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

void first_blocks()
{
	next_blocks.push_back(get_new_block());
	next_blocks.push_back(get_new_block());
	next_blocks.push_back(get_new_block());
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
	first_blocks();

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

