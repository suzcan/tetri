#include "particle_generator.h"

ParticleGenerator::ParticleGenerator() {}

unsigned char* ParticleGenerator::blob_image(size_t size)
{
	unsigned char* im = new unsigned char[4*size*size];
	const float sd2 = (float(size)/3.0f)*(float(size)/3.0f);

    //unsigned char m = (unsigned char)(size/8); // mask for checker board
	size_t p = 0;
	for (size_t i=0;i<size;i++)
	for (size_t j=0;j<size;j++,p+=4)
	{
		const float x = float(j) - float(size/2);
		const float y = float(i) - float(size/2);

		const float g = exp(-2*(x*x+y*y)/sd2);
		//const float g = float(((i&m)==0)^((j&m)==0)); // checker board
		im[p+0] = 0;
		im[p+1] = 0;
		im[p+2] = 0;
		im[p+3] = (unsigned char)(255 * g); // alpha value
	}

	return im;
}

std::vector<ParticleGenerator::particle_t> ParticleGenerator::make_particles(const size_t n, float x, float y)
{
    std::vector<particle_t> g_particles;
	const size_t size = 64;
	unsigned char* im = blob_image(size);

	const float env_colour[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	srand48(time(NULL));

	for (size_t i=0;i<n;i++)
	{
		particle_t p;

		p.size = 0.005f; // we could make them different sizes

		float rand_r = float(drand48());
		float rand_g = float(0);
		float rand_b = float(0);
		p.colour[0] = rand_r;
		p.colour[1] = rand_g;
		p.colour[2] = rand_b;
		p.colour[3] = 0.75f;

		float rand_x = float(drand48()) - 0.5f;
		float rand_y = float(drand48()) - 0.5f;
		float rand_z = float(drand48()) - 0.5f;
		p.position[0] = 0.0f;//rand_x;
		p.position[1] = rand_y;
		p.position[2] = -0.5;//rand_z;

		// random direction of movement
		float rand_dx = (float(drand48()) - 0.5f)/20;
		float rand_dy = (float(drand48()) - 0.5f)/20;
		float rand_dz = 0.0;

		// debug on the initial particle state
		//printf("\tparticle %d: dx = %f, dy = %f, dz = %f\n", int(i), rand_dx, rand_dy, rand_dz);
		p.direction[0] = rand_dx;
		p.direction[1] = rand_y + 0.5f;;
		p.direction[2] = rand_dz;

		glGenTextures(1, &(p.tex));
		glBindTexture(GL_TEXTURE_2D, p.tex);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_colour);

		glTexImage2D(GL_TEXTURE_2D, 0,
					GL_RGBA8, size, size, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, (void*)im);

		g_particles.push_back(p);
	}

	delete[] im; // free up the image
    return g_particles;
}

