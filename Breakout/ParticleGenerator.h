#pragma once

#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"

#include <vector>

struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	void Draw();
private:
	std::vector<Particle> particles;
	unsigned int amount;

	Shader shader;
	Texture2D texture;
	unsigned int VAO;

	void Init(); // Init VAO and VBO
	unsigned int FirstUnusedParticle(); // Returns first particle index that's currently unused
	void RespawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif // !PARTICLE_GENERATOR_H