#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;
GameObject* Player;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
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
		// Levels
	/*ResourceManager::LoadTexture("res/textures/backgrounds/overworld/Overworld2.png", true, "overworld");
	ResourceManager::LoadTexture("res/textures/backgrounds/caverns/Cavern3.png", true, "cavern");
	ResourceManager::LoadTexture("res/textures/backgrounds/crimson/Crimson2.png", true, "crimson");
	ResourceManager::LoadTexture("res/textures/backgrounds/hell/Hell1.png", true, "hell");
		// Sprites
	ResourceManager::LoadTexture("res/textures/sprites/doomguy.png", true, "slayer");
	ResourceManager::LoadTexture("res/textures/sprites/steve.png", false, "steve");
	ResourceManager::LoadTexture("res/textures/sprites/wood_platform.png", true, "paddle");
	ResourceManager::LoadTexture("res/textures/sprites/beach_ball.png", true, "ball");
		// Blocks
	ResourceManager::LoadTexture("res/textures/blocks/soil/dirt.png", true, "dirt");
	ResourceManager::LoadTexture("res/textures/blocks/soil/sand.png", true, "stone");
	ResourceManager::LoadTexture("res/textures/blocks/brick/silver.png", true, "silver");
	ResourceManager::LoadTexture("res/textures/blocks/brick/gold.png", true, "gold");
	ResourceManager::LoadTexture("res/textures/blocks/decoration/obsidian.png", true, "obsidian");*/
	
	ResourceManager::LoadTexture("res/textures/misc/background.jpg", false, "background");
	ResourceManager::LoadTexture("res/textures/misc/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("res/textures/misc/block.png", false, "block");
	ResourceManager::LoadTexture("res/textures/misc/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("res/textures/misc/paddle.png", true, "paddle");

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
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
		}
	}
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

		Player->Draw(*Renderer);

		Texture2D faceTex = ResourceManager::GetTexture("face");
		Renderer->DrawSprite(faceTex, glm::vec2(this->Width * 0.4f, this->Height * 0.6f), glm::vec2(50.0f, 50.0f), 0.0f);
		
		this->Levels[this->Level].Draw(*Renderer);
		
		//Texture2D myTexture;
		//myTexture = ResourceManager::GetTexture("slayer");
		//Renderer->DrawSprite(myTexture, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	
}