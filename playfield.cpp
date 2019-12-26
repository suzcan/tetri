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
	Creates array of heights were a collision will occur between placed blocks
	and the current block being added
*/
void TetrisPlayfield::update_drop_stop()
{
	for(size_t h=0; h<10; h++) 
	{
		for(size_t v=0; v<20; v++)
		{
			if(occupied[v][h])
			{
				drop_stop[h] = -1.8 + (0.7 * v); 
			}
		}
	}
}
