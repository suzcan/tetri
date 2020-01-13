#include "blockbuilder.h"
#include"load_and_bind_texture.h"

BlockBuilder::BlockBuilder() {}

void BlockBuilder::make_block(unsigned int tex, float r, float g, float b)
{	
	glEnable(GL_TEXTURE_2D);
	for (size_t s=0;s<6;s++)
		{
			glBindTexture(GL_TEXTURE_2D, tex);
			glBegin(GL_QUADS);
				for (size_t i=0;i<4;i++)
				{
					glTexCoord2f(
						g_tex_coord[i][0],
						g_tex_coord[i][1]
					);
					glVertex3fv(block_vertices[s][i]);
				}
			glEnd();
		}
	glDisable(GL_TEXTURE_2D);
}

void BlockBuilder::make_large_block(float vertices[][2], unsigned int tex, int num) 
{
	glPushMatrix();
		for(size_t i = 0; i < num; i++) 
		{
			glTranslatef(vertices[i][0], 
						vertices[i][1],
						0.0f);
			make_block(tex);
		}
	glPopMatrix();
}

void BlockBuilder::make_o_block(unsigned int tex)
{
	make_large_block(o_block_vertices, tex);
}

void BlockBuilder::make_t_block(unsigned int tex)
{
	make_large_block(t_block_vertices, tex);
}

void BlockBuilder::make_s_block(unsigned int tex)
{
	make_large_block(s_block_vertices, tex);
}

void BlockBuilder::make_z_block(unsigned int tex)
{
	make_large_block(z_block_vertices, tex);
}

void BlockBuilder::make_i_block(unsigned int tex)
{
	make_large_block(i_block_vertices, tex);
}

void BlockBuilder::make_l_block(unsigned int tex)
{
	make_large_block(l_block_vertices, tex);
}

void BlockBuilder::make_j_block(unsigned int tex)
{
	make_large_block(j_block_vertices, tex);
}

void BlockBuilder::make_letter(float vertices[][2], unsigned int tex)
{
	make_large_block(vertices, tex, 11);
}

/*
	Creates the title "TETRIS" in 3d blocks
*/
void BlockBuilder::make_title(unsigned int tex)
{
	glPushMatrix();
		make_letter(letter_t_vertices, tex);
		glPushMatrix();
			glTranslatef(0.30f, -0.10f, 0.0f);
			make_letter(letter_e_vertices, tex);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.45f, 0.0f, 0.0f);
			make_letter(letter_t_vertices, tex);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.75f, -0.10f, 0.0f);
			make_letter(letter_r_vertices, tex);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.0f, -0.10f, 0.0f);
			make_letter(letter_i_vertices, tex);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.25f, -0.10f, 0.0f);
			make_letter(letter_s_vertices, tex);
		glPopMatrix();
	glPopMatrix();
}