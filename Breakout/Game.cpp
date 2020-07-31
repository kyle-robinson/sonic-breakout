#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "TextRenderer.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <irrKlang/irrKlang.h>

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
ParticleGenerator* SuperParticles;
ParticleGenerator* PassthroughParticles;
ParticleGenerator* BoxParticles;
PostProcessor* Effects;
TextRenderer* Text;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

glm::vec2 boxPosition(-100.0f, -100.0f);
float ShakeTime = 0.0f;
bool isPlaying = false;

bool super_sonic = false;
bool notStarted = true;

bool boxDestroyed = false;
int boxTimer = 10;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Level(0), Lives(3)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete SuperParticles;
	delete PassthroughParticles;
	delete BoxParticles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init()
{
	// Shaders
	ResourceManager::LoadShader("res/shaders/Sprite.shader", "sprite");
	ResourceManager::LoadShader("res/shaders/Particle.shader", "particle");
	ResourceManager::LoadShader("res/shaders/PostProcessing.shader", "post_processor");

	// Shader Configuration
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 0.0f);
	ResourceManager::GetShader("sprite").Bind().SetUniform1i("image", 0);
	ResourceManager::GetShader("sprite").SetUniformMatrix4fv("projection", projection);

	ResourceManager::GetShader("particle").Bind().SetUniform1i("sprite", 0);
	ResourceManager::GetShader("particle").SetUniformMatrix4fv("projection", projection);

	// Textures
	// Backgrounds
	ResourceManager::LoadTexture("res/textures/sprites/title.png", true, "title");
	ResourceManager::LoadTexture("res/textures/backgrounds/green-hill-zone.png", false, "green-hill-zone");
	ResourceManager::LoadTexture("res/textures/backgrounds/marble-zone.png", false, "marble-zone");
	ResourceManager::LoadTexture("res/textures/backgrounds/starlight-zone.png", false, "starlight-zone");
	ResourceManager::LoadTexture("res/textures/backgrounds/final-zone.png", false, "final-zone");
	// Ball
	ResourceManager::LoadTexture("res/textures/sprites/sonic.png", true, "sonic");
	ResourceManager::LoadTexture("res/textures/sprites/super-sonic.png", true, "super-sonic");
	// Player
	ResourceManager::LoadTexture("res/textures/sprites/green-hill-platform.png", true, "green-hill-platform");
	ResourceManager::LoadTexture("res/textures/sprites/marble-platform.png", true, "marble-platform");
	ResourceManager::LoadTexture("res/textures/sprites/starlight-platform.png", true, "starlight-platform");
	ResourceManager::LoadTexture("res/textures/sprites/final-platform.png", true, "final-platform");
	// Particles
	ResourceManager::LoadTexture("res/textures/sprites/particle.png", true, "particle");
	ResourceManager::LoadTexture("res/textures/sprites/super-particle.png", true, "super-particle");
	ResourceManager::LoadTexture("res/textures/sprites/pass-particle.png", true, "passthrough-particle");
	ResourceManager::LoadTexture("res/textures/sprites/box-particle.png", true, "box-particle");
	// Blocks
	ResourceManager::LoadTexture("res/textures/blocks/blue-block.png", false, "blue");
	ResourceManager::LoadTexture("res/textures/blocks/dark-blue-block.png", false, "dark-blue");
	ResourceManager::LoadTexture("res/textures/blocks/teal-block.png", false, "teal");
	ResourceManager::LoadTexture("res/textures/blocks/green-block.png", false, "green");
	ResourceManager::LoadTexture("res/textures/blocks/red-block.png", false, "red");
	ResourceManager::LoadTexture("res/textures/blocks/yellow-block.png", false, "yellow");
	ResourceManager::LoadTexture("res/textures/blocks/pink-block.png", false, "pink");
	ResourceManager::LoadTexture("res/textures/blocks/metal.png", true, "metal");
	// Powerups
	ResourceManager::LoadTexture("res/textures/sprites/yellow-emerald.png", true, "powerup_speed");
	ResourceManager::LoadTexture("res/textures/sprites/purple-emerald.png", true, "powerup_sticky");
	ResourceManager::LoadTexture("res/textures/sprites/blue-emerald.png", true, "powerup_increase");
	ResourceManager::LoadTexture("res/textures/sprites/red-emerald.png", true, "powerup_passthrough");
	ResourceManager::LoadTexture("res/textures/sprites/star-enemy.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("res/textures/sprites/eggman.png", true, "powerup_chaos");

	// Render-Specific Controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	SuperParticles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("super-particle"), 500);
	PassthroughParticles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("passthrough-particle"), 500);
	BoxParticles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("box-particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("post_processor"), this->Width, this->Height);

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

	// Audio
	SoundEngine->setSoundVolume(0.1f);

	// Game Objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("sonic"));

	// Text
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("res/fonts/NiseSegaSonic.TTF", 24);
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Circle = false;
			isPlaying = false;
			SoundEngine->stopAllSounds();
			switch (this->Level)
			{
			case 0:
				SoundEngine->play2D("res/audio/music/green-hill.mp3", true);
				break;
			case 1:
				SoundEngine->play2D("res/audio/music/marble.mp3", true);
				break;
			case 2:
				SoundEngine->play2D("res/audio/music/starlight.mp3", true);
				break;
			case 3:
				SoundEngine->play2D("res/audio/music/final.mp3", true);
				break;
			}
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}

	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
		
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("res/audio/effects/pause.mp3", false);
			Effects->Darken = true;
			this->KeysProcessed[GLFW_KEY_P] = true;
			this->State = GAME_PAUSE;
		}
	}

	if (this->State == GAME_MENU || this->State == GAME_ACTIVE)
	{
		if (this->Keys[GLFW_KEY_TAB] && !this->KeysProcessed[GLFW_KEY_TAB])
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("res/audio/effects/pause.mp3", false);
			Effects->Darken = true;
			this->KeysProcessed[GLFW_KEY_TAB] = true;
			this->State = GAME_HELP;
		}
	}

	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}

	if (this->State == GAME_LOSE)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Confuse = false;
			this->State = GAME_MENU;
		}
	}

	if (this->State == GAME_PAUSE)
	{
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
		{
			switch (this->Level)
			{
			case 0:
				SoundEngine->play2D("res/audio/music/green-hill.mp3", true);
				break;
			case 1:
				SoundEngine->play2D("res/audio/music/marble.mp3", true);
				break;
			case 2:
				SoundEngine->play2D("res/audio/music/starlight.mp3", true);
				break;
			case 3:
				SoundEngine->play2D("res/audio/music/final.mp3", true);
				break;
			}
			Effects->Darken = false;
			this->KeysProcessed[GLFW_KEY_P] = true;
			this->State = GAME_ACTIVE;
		}
	}

	if (this->State == GAME_HELP)
	{
		if (this->Keys[GLFW_KEY_TAB] && !this->KeysProcessed[GLFW_KEY_TAB])
		{
			Effects->Darken = false;
			this->KeysProcessed[GLFW_KEY_TAB] = true;
			if (notStarted)
			{
				SoundEngine->stopAllSounds();
				SoundEngine->play2D("res/audio/music/title.mp3", true);
				this->State = GAME_MENU;
			}
			else
			{
				this->State = GAME_ACTIVE;
				switch (this->Level)
				{
				case 0:
					SoundEngine->play2D("res/audio/music/green-hill.mp3", true);
					break;
				case 1:
					SoundEngine->play2D("res/audio/music/marble.mp3", true);
					break;
				case 2:
					SoundEngine->play2D("res/audio/music/starlight.mp3", true);
					break;
				case 3:
					SoundEngine->play2D("res/audio/music/final.mp3", true);
					break;
				}
			}
		}
	}
}

