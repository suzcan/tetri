#include <GL/glut.h> 

#include <iostream>
#include <list>
#include <iterator>
#include <tuple>

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> // for usleep
#include <math.h>
#include <cmath>
#include <vector>
#include <random>
#include <string.h>

#define PI 3.14159265

// game objects
#include "playfield.cpp"
#include "blockbuilder.cpp"
#include "menuitem.cpp"
#include "particle_generator.cpp"

#include "load_and_bind_texture.h"

using namespace std;

// game state vars
int game_curr_state = 4;
enum game_state {GAME_MENU=0, GAME_INTRO=1, GAME_ACTIVE=2, 
				GAME_WIN=3, GAME_LOSE=4};

// playfield global vars
TetrisPlayfield playfield;

bool g_spinning = false;
int g_spin = 0;
bool update_locked = true;
bool draw_pause = false;
int lines_cleared = 0;

// menu global vars
MenuItemBuilder menu;
int selection = 2;
bool up = true;

// block global vars
BlockBuilder builder;

bool dropping = false;
float drop = 0.0f;
float drop_rate = 0.05f;
float move_h = 0.0f;
float move_rate = 0.07f;
float rotate = 0.0f;

list<int> next_blocks;
int curr_block = 0;

enum pieces_t {
	O_BLOCK=0, T_BLOCK=1, S_BLOCK=2,
	Z_BLOCK=3, I_BLOCK=4, L_BLOCK=5,
	J_BLOCK=6, TYPES=7
};

size_t g_pieces = 0;
unsigned int g_block_tex = 0;
unsigned int texture = 0;

// particle variables
ParticleGenerator p_generator;
std::vector<ParticleGenerator::particle_t> g_particles;

/*
 variable to store the position of the 4 blocks that make up a single game piece
 each pair represents an x, y corner of the block
*/
float curr_block_pos[4][2] = {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}};


int get_new_block() {	
	return rand() % 7;
}

float to_rad(float degree) {
	return (degree * PI/ 180.0);
}

/*
	loads a texture for the blocks in the game depending on
	the value of texture
*/
void load_and_bind_textures()
{
	// 340x340 px image for block  
	switch(texture)
	{
		case 0: 
			g_block_tex = load_and_bind_texture("./textures/green_block.png");
			break;
		case 1:
			g_block_tex = load_and_bind_texture("./textures/block.png");
			break;
		case 2:
			g_block_tex = load_and_bind_texture("./textures/tex3.png");
			break;
		case 3:
			g_block_tex = load_and_bind_texture("./textures/thwomp.png");
			break;
	}
}

/*
	Creates a list with different types of tetris pieces
*/
void make_pieces()
{
	g_pieces = glGenLists(TYPES);
	
	glNewList(g_pieces + O_BLOCK, GL_COMPILE);
		builder.make_o_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + T_BLOCK, GL_COMPILE);
		builder.make_t_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + S_BLOCK, GL_COMPILE);
		builder.make_s_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + Z_BLOCK, GL_COMPILE);
		builder.make_z_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + I_BLOCK, GL_COMPILE);
		builder.make_i_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + L_BLOCK, GL_COMPILE);
		builder.make_l_block(g_block_tex);
	glEndList();
	
	glNewList(g_pieces + J_BLOCK, GL_COMPILE);
		builder.make_j_block(g_block_tex);
	glEndList();
}

/*
	Draws the active playfield
*/
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
			builder.make_block(g_block_tex, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, 0.0f);
		glPopMatrix();
	}
}

/*
	Uses the drop and horizonal movement of the player to find the logical position of the other
	pieces attached to the center block forming the piece
*/
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

/*
	Calls update_block_pos with the vertices transformations
	of the current block
*/
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
	Checks if the blocks have been placed outside the play area
	and changes game state to fail
*/
void check_game_over()
{
	for(int i = 0; i < 10; i++) {
		if(playfield.occupied[21][i]) {
			game_curr_state = GAME_LOSE;
			g_spinning = true;
		}
	}
}

/*
	Checks if more than win_condition blocks have been cleared
*/
void check_game_win()
{
	if(lines_cleared >= 3) {
		game_curr_state = GAME_WIN;
		g_spinning = true;
	}
}


