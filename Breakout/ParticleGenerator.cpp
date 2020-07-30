#include "ParticleGenerator.h"

int particleTimer = 0;

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->Init();
}

void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	// Add new particles
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->FirstUnusedParticle();
		this->RespawnParticle(this->particles[unusedParticle], object, offset);
	}

	// Update all particles
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f) // particle is alive
		{
			p.Position -= p.Velocity * dt;
			p.Color -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::Update(float dt, glm::vec2 position, unsigned int newParticles, glm::vec2 offset)
{
	// Add new particles
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->FirstUnusedParticle();
		this->RespawnParticle(this->particles[unusedParticle], position, offset);
	}

	// Update all particles
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f) // particle is alive
		{
			p.Position -= p.Velocity * dt;
			p.Color -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::Draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // use additive blending for 'glow' effect
	this->shader.Bind();
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader.SetUniform2f("offset", particle.Position);
			this->shader.SetUniform4f("color", particle.Color);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::Init()
{
	unsigned int VBO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), &particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);

	for (unsigned int i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::FirstUnusedParticle()
{
	// Search from last used particle - usually return instantly
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i)
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// Otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i)
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// All particles are taken, so override the first one
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}

void ParticleGenerator::RespawnParticle(Particle& particle, glm::vec2 position, glm::vec2 offset)
{
	particleTimer++;
	if (particleTimer >= 100)
		particleTimer = 0;

	float theta = 1000.0f * 3.1315826f * float(particleTimer);
	float x = cosf(theta) * 5.0f * float(particleTimer % 10);
	float y = sinf(theta) * 5.0f * float(particleTimer % 10);
	particle.Position = glm::vec2(x + position.x + offset.x, y + position.y + offset.y);
	particle.Position = glm::vec2(x + position.x + offset.x, y + position.y + offset.y);

	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = glm::vec2(0.0f, 0.0f) * 0.1f;
}