void Game::Update(float dt)
{	
	if (this->State == GAME_MENU)
	{
		notStarted = true;
		if (!isPlaying)
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("res/audio/music/title.mp3", true);
			isPlaying = true;
		}
		Effects->Circle = true;
	}
	
	if (this->State == GAME_ACTIVE)
	{
		notStarted = false;
		
		Ball->PassThrough ? PassthroughParticles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f))
			: (super_sonic ? SuperParticles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f))
				: Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f)));

		if (boxTimer < 0)
			boxPosition = glm::vec2(-100.0f, -100.0f);
		BoxParticles->Update(dt, boxPosition, 2, glm::vec2(25.0f, 20.0f));

		Ball->Move(dt, this->Width);
		Ball->Stuck ? Ball->Rotation = 0.0f : Ball->Rotation += 20.0f;
		this->DoCollisions();
		this->UpdatePowerUps(dt);

		if (ShakeTime > 0.0f)
		{
			ShakeTime -= dt;
			if (ShakeTime <= 0.0f)
				Effects->Shake = false;
		}

		if (Ball->Position.y >= this->Height)
		{
			--this->Lives;
			Ball->Velocity = INITIAL_BALL_VELOCITY;
			super_sonic = false;
			
			if (this->Lives == 0)
			{
				this->ResetLevel();
				this->State = GAME_LOSE;
				Effects->Confuse = true;
				Effects->Circle = true;
				SoundEngine->stopAllSounds();
				SoundEngine->play2D("res/audio/effects/game-over.mp3", false);
				SoundEngine->play2D("res/audio/eggman/laughing-long.mp3", false);
			}
			else
				SoundEngine->play2D("res/audio/effects/death.mp3", false);

			this->ResetPlayer();
		}
	}

	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;
		SoundEngine->stopAllSounds();
		SoundEngine->play2D("res/audio/effects/stage-clear.mp3", false);
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN || this->State == GAME_LOSE || this->State == GAME_PAUSE || this->State == GAME_HELP)
	{
		Effects->BeginRender();
		
		Texture2D backgroundTexture;
		Texture2D playerTexture;
		switch (this->Level)
		{
		case 0:
			playerTexture = ResourceManager::GetTexture("green-hill-platform");
			backgroundTexture = ResourceManager::GetTexture("green-hill-zone");
			break;
		case 1:
			playerTexture = ResourceManager::GetTexture("marble-platform");
			backgroundTexture = ResourceManager::GetTexture("marble-zone");
			break;
		case 2:
			playerTexture = ResourceManager::GetTexture("starlight-platform");
			backgroundTexture = ResourceManager::GetTexture("starlight-zone");
			break;
		case 3:
			playerTexture = ResourceManager::GetTexture("final-platform");
			backgroundTexture = ResourceManager::GetTexture("final-zone");
			break;
		}
		Renderer->DrawSprite(backgroundTexture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
				
		this->Levels[this->Level].Draw(*Renderer);
		
		Player->Sprite = playerTexture;
		Player->Draw(*Renderer);

		if (this->State == GAME_ACTIVE)
		{
			for (PowerUp& powerUp : this->PowerUps)
				if (!powerUp.Destroyed)
					powerUp.Draw(*Renderer);
		}
			
		Ball->PassThrough ? PassthroughParticles->Draw() : (super_sonic ? SuperParticles->Draw() : Particles->Draw());
		
		if (boxDestroyed)
		{
			boxTimer = 10;
			boxDestroyed = false;
		}

		if (boxTimer >= 0)
		{
			BoxParticles->Draw();
			boxTimer--;
		}

		Texture2D sonicTexture = ResourceManager::GetTexture("sonic");
		Texture2D superTexture = ResourceManager::GetTexture("super-sonic");
		super_sonic ? Ball->Sprite = superTexture : Ball->Sprite = sonicTexture;
		Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());

		if (this->State == GAME_ACTIVE)
		{
			if (Ball->Stuck && !Ball->Sticky)
			{
				Text->RenderText("Press 'SPACE' to launch Sonic!", 247.5f, 427.5f, 1.0f, glm::vec3(0.0f));
				Text->RenderText("Press 'SPACE' to launch Sonic!", 250.0f, 430.0f, 1.0f);
			}
			
			std::stringstream ss; ss << this->Lives;
			Text->RenderText("Lives: " + ss.str(), 2.5f, 2.5f, 1.0f, glm::vec3(0.0f));
			Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f);
			
			Text->RenderText("'P' to pause", 372.5f, 2.5f, 1.0f, glm::vec3(0.0f));
			Text->RenderText("'P' to pause", 375.0f, 5.0f, 1.0f);
			
			Text->RenderText("'TAB' for help", this->Width - 242.5f, 2.5f, 1.0f, glm::vec3(0.0f));
			Text->RenderText("'TAB' for help", this->Width - 240.0f, 5.0f, 1.0f);
		}
	}

	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start!", 317.5f, Height / 2 + 37.5f, 1.0f, glm::vec3(0.0f));
		Text->RenderText("Press ENTER to start!", 320.0f, Height / 2 + 40.0f, 1.0f);
		Text->RenderText("Press W or S to select level.", 317.5f, Height / 2 + 77.5f, 0.75f, glm::vec3(0.0f));
		Text->RenderText("Press W or S to select level.", 320.0f, Height / 2 + 80.0f, 0.75f);
		Text->RenderText("Press TAB for help.", 377.5f, Height / 2 + 117.5f, 0.75f, glm::vec3(0.0f));
		Text->RenderText("Press TAB for help.", 380.0f, Height / 2 + 120.0f, 0.75f);
		Texture2D titleTexture = ResourceManager::GetTexture("title");
		Renderer->DrawSprite(titleTexture, glm::vec2(this->Width / 2 - 256.0f, this->Height / 2 - 300.0f), glm::vec2(512.0f, 300.0f), 0.0f);
	}

	if (this->State == GAME_WIN)
	{
		Text->RenderText("You WON!!!", 417.5f, Height / 2 - 27.5f, 1.0f, glm::vec3(0.0f));
		Text->RenderText("You WON!!!", 420.0f, Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit.", 187.5f, Height / 2 + 27.5f, 1.0f, glm::vec3(0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit.", 190.0f, Height / 2 + 30.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	if (this->State == GAME_LOSE)
	{
		Text->RenderText("GAME OVER!", 417.5f, Height / 2 - 22.5f, 1.0f, glm::vec3(0.0f));
		Text->RenderText("GAME OVER!", 420.0f, Height / 2 - 20.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit.", 187.5f, Height / 2 + 27.5f, 1.0f, glm::vec3(0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit.", 190.0f, Height / 2 + 30.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	if (this->State == GAME_PAUSE)
	{
		Text->RenderText("PAUSED", this->Width / 2.0f - 50.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f));
	}

	if (this->State == GAME_HELP)
	{
		// Text
			// Titles
		Text->RenderText("HELP MENU", 425.0f, 50.0f, 1.0f, glm::vec3(1.0f));
		Text->RenderText("Power-Ups", 50.0f, 200.0f, 1.0f, glm::vec3(1.0f));
		Text->RenderText("Sprites", 50.0f, 425.0f, 1.0f, glm::vec3(1.0f));
		Text->RenderText("Blocks", 50.0f, 650.0f, 1.0f, glm::vec3(1.0f));

			// Power-Ups
		Text->RenderText("Speed", 293.5f, 250.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Increase", 283.0f, 270.0f, 0.5f, glm::vec3(1.0f));
		
		Text->RenderText("Ball", 420.0f, 250.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Passthrough", 384.5f, 270.0f, 0.5f, glm::vec3(1.0f));
		
		Text->RenderText("Sticky", 533.0f, 250.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Paddle", 530.0f, 270.0f, 0.5f, glm::vec3(1.0f));
		
		Text->RenderText("Size ", 663.5f, 250.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Increase", 645.5f, 270.0f, 0.5f, glm::vec3(1.0f));

		Text->RenderText("Confusion", 757.5f, 250.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Chaos", 905.0f, 250.0f, 0.5f, glm::vec3(1.0f));
		
			// Sprites
		Text->RenderText("Sonic : Ball", 395.0f, 475.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Platform : Player", 710.0f, 475.0f, 0.5f, glm::vec3(1.0f));

			// Blocks
		Text->RenderText("Destructible", 390.0f, 700.0f, 0.5f, glm::vec3(1.0f));
		Text->RenderText("Non-destructible", 715.0f, 700.0f, 0.5f, glm::vec3(1.0f));


		// Powerups
		Texture2D speedTexture = ResourceManager::GetTexture("powerup_speed");
		Texture2D passthroughTexture = ResourceManager::GetTexture("powerup_passthrough");
		Texture2D stickyTexture = ResourceManager::GetTexture("powerup_sticky");
		Texture2D increaseTexture = ResourceManager::GetTexture("powerup_increase");
		Texture2D starTexture = ResourceManager::GetTexture("powerup_confuse");
		Texture2D eggmanTexture = ResourceManager::GetTexture("powerup_chaos");
		Renderer->DrawSprite(speedTexture, glm::vec2(300.0f, 190.0f), glm::vec2(40.0f, 31.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(passthroughTexture, glm::vec2(420.0f, 190.0f), glm::vec2(40.0f, 31.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(stickyTexture, glm::vec2(540.0f, 190.0f), glm::vec2(40.0f, 31.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(increaseTexture, glm::vec2(660.0f, 190.0f), glm::vec2(40.0f, 31.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(starTexture, glm::vec2(780.0f, 190.0f), glm::vec2(40.0f, 31.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(eggmanTexture, glm::vec2(900.0f, 197.5f), glm::vec2(60.0f, 20.0f), 0.0f, glm::vec3(1.0f));
		
		// Sprites
		Texture2D ballTexture;
		Texture2D sonicTexture = ResourceManager::GetTexture("sonic");
		Texture2D superTexture = ResourceManager::GetTexture("super-sonic");
		int timer = glfwGetTime();
		switch (timer % 2)
		{
		case 0:
			ballTexture = sonicTexture;
			break;
		case 1:
			ballTexture = superTexture;
			break;
		}
		Renderer->DrawSprite(ballTexture, glm::vec2(415.0f, 400.0f), glm::vec2(50.0f, 50.0f), 0.0f, glm::vec3(1.0f));

		Texture2D playerTexture;
		Texture2D greenHillPlatform = ResourceManager::GetTexture("green-hill-platform");
		Texture2D marblePlatform = ResourceManager::GetTexture("marble-platform");
		Texture2D starlightPlatform = ResourceManager::GetTexture("starlight-platform");
		Texture2D finalPlatform = ResourceManager::GetTexture("final-platform");
		switch (timer % 4)
		{
		case 0:
			playerTexture = greenHillPlatform;
			break;
		case 1:
			playerTexture = marblePlatform;
			break;
		case 2:
			playerTexture = starlightPlatform;
			break;
		case 3:
			playerTexture = finalPlatform;
			break;
		}
		Renderer->DrawSprite(playerTexture, glm::vec2(740.0f, 400.0f), glm::vec2(100.0f, 50.0f), 0.0f, glm::vec3(1.0f));
		
		// Blocks
		Texture2D colorTexture;
		Texture2D metalTexture = ResourceManager::GetTexture("metal");
		switch (timer % 4)
		{
		case 0:
			colorTexture = ResourceManager::GetTexture("blue");
			break;
		case 1:
			colorTexture = ResourceManager::GetTexture("green");
			break;
		case 2:
			colorTexture = ResourceManager::GetTexture("yellow");
			break;
		case 3:
			colorTexture = ResourceManager::GetTexture("red");
			break;
		}
		Renderer->DrawSprite(colorTexture, glm::vec2(405.0f, 625.0f), glm::vec2(80.0f, 50.0f), 0.0f, glm::vec3(1.0f));
		Renderer->DrawSprite(metalTexture, glm::vec2(750.0f, 625.0f), glm::vec2(80.0f, 50.0f), 0.0f, glm::vec3(1.0f));
	}
}

bool Game::CheckCollision(GameObject &one, GameObject &two) // AABB
{
	// x-axis
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	// y-axis
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& one, GameObject& two) // AABB - Circle Collision
{
	glm::vec2 center(one.Position + one.Radius); // circle center point
	
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

	// get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	glm::vec2 closest = aabb_center + clamped; // add clamped value to aabb_center to get value of box closest to circle
	difference = closest - center; // get vector between center circle and closest point aabb

	if (glm::length(difference) < one.Radius) // check length <= radius
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions()
{
	for (GameObject &box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
				{
					box.Destroyed = true;
					this->SpawnPowerUps(box);
					boxPosition = box.Position;
					boxDestroyed = true;
					SoundEngine->play2D("res/audio/effects/collapse.mp3", false);
				}
				else
				{
					ShakeTime = 0.05f;
					Effects->Shake = true;
					SoundEngine->play2D("res/audio/effects/metal.wav", false);
				}

				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid))
				{
					if (dir == LEFT || dir == RIGHT) // horizontal collision
					{
						Ball->Velocity.x = -Ball->Velocity.x;
						// relocate
						float penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration;
						else
							Ball->Position.x -= penetration;
					}
					else // vertical collision
					{
						Ball->Velocity.y = -Ball->Velocity.y;
						// relocate
						float penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration;
						else
							Ball->Position.y += penetration;
					}
				}
			}
		}
	}
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// check where ball hit player
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		// move ball accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		Ball->Stuck = Ball->Sticky;
		SoundEngine->play2D("res/audio/effects/bong.mp3", false);
	}

	for (PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;
			if (CheckCollision(*Player, powerUp))
			{
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
				if (powerUp.Type == "speed")
					SoundEngine->play2D("res/audio/effects/emerald.mp3", false);
				else if (powerUp.Type == "confuse")
				{
					SoundEngine->play2D("res/audio/eggman/laser2.wav", false);
				}
				else if (powerUp.Type == "chaos")
				{
					SoundEngine->play2D("res/audio/eggman/laser.mp3", false);
					SoundEngine->play2D("res/audio/eggman/laughing-2.mp3", false);
				}
				else
					SoundEngine->play2D("res/audio/effects/rings.mp3", false);
			}
		}
	}
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f)
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("res/levels/one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("res/levels/two.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("res/levels/three.lvl", this->Width, this->Height / 2);
	else if (this->Level == 4)
		this->Levels[4].Load("res/levels/four.lvl", this->Width, this->Height / 2);

	this->Lives = 3;
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

bool Game::ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("speed", glm::vec2(40.0f, 31.0f), glm::vec3(1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	else if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec2(40.0f, 31.0f), glm::vec3(1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	else if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec2(40.0f, 31.0f), glm::vec3(1.0f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	else if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("increase", glm::vec2(40.0f, 31.0f), glm::vec3(1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	else if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("confuse", glm::vec2(40.0f, 31.0f), glm::vec3(1.0f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	else if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec2(60.0f, 20.0f), glm::vec3(1.0f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2f;
		super_sonic = true;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Confuse)
			Effects->Confuse = true;
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Chaos)
			Effects->Chaos = true;
	}
}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = false;
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky")) // Only reset if not already active
					{
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
						Effects->Confuse = false;
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
						Effects->Chaos = false;
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp){ return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

bool Game::IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}