/*
	Reset dynamic game variables
*/
void reset_game()
{
	drop = 0;
	dropping = true;
	move_h = 0;
	g_spin = 0;
	g_spinning = false;
	lines_cleared = 0;
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
		lines_cleared = lines_cleared + playfield.clear_row_check();
		// check for game over
		check_game_over();
		// check for game won
		check_game_win();
	}
}

/*
	Creates all items in the menu screen
*/
void create_menu()
{
	// TETRIS TITLE 
	glPushMatrix();
		glRotatef(g_spin, 0, 1, 0);
		glTranslatef(-0.70f, 0.3f, 0.0f);
		builder.make_title(g_block_tex);
	glPopMatrix();

	// SELECT BETWEEN STARTING GAME OR READING INSTRUCTIONS
	glPushMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		if(!up) glTranslatef(0.0f, -0.15f, 0.0f);
		menu.draw_arrow(); 
	glPopMatrix();

	// draw menu option texts
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
}

void create_instructions()
{

}

void flame(float x)
{
	
	p_generator.make_particles(12);
	static const float eye[3] = {0, 0.25, 2.5};
	static const float z_axis[3] = {0, 0, 1};

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
	
	// position and orient camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
			  eye[0], eye[1], eye[2],
			  0, 0, 0, // reference point
			  0, 1, 0  // up vector
		);
	glEnable(GL_TEXTURE_2D);
	for(float i = 0.25; i > -0.25; i = i - 0.01)
	{
		for (size_t i=0;i<g_particles.size();i++)
		{
			glColor4fv(g_particles[i].colour);
			glColor4f(
				g_particles[i].colour[0],
				g_particles[i].colour[1],
				g_particles[i].colour[2],
				g_particles[i].colour[3]
			);
			glPushMatrix();
				glTranslatef(
						g_particles[i].position[0] - 0.2,
						g_particles[i].position[1] - 0.9f,
						g_particles[i].position[2]);
				

				float size = g_particles[i].size;
			
				glBegin (GL_LINE_LOOP);
					glVertex3f(-size, -size, 0);
					glVertex3f(size, -size, 0);
					glVertex3f(size, size, 0);
					glVertex3f(-size, size, 0);
				glEnd();


				// post the texture
				glBegin (GL_QUADS);
					glTexCoord2f(0, 0); glVertex3f(-size, -size, 0.0f);
					glTexCoord2f(1, 0); glVertex3f(size, -size, 0.0f);
					glTexCoord2f(1, 1); glVertex3f(size, size, 0.0f);
					glTexCoord2f(0, 1); glVertex3f(-size, size, 0.0f);
				glEnd ();
				
			glPopMatrix();
		}
	}
    glDisable(GL_TEXTURE_2D);
}


void fireworks()
{
	
}

