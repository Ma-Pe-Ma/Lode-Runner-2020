#include "Drawing.h"
#include "Enemy.h"
#include "GameTime.h"

namespace Drawing {
	float block[16] = {
		-0.5,	-0.5,
		0.5,	-0.5,
		0.5, 	 0.5,
		-0.5,	 0.5,
	};

	float main_menu[16] = {
		-1,	-1,		0.0, 0.0,
		 1, -1,		1.0, 0.0,
		 1,	 1,		1.0, 1.0,
		-1,	 1,		0.0, 1.0,
	};

	const unsigned int indices[6] = {
		 0,1,2,
		 2,3,0
	};

	//from left to right, from down to up
	void playerTextureMapping(int index) {
		block[8] = ((index % 12) * 1.0) / 12;
		block[9] = ((index / 12) * 1.0) / 5;

		block[10] = ((index % 12 + 1) * 1.0) / 12;
		block[11] = ((index / 12) * 1.0) / 5;

		block[12] = ((index % 12 + 1) * 1.0) / 12;
		block[13] = ((index / 12 + 1) * 1.0) / 5;

		block[14] = ((index % 12) * 1.0) / 12;
		block[15] = ((index / 12 + 1) * 1.0) / 5;
	}

	//from up to down, from left to right
	void levelTextureMapping(int index) {
		block[8] = ((index / 6) * 1.0) / 11;
		block[9] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

		block[10] = ((index / 6 + 1) * 1.0) / 11;
		block[11] = 5.0 / 6 - ((index % 6) * 1.0) / 6;

		block[12] = ((index / 6 + 1) * 1.0) / 11;
		block[13] = 1.0 - ((index % 6) * 1.0) / 6;

		block[14] = ((index / 6) * 1.0) / 11;
		block[15] = 1.0 - ((index % 6) * 1.0) / 6;
	}

	void abcTextureMapping(char character) {
		std::string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		if (character == '.') {
			block[8] = float(0) / 16;
			block[9] = (3.0f) / 4;

			block[10] = float(1) / 16;
			block[11] = (3.0f) / 4;

			block[12] = float(1) / 16;
			block[13] = (4.0f) / 4;

			block[14] = float(0) / 16;
			block[15] = (4.0f) / 4;

			return;
		}

		if (character == ':') {
			block[8] = float(1) / 16;
			block[9] = (3.0f) / 4;

			block[10] = float(2) / 16;
			block[11] = (3.0f) / 4;

			block[12] = float(2) / 16;
			block[13] = (4.0f) / 4;

			block[14] = float(1) / 16;
			block[15] = (4.0f) / 4;

			return;
		}

		for (int j = 0; j < 10; j++) {
			if (j == (int(character) - 48)) {
				block[8] = (1.0 * j + 0.01) / 16;
				block[9] = (1.0) / 2;

				block[10] = (j + 1.0) / 16;
				block[11] = (1.0) / 2;

				block[12] = (j + 1.0) / 16;
				block[13] = (3.0) / 4;

				block[14] = (1.0 * j + 0.01) / 16;
				block[15] = (3.0) / 4;
			}
		}

		for (int i = 0; i < ABC.length(); i++) {
			if (character == ABC[i]) {
				if (i < 15) {
					block[8] = (float(1.01 + i)) / 16;
					block[9] = (float(1)) / 4;

					block[10] = (float(2 + i)) / 16;
					block[11] = (float(1)) / 4;

					block[12] = (float(2 + i)) / 16;
					block[13] = (float(2)) / 4;

					block[14] = (float(1.01 + i)) / 16;
					block[15] = (float(2)) / 4;
					return;
				}

				if (i > 14) {
					block[8] = float(i - 14.99) / 16;
					block[9] = (0.0) / 4;

					block[10] = float(i - 14) / 16;
					block[11] = (0.0) / 4;

					block[12] = float(i - 14) / 16;
					block[13] = (1.0) / 4;

					block[14] = float(i - 14.99) / 16;
					block[15] = (1.0) / 4;
					return;
				}
			}
		}
	}

	void textWriting(std::string text, float i, float j) {
		for (int k = 0; k < text.length(); k++) {
			if (i == 32) {
				i = 0;
				j++;
			}

			if (text.at(k) != ' ') {
				abcTextureMapping(text.at(k));
				GLHelper::selectShader->use();
				GLHelper::selectShader->setVec2("gPos", i, j);
				GLHelper::selectShader->setInt("textureA", 3);

				glBindVertexArray(GLHelper::selectVAO);
				glBindBuffer(GL_ARRAY_BUFFER, GLHelper::selectVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(block), block, GL_STATIC_DRAW);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			i++;
		}
	}

	/*void drawLevel(float x, float y, int ref) {
		levelTextureMapping(ref);

		GLHelper::levelShader->use();
		glBindVertexArray(GLHelper::levelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, GLHelper::levelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(block), block, GL_STATIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, (void*) &block[8]);
		GLHelper::levelShader->setVec2("gPos", x, y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}*/

	void drawEnemy(float x, float y, int ref, Direction direction, bool gold) {
		bool dirToShader = true;

		if (direction == Direction::right) {
			dirToShader = false;
		}

		playerTextureMapping(ref);

		GLHelper::playerShader->use();
		glBindVertexArray(GLHelper::playerVAO);
		glBindBuffer(GL_ARRAY_BUFFER, GLHelper::playerVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(block), block, GL_STATIC_DRAW);
		GLHelper::playerShader->setVec2("gPos", x, y);
		GLHelper::playerShader->setBool("direction", dirToShader);
		GLHelper::playerShader->setBool("carryGold", gold);
		GLHelper::playerShader->setFloat("ref", block[8]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}