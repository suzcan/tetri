#ifndef __PLAYFIELDH__
#define __PLAYFIELDH__

#include <iostream>
#include <list>
#include <tuple>

#include <stdlib.h>
#include <stddef.h>

using namespace std;


class TetrisPlayfield
{
	public:
		TetrisPlayfield();
		
		// playfield construction
		const float g_l = -0.42f;
		const float g_r = 0.35f;
		const float g_t = 0.7f;
		const float g_b = -0.73f;
		const float g_f = 0.07f;
		const float g_k = -0.07f;
		
		
		const float g_l_n = 0.42f;
		const float g_r_n = 0.82f;
		const float g_b_n = -0.14;
		/**/
		const float playfield_vertices[5][4][3] = {
			{ {g_l, g_b, g_f}, {g_r, g_b, g_f}, {g_r, g_t, g_f}, {g_l, g_t, g_f} }, // front
			{ {g_r, g_b, g_f}, {g_r, g_b, g_k}, {g_r, g_t, g_k}, {g_r, g_t, g_f} }, // left-side 
			{ {g_l, g_b, g_k}, {g_l, g_b, g_f}, {g_l, g_t, g_f}, {g_l, g_t, g_k} }, // right-side
			{ {g_r, g_t, g_f}, {g_r, g_t, g_k}, {g_l, g_t, g_k}, {g_l, g_t, g_f} }, // top
			{ {g_r, g_b, g_k}, {g_l, g_b, g_k}, {g_l, g_t, g_k}, {g_r, g_t, g_k} }  // back
		};
		
		const float playfield_floor[4][3] = {
			{g_l, g_b, g_f}, {g_r, g_b, g_f}, {g_r, g_b, g_k}, {g_l, g_b, g_k}
		}; 
		
		
		const float playfield_up_next[5][4][3] = {
			{ {g_l_n, g_b_n, g_f}, {g_r_n, g_b_n, g_f}, {g_r_n, g_t, g_f}, {g_l_n, g_t, g_f} }, // front
			{ {g_r_n, g_b_n, g_f}, {g_r_n, g_b_n, g_k}, {g_r_n, g_t, g_k}, {g_r_n, g_t, g_f} }, // left-side 
			{ {g_l_n, g_b_n, g_k}, {g_l_n, g_b_n, g_f}, {g_l_n, g_t, g_f}, {g_l_n, g_t, g_k} }, // right-side
			{ {g_r_n, g_t, g_f}, {g_r_n, g_t, g_k}, {g_l_n, g_t, g_k}, {g_l_n, g_t, g_f} }, // top
			{ {g_r_n, g_b_n, g_k}, {g_l_n, g_b_n, g_k}, {g_l_n, g_t, g_k}, {g_r_n, g_t, g_k} }  // back
		};
		/**/
	
		// playfield state
		bool occupied[25][10] = {
			{false, false, false, false, false, false, false, false, false, false}, // 1
			{false, false, false, false, false, false, false, false, false, false}, 
			{false, false, false, false, false, false, false, false, false, false}, 
			{false, false, false, false, false, false, false, false, false, false}, 
			{false, false, false, false, false, false, false, false, false, false}, // 5
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false}, // 10
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false}, // 15
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},  // 20
			{false, false, false, false, false, false, false, false, false, false}, 
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false},
			{false, false, false, false, false, false, false, false, false, false}   //25
		};
		
		// used to create blocks on occupied space
		list<tuple<float, float>> transforms;
		void update_transforms();
		
		// collision calculation
		float initial_stop = -1.8f;
		
		int get_index_h(float x_pos, float size_b);
		int get_index_v(float x, float o_size);
};

#endif