/*
	Screen that appears after win or lose condition
*/
void game_end_screen(const char* msg)
{
	// draw mini play area
	glPushMatrix();
		glRotatef(g_spin, 0, 1, 0);
		glTranslatef(0.0f, -0.4f, 0.0f);
		glScalef(0.5f, 0.5f, 1.0f);
		create_playfield();
	glPopMatrix();

	// draw end screen intructions texts
	glPushMatrix();
		gluOrtho2D(0, 1000, 0, 1000);
		glTranslatef(80.0f, 800.0f, 1.0f);
		menu.draw_text(msg);
		glPushMatrix();
			glTranslatef(-650.0f, -200.0f, 1.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			menu.draw_text("press 'r' to retry");			
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-850.0f, -300.0f, 1.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			menu.draw_text("press 'b' to go back to start");
		glPopMatrix();		
	glPopMatrix();
}

void display()
{
	load_and_bind_textures();
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
			// STARTING MENU SCREEN
			create_menu();
			break;
		case(GAME_INTRO):
			// TODO:
			// KEY BINDINGS (WASD || Arrow Keys)
			// WIN CONDITION 
			create_instructions();
			break;
		case(GAME_ACTIVE):
			// GAME PLAY SCREEN
			glPushMatrix();
				glRotatef(g_spin, 0, 1, 0);

				if(draw_pause) {
					gluOrtho2D(0, 1000, 0, 1000);
					glTranslatef(250.0f, 250.0f, 0.0f);
					menu.draw_text("PAUSED");
				}
				
				// draw tetris play area
				create_playfield();
				
				// draw player block
				glTranslatef(0.0f + move_h, 1.1f + drop, 0.0f);
				glRotatef(rotate, 0, 0, 1);
				glCallList(g_pieces + curr_block);
				
			glPopMatrix();

			collision_check();
			
			break;
		case(GAME_WIN):
			// GAME WON screen
			// FIREWORK PARTICLES
			game_end_screen("GAME WON!");
			break;
		case(GAME_LOSE):
			// GAME OVER screen
			game_end_screen("GAME OVER");
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
	if(game_curr_state == GAME_ACTIVE)
	{
		switch (key)
		{
			case 'q': exit(1); break;
			case ' ': 
					g_spinning = !g_spinning;
					break;
			case 'p':
					dropping = !dropping;
					draw_pause = !draw_pause;
					g_spin = 0;
					break;
			case 'c':
					if(texture == 3) 
					{
						texture = 0;
					} else {
						texture += 1;
					}
					load_and_bind_textures();
					make_pieces();
					break;
			case 'r':
					if (dropping)
					{
						int curr = rotate;
						if(rotate == 270) {
							rotate = 0;
							if(!valid_move()) rotate = curr;
						} else {
							rotate = 90 + rotate;
							if(!valid_move()) rotate = curr;
						}
					}
					break;		
		} 
	} else if (game_curr_state == GAME_MENU)
	{
		switch(key)
		{
			case 'q': exit(1); break;
			case ' ':
				game_curr_state = selection;
				if (selection == 2) reset_game();
				break;
		}
	} else if (game_curr_state == GAME_INTRO)
	{
		switch (key)
		{
			case 'q': exit(1); break;
			case 'b': 
				game_curr_state = 0; 
				selection = 2;
				break;
			case ' ': 
				g_spinning = !g_spinning;
				break;
		}
	} else if (game_curr_state == GAME_LOSE)
	{
		switch (key)
		{
			case 'q': exit(1); break;
			case 'r': 
				playfield.reset_playfield();
				game_curr_state = 2; 
				dropping = true;
				reset_game();
				break;
			case 'b': 
				playfield.reset_playfield();
				game_curr_state = 0;
				break;
		}
	} else if (game_curr_state == GAME_WIN)
	{
		switch (key)
		{
			case 'q': exit(1); break;
			case 'r': 
				playfield.reset_playfield();
				game_curr_state = 2; 
				dropping = true;
				reset_game();
				break;
			case 'b': game_curr_state = 0; break;
		}
	}
	glutPostRedisplay();
}

void special(int key, int, int)
{
	if(game_curr_state == GAME_ACTIVE)
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
		}
	} else if (game_curr_state == GAME_MENU)
	{
		switch (key)
		{
			case GLUT_KEY_UP:
				up = true;
				selection = 2;
				break;
			case GLUT_KEY_DOWN:
				up = false;
				selection = 1;
				break;
		}
	} else if (game_curr_state == GAME_INTRO)
	{
		switch (key)
		{
			case GLUT_KEY_UP:
				break;
			case GLUT_KEY_DOWN:
				break;
		}
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

/*
	Creates the first 3 blocks coming up
	after the first block
*/
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

	for (size_t i=0;i<g_particles.size();i++)
		for (size_t j=0;j<3;j++) // for each dimension
		{
			g_particles[i].position[j] += g_particles[i].direction[j];

			// if out of bounds then reverse direction 
			if (g_particles[i].position[j] > 0.5f)
			{
				g_particles[i].position[j] = 0.5f;
				g_particles[i].direction[j] = -g_particles[i].direction[j];
			}
			else
			if (g_particles[i].position[j] < -0.5f)
			{
				g_particles[i].position[j] = -0.5f;
				g_particles[i].direction[j] = -g_particles[i].direction[j];
			}
		}

	glutPostRedisplay();
}

void init()
{
	srand(time(NULL));

	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {0.5, 0.5, 0.5, 1.0};

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH); 

	// turn on blending and set a blending function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLenum error = glGetError();
	if (error!=GL_NO_ERROR)
		fprintf(stderr, "GL error %s\n", gluErrorString(error));
	
	glEnable(GL_DEPTH_TEST);

	load_and_bind_textures();
	make_pieces();
	first_blocks();
	g_particles = p_generator.make_particles(1000);
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
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	
	glutIdleFunc(idle);

	glutMainLoop(); 

	return 0; 
}

