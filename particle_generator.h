#ifndef __PARTICLEGENH__
#define __PARTICLEGENH__

#include <GL/glut.h>
#include <iostream>

#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include <vector>
#include <cmath>


class ParticleGenerator
{
	public:
		ParticleGenerator();

        struct particle_t{
	        unsigned int tex;
	        float colour[4];
	        float position[3]; 
	        float direction[3];
	        float size; 
        };

        unsigned char* blob_image(size_t size);
        std::vector<ParticleGenerator::particle_t> make_particles(const size_t n, 
																float x=0.0f, 
																float y=0.0f);
};


#endif