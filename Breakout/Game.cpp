#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
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
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
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
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Update(float dt)
{
	if (Ball->Stuck)
		Ball->Position = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	else
	{
		Ball->Move(dt, this->Width);
		this->DoCollisions();
		if (Ball->Position.y >= this->Height)
		{
			this->ResetLevel();
			this->ResetPlayer();
		}
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Texture2D backgroundTexture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(backgroundTexture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
				
		this->Levels[this->Level].Draw(*Renderer);
		
		Player->Draw(*Renderer);

		Ball->Draw(*Renderer);
		
		//Texture2D myTexture;
		//myTexture = ResourceManager::GetTexture("slayer");
		//Renderer->DrawSprite(myTexture, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
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
					box.Destroyed = true;
				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
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
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}