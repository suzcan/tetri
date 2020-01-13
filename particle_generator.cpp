#include "particle_generator.h"

ParticleGenerator::ParticleGenerator() {}

unsigned char* ParticleGenerator::blob_image(size_t size)
{
	unsigned char* im = new unsigned char[4*size*size];
	const float sd2 = (float(size)/3.0f)*(float(size)/3.0f);
	size_t p = 0;
	for (size_t i=0;i<size;i++)
	for (size_t j=0;j<size;j++,p+=4)
	{
		const float x = float(j) - float(size/2);
		const float y = float(i) - float(size/2);

		const float g = exp(-2*(x*x+y*y)/sd2);
		im[p+0] = 0;
		im[p+1] = 0;
		im[p+2] = 0;
		im[p+3] = (unsigned char)(255 * g); 
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

	float rand_x = float(drand48());

	for (size_t i=0;i<n;i++)
	{
		particle_t p;

		p.size = 0.015f;

		float rand_r = float(drand48());
		float rand_g = float(drand48());
		float rand_b = float(drand48());

		p.colour[0] = rand_r;
		p.colour[1] = rand_g;
		p.colour[2] = rand_b;
		p.colour[3] = 0.75f;

		p.position[0] = rand_x -0.5;
		p.position[1] = y;
		p.position[2] = -0.5;

		p.direction[0] = 0.0;
		p.direction[1] = 0.02;
		p.direction[2] = 0.0;

		glGenTextures(1, &(p.tex));
		glBindTexture(GL_TEXTURE_2D, p.tex);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_colour);

		glTexImage2D(GL_TEXTURE_2D, 0,
					GL_RGBA8, size, size, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, (void*)im);

		g_particles.push_back(p);
	}

	delete[] im;
    return g_particles;
}

