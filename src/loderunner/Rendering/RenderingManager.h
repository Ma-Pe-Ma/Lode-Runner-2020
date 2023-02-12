#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include <Shader.h>
#include <memory>
#include <vector>

#include "../Brick.h"
#include "../Trapdoor.h"

class RenderingManager {
	float levelVertices[16] = {
		-1.0f,			-1.0f,
		-14.0f / 15,	-1.0f,
		-14.0f / 15,	-8.0f / 9,
		-1.0f,			-8.0f / 9,

		0.0f,			5.0f / 6,
		1.0f / 11,		5.0f / 6,
		1.0f / 11,		1.0f,
		0.0f,			1.0f
	};

	unsigned int indices[6] = {
		0,1,2,
		0,2,3
	};

	unsigned int VAO, VBO, EBO;

	Shader* shader;
	int* textureIDs = nullptr;
	int* drawables = nullptr;
	int drawableSize = 0;
	int currentDrawableSize = 0;

	std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList;
	std::vector<std::shared_ptr<Trapdoor>> trapdoorList;
	std::vector<std::tuple<int, int>> poleList;
	std::vector<std::tuple<int, int>> concreteList;
	std::vector<std::tuple<int, int>> ladderList;
	std::vector<std::tuple<int, int>> finishingLadderList;

	int ladderStartIndex = 0;
	int ladderSize = 0;
	int finishingLadderSize = 0;

public:
	RenderingManager() {
		shader = new Shader("Assets/Shaders/level2.vs", "Assets/Shaders/level2.fs");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(levelVertices), levelVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0 * sizeof(float)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	void setBrickList(std::shared_ptr<std::vector<std::shared_ptr<Brick>>> brickList)
	{
		this->brickList = brickList;
	}

	void setPoleList(std::vector<std::tuple<int, int>> poleList)
	{
		this->poleList = poleList;
	}

	void setConcreteList(std::vector<std::tuple<int, int>> concreteList)
	{
		this->concreteList = concreteList;
	}

	void setTrapdoorList(std::vector<std::shared_ptr<Trapdoor>> trapdoorList)
	{
		this->trapdoorList = trapdoorList;
	}

	void setLadderList(std::vector<std::tuple<int, int>> ladderList)
	{
		this->ladderList = ladderList;
	}

	void setFinishingLadderList(std::vector<std::tuple<int, int>> finishingLadderList)
	{
		this->finishingLadderList = finishingLadderList;
	}

	void initialize()
	{
		drawableSize = 1 + poleList.size() + concreteList.size() + brickList->size() + trapdoorList.size() + ladderList.size() + finishingLadderList.size();
		currentDrawableSize = drawableSize - finishingLadderList.size();
		drawables = new int[drawableSize * 2];
		textureIDs = new int[drawableSize];

		Brick::setPointerToDebrisTexture(&textureIDs[0]);
		Brick::setPointerToDebrisLocation(&drawables[0]);

		for (auto iterator = brickList->begin(); iterator != brickList->end(); iterator++)
		{
			int index = iterator - brickList->begin() + 1;

			drawables[2 * index + 0] = (*iterator)->getPosition().x;
			drawables[2 * index + 1] = (*iterator)->getPosition().y;
		
			(*iterator)->setTexturePointer(&textureIDs[index]);
		}

		std::fill_n(textureIDs + 1, brickList->size(), 0);

		for (auto iterator = poleList.begin(); iterator != poleList.end(); iterator++)
		{
			int index = iterator - poleList.begin() + brickList->size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + brickList->size() + 1, poleList.size(), 18);

		for (auto iterator = concreteList.begin(); iterator != concreteList.end(); iterator++)
		{
			int index = iterator - concreteList.begin() + brickList->size() + poleList.size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + brickList->size() + poleList.size() + 1, concreteList.size(), 6);

		for (auto iterator = trapdoorList.begin(); iterator != trapdoorList.end(); iterator++)
		{
			int index = iterator - trapdoorList.begin() + brickList->size() + poleList.size() + concreteList.size() + 1;

			drawables[2 * index + 0] = (*iterator)->getPos().x;
			drawables[2 * index + 1] = (*iterator)->getPos().y;
			
			(*iterator)->setTexturePointer(&textureIDs[index]);
		}

		std::fill_n(textureIDs + brickList->size() + poleList.size() + concreteList.size() + 1, trapdoorList.size(), 0);

		ladderStartIndex = brickList->size() + poleList.size() + concreteList.size() + trapdoorList.size() + 1;
		ladderSize = ladderList.size();

		for (auto iterator = ladderList.begin(); iterator != ladderList.end(); iterator++)
		{
			int index = iterator - ladderList.begin() + brickList->size() + poleList.size() + concreteList.size() + trapdoorList.size() + 1;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		std::fill_n(textureIDs + ladderStartIndex, ladderSize, 12);

		for (auto iterator = finishingLadderList.begin(); iterator != finishingLadderList.end(); iterator++)
		{
			int index = iterator - finishingLadderList.begin() + ladderStartIndex + ladderSize;

			drawables[2 * index + 0] = std::get<0>(*iterator);
			drawables[2 * index + 1] = std::get<1>(*iterator);
		}

		finishingLadderSize = finishingLadderList.size();

		brickList = nullptr;
		poleList.clear();
		concreteList.clear();
		trapdoorList.clear();
		ladderList.clear();
		finishingLadderList.clear();
	}

	void setLadderFlashFactor(int factor)
	{
		std::fill_n(textureIDs + ladderStartIndex, ladderSize, 12 + factor);
	}

	void enableFinishingLadderDrawing()
	{
		currentDrawableSize = drawableSize;
		ladderSize += finishingLadderSize;
	}

	void render()
	{
		shader->use();
		glBindVertexArray(VAO);
		shader->setInt("textureA", 1);
		shader->setIntArray("textureID", textureIDs, currentDrawableSize);
		shader->setInt2Array("gPos", drawables, currentDrawableSize);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, currentDrawableSize);
	}

	void clearRenderableObjects()
	{
		if (drawables != nullptr)
		{
			delete[] drawables;
			drawables = nullptr;
		}

		if (textureIDs != nullptr)
		{
			delete[] textureIDs;
			textureIDs = nullptr;
		}
	}
};

#endif