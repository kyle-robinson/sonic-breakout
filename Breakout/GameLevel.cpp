#include "GameLevel.h"

#include <fstream>
#include <sstream>

void GameLevel::Load(const std::string &file, unsigned int levelWidth, unsigned int levelHeight)
{
	this->Bricks.clear();

	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;

	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode)
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->Init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
	for (GameObject& tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
	for (GameObject& tile : this->Bricks)
		if (!tile.IsSolid && !tile.Destroyed)
			return false;
	return true;
}

void GameLevel::Init(std::vector<std::vector<unsigned int>> tileData, unsigned int lvlWidth, unsigned int lvlHeight)
{
	// dimensions
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unit_width = lvlWidth / static_cast<float>(width);
	float unit_height = lvlHeight / height;

	// initialise tiles
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			// check block type
			if (tileData[y][x] == 1) // solid
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				//GameObject obj(pos, size, ResourceManager::GetTexture("obsidian"), glm::vec3(1.0f, 1.0f, 1.0f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1) // not solid
			{
				glm::vec3 color = glm::vec3(1.0f);
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				/*Texture2D blockTexture;
				if (tileData[y][x] == 2)
					blockTexture = ResourceManager::GetTexture("dirt");
				else if (tileData[y][x] == 3)
					blockTexture = ResourceManager::GetTexture("stone");
				else if (tileData[y][x] == 4)
					blockTexture = ResourceManager::GetTexture("silver");
				else if (tileData[y][x] == 5)
					blockTexture = ResourceManager::GetTexture("gold");*/

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
				//this->Bricks.push_back(GameObject(pos, size, blockTexture, color));
			}
		}
	}
}