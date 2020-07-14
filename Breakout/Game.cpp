#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	// Shaders
	ResourceManager::LoadShader("res/shaders/Sprite.shader", "sprite");

	// Shader Configuration
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 0.0f);
	ResourceManager::GetShader("sprite").Bind().SetUniform1i("image", 0);
	ResourceManager::GetShader("sprite").SetUniformMatrix4fv("projection", projection);

	// Render-Specific Controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// Textures
	ResourceManager::LoadTexture("res/textures/backgrounds/terraria.png", true, "background");
	
	ResourceManager::LoadTexture("res/textures/sprites/doomguy.png", true, "slayer");
	
	ResourceManager::LoadTexture("res/textures/blocks_terraria/overworld/high_res/dirt.png", true, "dirt");
	ResourceManager::LoadTexture("res/textures/blocks_terraria/overworld/high_res/sand.png", true, "stone");
	ResourceManager::LoadTexture("res/textures/blocks_terraria/overworld/high_res/silver.png", true, "silver");
	ResourceManager::LoadTexture("res/textures/blocks_terraria/overworld/high_res/gold.png", true, "gold");
	ResourceManager::LoadTexture("res/textures/blocks_terraria/overworld/high_res/obsidian.png", true, "obsidian");
	//ResourceManager::LoadTexture("res/textures/block_solid.png", false, "block_solid");
	
	//ResourceManager::LoadTexture("res/textures/paddle.png", true, "paddle");

	// Levels
	GameLevel one; one.Load("res/levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("res/levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("res/levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("res/levels/four.lvl", this->Width, this->Height / 2);
	Levels.push_back(one);
	Levels.push_back(two);
	Levels.push_back(three);
	Levels.push_back(four);
	this->Level = 0;

	// Game Objects

}

void Game::ProcessInput(float dt)
{

}

void Game::Update(float dt)
{

}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Texture2D backgroundTexture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(backgroundTexture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		this->Levels[this->Level].Draw(*Renderer);
	}
	
	//Texture2D myTexture;
	//myTexture = ResourceManager::GetTexture("slayer");
	//Renderer->DrawSprite(myTexture, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}