#include "playfield.h"

TetrisPlayfield::TetrisPlayfield() {}

/*
	Creates a list of transforms to be inputted into glTranslate
	so that the playfield can show blocks that have already been placed.
	Placed blocks are found by reading the playfields state from the 
	occupied array.
*/
void TetrisPlayfield::update_transforms() 
{
	transforms.clear();
	for(size_t v=0; v<20; v++) 
   	{
   		for(size_t h=0; h<10; h++) 
		{
			if(occupied[v][h]) 
			{
				transforms.push_front(make_tuple(-0.35 + (0.07*h), -0.7 + (0.07*v)));
			} 	
		}
	}
}

/*
	Retrieves the the horizantal index value of a given 
	block position
	abs(int(((curr_block_pos[x][0])/0.07) + 18));
*/
int TetrisPlayfield::get_index_h(float x_pos, float size_b)
{
	// normalize
	int x = int(x_pos * 100); 
	int size = int(size_b * 100);
	int mod = x % 7;
	if(mod != 0) {
		int negate = 1;
		if( x < 0) negate = -1; 
		x = (x - mod + 7) * negate;
	}
	if (x < size * -5) {
		return -1;
	} else if (x == (size * -5)) {
		return 0;
	} else if (x == (size * -4)) {
		return 1;
	} else if (x == (size * -3)) {
		return 2;
	} else if (x == (size * -2)) {
		return 3;
	} else if (x == (size * -1)) {		 
		return 4;
	} else if (x == (size * 0)) {
		return 5;
	} else if (x == (size * 1)) {
		return 6;
	} else if (x == (size * 2)) {
		return 7;
	} else if (x == (size * 3)) {
		return 8;
	} else if (x == (size * 4)) {
		return 9;
	} else {
		return 10;
	}
}


/*
	Retrieves the the vertial index value of a given 
	block position
	abs(int(((curr_block_pos[x][1])/0.07) + 26));
*/
int TetrisPlayfield::get_index_v(float x, float size)
{
	if (x < size * -26) {
		return -1;
	} else if (x >= (size * -26) && x < (size * -25)) {
		return 0;
	} else if (x >= (size * -25) && x < (size * -24)) {
		return 1;
	} else if (x >= (size * -24) && x < (size * -23)) {
		return 2;
	} else if (x >= (size * -23) && x < (size * -22)) {
		return 3;
	} else if (x >= (size * -22) && x < (size * -21)) {		 
		return 4;
	} else if (x >= (size * -21) && x < (size * -20)) {
		return 5;
	} else if (x >= (size * -20) && x < (size * -19)) {
		return 6;
	} else if (x >= (size * -19) && x < (size * -18)) {
		return 7;
	} else if (x >= (size * -18) && x < (size * -17)) {
		return 8;
	} else if (x >= (size * -17) && x < (size * -16)) {
		return 9;
	} else if (x >= (size * -16) && x < (size * -15)) {
		return 10;
	} else if (x >= (size * -15) && x < (size * -14)) {
		return 11;
	} else if (x >= (size * -14) && x < (size * -13)) {
		return 12;
	} else if (x >= (size * -13) && x < (size * -12)) {
		return 13;
	} else if (x >= (size * -12) && x < (size * -11)) {
		return 14;
	} else if (x >= (size * -11) && x < (size * -10)) {		 
		return 15;
	} else if (x >= (size * -10) && x < (size * -9)) {
		return 16;
	} else if (x >= (size * -9) && x < (size * -8)) {
		return 17;
	} else if (x >= (size * -8) && x < (size * -7)) {
		return 18;
	} else if (x >= (size * -7) && x < (size * -6)) {
		return 19;
	} else {
		return 20;
	}
	/*
	float stop = initial_stop + 0.02;	
	if (x < stop) {
		return -1;
	} else if (x >= stop + (size * 1) && x < stop + (size * 2)) {
		return 0;
	} else if (x >= stop + (size * 2) && x < stop + (size * 3)) {
		return 1;
	} else if (x >= stop + (size * 3) && x < stop + (size * 4)) {
		return 2;
	} else if (x >= stop + (size * 4) && x < stop + (size * 5)) {
		return 3;
	} else if (x >= stop + (size * 5) && x < stop + (size * 6)) {		 
		return 4;
	} else if (x >= stop + (size * 6) && x < stop + (size * 7)) {
		return 5;
	} else if (x >= stop + (size * 7) && x < stop + (size * 8)) {
		return 6;
	} else if (x >= stop + (size * 8) && x < stop + (size * 9)) {
		return 7;
	} else if (x >= stop + (size * 9) && x < stop + (size * 10)) {
		return 8;
	} else if (x >= stop + (size * 10) && x < stop + (size * 11)) {
		return 9;
	} else if (x >= stop + (size * 11) && x < stop + (size * 12)) {
		return 10;
	} else if (x >= stop + (size * 12) && x < stop + (size * 13)) {
		return 11;
	} else if (x >= stop + (size * 13) && x < stop + (size * 14)) {
		return 12;
	} else if (x >= stop + (size * 14) && x < stop + (size * 15)) {
		return 13;
	} else if (x >= stop + (size * 15) && x < stop + (size * 16)) {		 
		return 14;
	} else if (x >= stop + (size * 16) && x < stop + (size * 17)) {
		return 15;
	} else if (x >= stop + (size * 17) && x < stop + (size * 18)) {
		return 16;
	} else if (x >= stop + (size * 18) && x < stop + (size * 19)) {
		return 17;
	} else if (x >= stop + (size * 19) && x < stop + (size * 20)) {
		return 18;
	} else if (x >= stop + (size * 20) && x < stop + (size * 21)) {
		return 19;
	} else {
		return 20;
	}
	/**/
}
