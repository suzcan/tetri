#include "blockbuilder.h"

BlockBuilder::BlockBuilder() {}

void BlockBuilder::make_block(float r, float g, float b)
{	
	for (size_t s=0;s<6;s++)
		{
			//glBindTexture(GL_TEXTURE_2D, g_camper_tex[s]);
			glColor3f(r, g, b);
			glBegin(GL_QUADS);
				for (size_t i=0;i<4;i++)
				{
					// TODO: TEXTURES
					glVertex3fv(block_vertices[s][i]);
				}
			glEnd();
		}
}

void BlockBuilder::make_large_block(float vertices[][2] ) 
{
	glPushMatrix();
		for(size_t i = 0; i < 4; i++) 
		{
			glTranslatef(vertices[i][0], 
						vertices[i][1],
						0.0f);
			make_block();
		}
	glPopMatrix();
}
