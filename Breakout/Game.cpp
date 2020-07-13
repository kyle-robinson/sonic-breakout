#include "Game.h"
#include "ResourceManager.h"

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
	//ResourceManager::LoadShader("res/shaders/Basic.vs", "res/shaders/Basic.fs", nullptr, "sprite");
	ResourceManager::LoadShader("res/shaders/Basic.shader", "sprite");
}

void Game::ProcessInput(float dt)
{

}

void Game::Update(float dt)
{

}

void Game::Render()
{

}