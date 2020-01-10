#ifndef __BLOCKBUILDERH__
#define __BLOCKBUILDERH__

#include <GL/glut.h> 
#include <iostream>

class BlockBuilder
{
	public:
		BlockBuilder();
		const float block_size = 0.07f;
		
		// block construction
		const float g_l = -0.035f; // left
		const float g_r = 0.035f; // right
		const float g_t = 0.035f; // top
		const float g_b = -0.035f; // bottom
		const float g_f = 0.035f; // front
		const float g_k = -0.035f; // forward
		
		const float block_vertices[6][4][3] = {
			{ {g_l, g_b, g_f}, {g_r, g_b, g_f}, {g_r, g_t, g_f}, {g_l, g_t, g_f} }, // front
			{ {g_r, g_b, g_f}, {g_r, g_b, g_k}, {g_r, g_t, g_k}, {g_r, g_t, g_f} }, // left-side 
			{ {g_l, g_b, g_k}, {g_l, g_b, g_f}, {g_l, g_t, g_f}, {g_l, g_t, g_k} }, // right-side
			{ {g_r, g_t, g_f}, {g_r, g_t, g_k}, {g_l, g_t, g_k}, {g_l, g_t, g_f} }, // top
			{ {g_r, g_b, g_k}, {g_l, g_b, g_k}, {g_l, g_t, g_k}, {g_r, g_t, g_k} }, // back
			{ {g_r, g_b, g_f}, {g_r, g_b, g_k}, {g_l, g_b, g_k}, {g_l, g_b, g_f} } // bottom
		};

		int edge = 1;
		// for 340x340 image
		int g_tex_coord[4][2] = {
			// {bottom-left, bottom-right, top-right, top-left}
			{0, edge}, {edge, edge}, {edge, 0}, {0, 0} // front
		};
		
		float o_block_vertices[4][2] = {{0.0f, 0.0f}, {0.07f, 0.0f}, {0.0f, 0.07f}, {-0.07f, 0.0f}};
		float t_block_vertices[4][2] = {{0.0f, 0.0f}, {0.07f, 0.0f}, {0.0f, -0.07f}, {0.07f, 0.07f}};
		float s_block_vertices[4][2] = {{0.0f, 0.0f}, {0.07f, 0.0f}, {0.0f, 0.07f}, {0.07f, 0.0f}};
		float z_block_vertices[4][2] = {{0.0f, 0.0f}, {0.07f, 0.0f}, {0.0f, -0.07f}, {0.07f, 0.0f}};
		float i_block_vertices[4][2] = {{0.0f, 0.0f}, {0.0f, 0.07f}, {0.0f, 0.07f}, {0.0f, 0.07f}};
		float l_block_vertices[4][2] = {{0.0f, 0.0f}, {-0.07f, 0.0f}, {0.0f, 0.07f}, {0.0f, 0.07f}};
		float j_block_vertices[4][2] = {{0.0f, 0.0f}, {0.07f, 0.0f}, {0.0f, 0.07f}, {0.0f, 0.07f}};
		
		void make_block(unsigned int tex, float r=0.0f, float g=1.0f, float b=0.0f);
		void make_large_block(float vertices[][2],unsigned int tex);

		void make_o_block(unsigned int tex);
		void make_t_block(unsigned int tex);
		void make_s_block(unsigned int tex);
		void make_z_block(unsigned int tex);
		void make_i_block(unsigned int tex);
		void make_l_block(unsigned int tex);
		void make_j_block(unsigned int tex);

};

#endif
