
void mainMenu(float currentFrame) {
	//main menu music cyclically playing!
	if (Audio::SFX[5].GetPlayStatus() == stopped)		
		Audio::SFX[5].PlayPause();
			
	//Drawing main menu
	if (championShip)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	else
		glClearColor(110.0 / 256, 93.0 / 256, 243.0 / 256, 1.0f);

	//drawing background
	glClear(GL_COLOR_BUFFER_BIT);
	mainShader->use();
	mainShader->setInt("mode", 0);
	mainShader->setInt("textureA", 2);
	glBindVertexArray(mainVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//drawing cursor
	if (!championShip) {

		glBindVertexArray(cursorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
		float cursorY = (1.0f - 2 * menuCursor) / 14;
		float cursorLocation[] = { -3.0f / 14, cursorY - 1.0f / 14, -6.0f / 35, cursorY - 1.0f / 14, -6.0f / 35, cursorY, -3.0f / 14, cursorY };
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, cursorLocation);

		mainShader->setInt("mode", 1);
		mainShader->setInt("textureA", 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//changing gamemode
	if (space.Simple() || down.Simple())
		if (++menuCursor == 3)
			menuCursor = 0;

	if (up.Simple())
		if (--menuCursor == -1)
			menuCursor = 2;

	if (championShip)
		menuCursor = 0;

	//choosing selected gamemode
	if (enter.Simple()) {
		
		Audio::SFX[5].StopAndRewind();

		switch (menuCursor) {
		
			//single player
			case 0:
				Audio::SFX[8].PlayPause();
				introTimer = currentFrame;
				menu = L02;
				break;

			//multiplayer
			case 1:
				Audio::SFX[8].PlayPause();
				menu = L02;
				introTimer = currentFrame;
				break;

			//level generator
			case 2:

				menu = L08;

				geX = 1;
				geY = 16;

				for (int i = 0; i < 30; i++)
					for (int j = 0; j < 17; j++) {
						if (j == 0)
							gen[i][j] = 2;
						else if (i == 0 || i == 29)
							gen[i][j] = 2;
						else
							gen[i][j] = 0;
					}
				break;
			}
	}
}

void introScreen(float currentFrame) {
	
	if (Audio::SFX[8].GetPlayStatus() == playing) {
	
		if (space.Simple()) {
			menu = L03;
			Audio::SFX[8].StopAndRewind();
		}

		if (enter.Simple())
			Audio::SFX[8].StopAndRewind();

		std::string number = "00" + std::to_string(level[playerNr]);
		
		if (level[playerNr] > 9)
			number = '0' + std::to_string(level[playerNr]);

		if (level[playerNr] > 99)
			number = std::to_string(level[playerNr]);

		std::string levelName = "STAGE " + number;
		std::string playerName = "PLAYER " + std::to_string(playerNr + 1);
		std::string lifeLeft = "LEFT " + std::to_string(playerLife[playerNr]);

		std::string pont = std::to_string(score[playerNr]);
		std::string zeros = "";

		for (int i = 0; i < 8 - pont.length(); i++)
			zeros = zeros + '0';

		std::string scoret = "SCORE    " + zeros + pont;

		std::string record = std::to_string(highScore);
		zeros = "";

		for (int i = 0; i < 8 - record.length(); i++)
			zeros = zeros + '0';

		std::string hiscore = "HISCORE  " + zeros + record;

		TextWriting(levelName, 8, 12);
		if (!championShip)
			TextWriting(playerName, 12, 6);

		TextWriting(lifeLeft, 19, 12);
		TextWriting(scoret, 8, 18);
		TextWriting(hiscore, 8, 20);
	}
	else {

		startingScreen = true;
		menu = L04;
		Enemy::enemyNr = 1;
		Gold::goldNr = 0;
		Gold::remainingGoldCount = 0;

		Audio::SFX[7].StopAndRewind();

		score_gold = 0;
		score_enemy = 0;

		LevelLoading(level[playerNr]);

		for (int i = 0; i < Enemy::enemyNr; i++) {

			layoutBlock blockUnder = layout[int(enemies[i].Pos.x)][int(enemies[i].Pos.y + 0.5) - 1];

			if ((blockUnder == empty || blockUnder == trapDoor || blockUnder == pole) && !Enemy::EnemyCheckerGlobal(enemies[i].Pos.x, enemies[i].Pos.y - 1)) {

				enemies[i].state = falling;
				if(i==0)
					enemies[i].TextureRef = 52;
				else
					enemies[i].TextureRef = 16;
			}
				
			else {
				enemies[i].state = freeRun;
				
				if (i == 0)
					enemies[i].TextureRef = 48;
				else
					enemies[i].TextureRef = 12;
			}
				
		}

		playerShader->setBool("direction", true);
		curSessionStartTime = currentFrame;
		prevSessionSum = 0;
		startingTimer = currentFrame;
	}
}

void selectScreen(float currentFrame) {
	
	if (leftButton.Simple()) {

		level[playerNr]--;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (rightButton.Simple()) {

		level[playerNr]++;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (up.Simple()) {

		level[playerNr] += 10;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}

	if (down.Simple()) {

		level[playerNr] -= 10;
		Audio::SFX[16].StopAndRewind();
		Audio::SFX[16].PlayPause();
	}


	if (championShip) {
		if (level[playerNr] < 1)
			level[playerNr] = 51 + level[playerNr];

		if (level[playerNr] > 51)
			level[playerNr] = level[playerNr] % 10;
	}
	else {

		if (level[playerNr] == 160)
			level[playerNr] = 10;

		if (level[playerNr] > 150)
			level[playerNr] = level[playerNr] % 10;
		
		if (level[playerNr] < 1)
			level[playerNr] = 150 + level[playerNr];
	}

	if (enter.Simple()) {
		menu = L02;
		introTimer = currentFrame;
		Audio::SFX[8].StopAndRewind();
		Audio::SFX[8].PlayPause();
	}

	std::string levelNumber;

	if (level[playerNr] > 99)
		levelNumber = std::to_string(level[playerNr]);

	if (level[playerNr] > 9 && level[playerNr] < 100)
		levelNumber = '0' + std::to_string(level[playerNr]);

	if (level[playerNr] < 10)
		levelNumber = "00" + std::to_string(level[playerNr]);

	std::string levelName = "STAGE " + levelNumber;
	TextWriting(levelName, 8, 12);
}

void gameScreen(float currentFrame) {

	//play gamplay music cyclically
	if (Audio::SFX[7].GetPlayStatus() != playing && enemies[0].state != dying)
		Audio::SFX[7].PlayPause();

	bool takePauseScreenshot = false;

	gameTime = prevSessionSum + (currentFrame - curSessionStartTime);

	//pause
	if (enter.Simple() && enemies[0].state != dying) {

		takePauseScreenshot = true;

		menu = L06;
		
		Audio::SFX[7].PlayPause();

		if (Audio::SFX[4].GetPlayStatus() == playing)
			Audio::SFX[4].PlayPause();

		if (Audio::SFX[1].GetPlayStatus() == playing)
			Audio::SFX[1].PlayPause();

		if (Audio::SFX[17].GetPlayStatus() == playing)
			Audio::SFX[17].PlayPause();

		Audio::SFX[14].PlayPause();

		prevSessionSum = gameTime;
	}

	//if every gold is collected draw the ladders which are needed to finish the level
	if (Gold::remainingGoldCount == 0) {
		
		Audio::SFX[4].PlayPause();
		highestLadder = 0;

		if (menuCursor < 2)
			HighestLadder();
		else
			HighestLadderGen();

		Gold::remainingGoldCount = -1;
	}
	
	//drawing level, digging and rebuilding brick, death by rebuilt brick
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 18; j++) {

			//if layout is empty do nothing
			if (layout[i][j] == empty);
			//do not draw brick if it's being dug
			else if (holeTimer[i][j] > 0);
			//animating flashing ladder
			else if (layout[i][j] == ladder) {
				int timeFactor= int(gameTime) % 4;
				if (timeFactor == 3)
					timeFactor = 1;
					DrawLevel(i, j, 12 + timeFactor, 0);
			}
			//draw every other item simply
			else if (layout[i][j] == brick)
				DrawLevel(i, j, 0, 0);
			else if(layout[i][j] == trapDoor)
				DrawLevel(i, j, 0, holeTimer[i][j]);
			else if (layout[i][j] == concrete)
				DrawLevel(i, j, 6, 0);
			else if (layout[i][j] == pole)
				DrawLevel(i, j, 18, 0);

			//animating brick which is being destroyed
			if (gameTime < holeTimer[i][j] + destroyTime && holeTimer[i][j] > 0) {

				int timeFactor = int(5 * (gameTime - holeTimer[i][j]) / destroyTime) % 5;

				//growing hole
				DrawLevel(i, j, 1 + timeFactor, 0);

				//drawing debris above hole
				int debris = 19 + randomDebris * 6;

				if (!(layout[i][j + 1] == brick || layout[i][j + 1] == concrete || Enemy::EnemyCheckerGlobal(i, j + 1)))
					DrawLevel(i, j + 1, debris + timeFactor, 0);				
				
				if (timeFactor == 4)
					layout[i][j] = empty;

				//digging is prevented if enemy is close to the brick
				for (int l = 1; l < Enemy::enemyNr; l++) {
					if (abs(enemies[l].Pos.x - i) <= 0.75 && enemies[l].Pos.y >= j + 1 && enemies[l].Pos.y < j + 1.5) {
						
						Audio::SFX[2].PlayPause();
						Audio::SFX[1].StopAndRewind();
						
						holeTimer[i][j] = 0;
						layout[i][j] = brick;
						digTime = gameTime - 2;
						enemies[0].state = freeRun;
						left_pit = false;
						right_pit = false;
					}
				}
			}

			//rebuilding brick
			if (holeTimer[i][j] + diggingTime - buildTime < gameTime && gameTime < holeTimer[i][j] + diggingTime && holeTimer[i][j] > 0) {
				int timeFactor = int(5 * (gameTime - diggingTime + buildTime - holeTimer[i][j]) / buildTime) % 5;
				DrawLevel(i, j, 11 - timeFactor,0);
			}

			//brick is rebuilt
			if (gameTime > holeTimer[i][j] + diggingTime && holeTimer[i][j] > 0) {

				layout[i][j] = brick;
				holeTimer[i][j] = 0;

				DrawLevel(i, j, 0, 0);

				//brick rebuilt player dies
				if (abs(enemies[0].Pos.x - i) < 0.5 && int(enemies[0].Pos.y + 0.5) == j && enemies[0].state != dying) {

					enemies[0].state = dying;
					enemies[0].dieTimer = currentFrame;
					Audio::SFX[17].StopAndRewind();
					Audio::SFX[7].StopAndRewind();
					Audio::SFX[3].PlayPause();
				}

				/*//useless condition as enemies can't drop gold at hole which they entered from side!
				if (int checkGold = Enemy::GoldChecker(i, j) != -1) {
					
					gold[checkGold].Pos.x = -2;
					gold[checkGold].Pos.y = -2;
					Gold::remainingGoldCount--;
				}*/

				//brick rebuilt but enemy got into the brick from the side
				for (int m = 1; m < Enemy::enemyNr; m++)
					if (abs(enemies[m].Pos.x - i) < 0.5 && abs(enemies[m].Pos.y - j) < 0.5 && enemies[m].state != pitting) {

						score_enemy += 300;

						if (enemies[m].goldVariable >= 0) {

							enemies[m].carriedGold->Pos = { -2, -2 };
							enemies[m].goldVariable = -1;
							enemies[m].carriedGold = nullptr;

							Gold::remainingGoldCount--;
						}

						enemies[m].pitState = out;
						enemies[m].state = dying;

						enemies[m].Pos.x = -1;
						enemies[m].Pos.y = -1;

						enemies[m].dPos.x = 0;
						enemies[m].dPos.y = 0;

						enemies[m].holePos.x = -1;
						enemies[m].holePos.y = -1;					
					}
			}
		}

	//Gold detection
	for (int i = 0; i < Enemy::enemyNr; i++) {
		int checkGold = Enemy::GoldChecker(enemies[i].Pos.x, enemies[i].Pos.y);
		if (checkGold >= 0) {

			if (i == 0) {
				if (Audio::SFX[0].GetPlayStatus() == playing)
					Audio::SFX[0].StopAndRewind();
					
				Audio::SFX[0].PlayPause();

				score_gold += 200;
				gold[checkGold].Pos.x = -2;
				gold[checkGold].Pos.y = -2;
				Gold::remainingGoldCount--;
			}
			else {
				if (enemies[i].carriedGold == nullptr) {

					enemies[i].carriedGold = &gold[checkGold];

					gold[checkGold].Pos.x = -1;
					gold[checkGold].Pos.y = -2;

					enemies[i].goldVariable = rand() % 26 + 14;
				}
			}	
		}
	}

	//enemy pathfinding
	if (enemies[0].state != dying)
		for (int i = 1; i < Enemy::enemyNr; i++) {
			if (enemies[i].state != dying)
				enemies[i].PathFinding();
		}
	
	//check if runner dies by enemy
	if(enemies[0].state != dying && enemies[0].state != falling)
		for (int i = 1; i < Enemy::enemyNr; i++) {
			if (abs(enemies[i].Pos.x - enemies[0].Pos.x) < 0.5 && abs(enemies[i].Pos.y - enemies[0].Pos.y) < 0.5) {
				
				enemies[0].state = dying;
				enemies[0].dieTimer = currentFrame;

				Audio::SFX[3].PlayPause();
				Audio::SFX[7].StopAndRewind();
				Audio::SFX[17].StopAndRewind();
			}
		}

	//Gold drawing
	for (int i = 0; i < Gold::goldNr; i++)
		if (gold[i].Pos.x > 0) {
			
			int timeFactor = int(gameTime) % 4;

			if (timeFactor == 3)
				timeFactor = 1;

			DrawLevel(gold[i].Pos.x, gold[i].Pos.y, 36 + timeFactor, 0);
		}

	//player input
	if (rightButton.continous() && leftButton.continous())
		enemies[0].dPos.x = 0;
	else if (rightButton.continous())
		enemies[0].dPos.x = playerSpeed * speed;
	else if (leftButton.continous())
		enemies[0].dPos.x = -playerSpeed * speed;

	if (up.continous() && down.continous())
		enemies[0].dPos.y = 0;
	else if (up.continous())
		enemies[0].dPos.y = playerSpeed * speed;
	else if (down.continous())
		enemies[0].dPos.y = -playerSpeed * speed;
	
	//moving runner and enemies by the input
	if (!startingScreen)
		for (int i = 0; i < Enemy::enemyNr; i++)
			if (enemies[0].state != dying) {
				
				enemies[i].Move();
				
				enemies[i].Pos.x += enemies[i].dPos.x;
				enemies[i].Pos.y += enemies[i].dPos.y;
				
				enemies[i].dPos.x = 0;
				enemies[i].dPos.y = 0;

				//dropping gold
				if (i != 0) {

					//Determining when to drop gold
					int curX = int(enemies[i].Pos.x + 0.5);
					int curY = int(enemies[i].Pos.y + 0.5);

					int prevX = int(enemies[i].prevPos.x + 0.5);
					int prevY = int(enemies[i].prevPos.y + 0.5);

					if (enemies[i].goldVariable > 0)
						if (curX != prevX || curY != prevY)
							enemies[i].goldVariable--;

					int checkGold = Enemy::GoldChecker(prevX, prevY);

					layoutBlock prevBlock = layout[prevX][prevY];
					layoutBlock prevBlockUnder = layout[prevX][prevY - 1];
					float prevHole = holeTimer[prevX][prevY];

					if (enemies[i].goldVariable == 0)
						if (prevBlock == empty && prevHole == 0 && checkGold < 0 && (prevBlockUnder == brick || prevBlockUnder == concrete || prevBlockUnder == ladder) && enemies[i].state == freeRun && (prevX != curX || prevY != curY)) {

							enemies[i].carriedGold->Pos = { (float)prevX, (float)prevY };
							enemies[i].goldVariable = -1;
							enemies[i].carriedGold = nullptr;
						}	
				}
			}

	//player dying
	if (enemies[0].state == dying) {
		if (Audio::SFX[3].GetPlayStatus() == playing) {
			
			Audio::SFX[17].StopAndRewind();
			double deathLength = Audio::SFX[3].LengthInSec();

			int timeFactor = int(9 * (currentFrame - enemies[0].dieTimer) / (deathLength + 0.01)) % 9;
			if (timeFactor == 8)
				timeFactor = 31;

			enemies[0].TextureRef = 28 + timeFactor;
		}
		else {
			Enemy::enemyNr = 1;
			Gold::goldNr = 0;
			if (menuCursor < 2) {
				if (playerLife[playerNr] - 1 == 0) {
					game_over[playerNr] = true;
					game_overTimer = currentFrame;
					menu = L07;
					Audio::SFX[6].PlayPause();
				}

				else {
					Audio::SFX[8].PlayPause();
					introTimer = currentFrame;
					menu = L02;
					playerLife[playerNr]--;

					if (menuCursor == 1)
						playerNr = 1 - playerNr;
				}
			}
			else
				menu = L08;
		}
	}

	//animation of characters
	for (int i = 0; i < Enemy::enemyNr; i++) {
		if (enemies[0].state == dying && i != 0)
			enemies[i].state = freeRun;

		if(!startingScreen)
			enemies[i].Animation();

		DrawEnemy(enemies[i].Pos.x, enemies[i].Pos.y, enemies[i].TextureRef, enemies[i].direction, enemies[i].carriedGold);

		enemies[i].dPrevPos.x = enemies[i].Pos.x - enemies[i].prevPos.x;
		enemies[i].dPrevPos.y = enemies[i].Pos.y - enemies[i].prevPos.y;

		enemies[i].prevPos.x = enemies[i].Pos.x;
		enemies[i].prevPos.y = enemies[i].Pos.y;
	}

	//end of level, the top of level reached
	if ((int(enemies[0].Pos.y + 0.5) >= highestLadder + 1 && Gold::remainingGoldCount == -1 && menuCursor < 2) || (menuCursor == 2 && int(enemies[0].Pos.y + 0.5) >= 17)) {
		menu = L05;
		outroTimer = currentFrame;

		Audio::SFX[7].StopAndRewind();
		Audio::SFX[4].StopAndRewind();
		Audio::SFX[13].PlayPause();

		if (playerLife[playerNr] < 9)
			playerLife[playerNr]++;

		enemies[0].Pos.y = 0;
	}

	if (takePauseScreenshot) {
		glActiveTexture(GL_TEXTURE4);

		unsigned char *screenBuffer = new unsigned char[viewPortWidth * viewPortHeight * 3];

		glReadPixels(viewPortX, viewPortY, viewPortWidth, viewPortHeight, GL_RGB, GL_UNSIGNED_BYTE, screenBuffer);

		glBindTexture(GL_TEXTURE_2D, pauseScreenT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewPortWidth, viewPortHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, screenBuffer);

		glGenerateMipmap(GL_TEXTURE_2D);
		float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		delete[] screenBuffer;
	}

	if (startingScreen) {
	
		gameTime = 0;
		curSessionStartTime = currentFrame;
		prevSessionSum = 0;

		if (currentFrame > startingTimer + 2) {

			if (enemies[0].state == falling)
				Audio::SFX[17].PlayPause();
			
			startingScreen = false;
		}
	}

	//levelselect with space
	if (space.Simple() && enemies[0].state != dying) {

		menu = L03;

		if (menuCursor >= 2)
			menu = L08;

		Audio::SFX[17].StopAndRewind();
		Audio::SFX[4].StopAndRewind();
		Audio::SFX[7].StopAndRewind();
		return;
	}

}

void pauseScreen(float currentFrame) {
	if (Audio::SFX[14].GetPlayStatus() != playing) {
		if (enter.Simple()) {
			
			Audio::SFX[14].PlayPause();
			menu = C06;
		}

		if (space.Simple()) {
			if (menuCursor < 2)
				menu = L03;
			else
				menu = L08;
		}
	}

	glBindVertexArray(mainVAO);
	mainShader->use();
	mainShader->setInt("textureA", 4);
	mainShader->setInt("mode", 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void pauseScreenOut(float currentFrame) {
	if (Audio::SFX[14].GetPlayStatus() != playing) {
		
		//continue playing digging sound
		if (Audio::SFX[1].GetPlayStatus() == paused)
			Audio::SFX[1].PlayPause();
		
		//if unpaused continue playing the melody for collecting every gold
		if (Audio::SFX[4].GetPlayStatus() == paused)
			Audio::SFX[4].PlayPause();

		//if unpaused continue playing the SFX for the falling of player
		if (Audio::SFX[17].GetPlayStatus() == paused)
			Audio::SFX[17].PlayPause();
		
		menu = L04;

		curSessionStartTime = currentFrame;
	}
	
	glBindVertexArray(mainVAO);
	mainShader->use();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void outroScreen(float currentFrame) {
	if (Audio::SFX[13].GetPlayStatus() == playing) {

		if (enter.Simple())
			Audio::SFX[13].StopAndRewind();

		std::string gold_points = std::to_string(score_gold) + " POINTS";
		std::string enemy_points = std::to_string(score_enemy) + " POINTS";
		std::string total = "TOTAL " + std::to_string(score_enemy + score_gold) + " POINTS";

		TextWriting(gold_points, 20, 6);
		TextWriting(enemy_points, 20, 12);
		TextWriting(total, 14.5, 23.25);

		//draw enemy for score indicator
		DrawEnemy(11, 9.5, 12, left, nullptr);
		
		//draw Gold for score indicator
		int ido = int(2 * currentFrame) % 4;
		if (ido == 3)
			ido = 1;

		DrawLevel(11, 13.25, 36 + ido, 0);

		for (int i = 7; i < 13; i++)
			for (int k = 0; k < 3; k++) {
				
				//drawing ladder
				if (i == 9)
					DrawLevel(i, k, 12 + ido,0);
				//drawing bricks
				if (k == 2)
					DrawLevel(i, k, 0,0);
			}

		//runner climbs ladder
		if (int(enemies[0].Pos.y + speed) != 3) {
			
			int timeFactor = ((currentFrame - outroTimer) * 4);
			
			enemies[0].Pos.y += speed * 0.1;
			enemies[0].TextureRef = 36 + timeFactor % 4;
			
			DrawEnemy(9, enemies[0].Pos.y, enemies[0].TextureRef, left, nullptr);
		}
		//nail bitting after reaching top of ladder
		else {
			int timeFactor = int((currentFrame - outroTimer) * 3) % 4;
			DrawEnemy(9, 3, 44 + timeFactor, left, nullptr);
		}
	}
	else {
		
		Audio::SFX[7].StopAndRewind();

		if (menuCursor < 2) {
			menu = L02;

			level[playerNr]++;
			if (level[playerNr] > 150)
				level[playerNr] = 1;

			introTimer = currentFrame;

			score[playerNr] += score_enemy + score_gold;
			if (score[playerNr] > highScore)
				highScore = score[playerNr];

			score_enemy = 0;
			score_gold = 0;

			Enemy::enemyNr = 1;
			Gold::goldNr = 0;

			Audio::SFX[8].PlayPause();
		}
		else
			menu = L08;
	}
}

void gameOverScreen(float currentFrame) {
	
	if (Audio::SFX[6].GetPlayStatus() == playing) {
		std::string gameOverText = "GAME OVER";
		std::string playerName = "PLAYER " + std::to_string(playerNr + 1);
		TextWriting(playerName, 12.5, 6);
		TextWriting(gameOverText, 12, 10);
	}
	else {
		if (menuCursor == 0) {
			menu = L01;
			playerLife[0] = 5;
			menuCursor = 0;
			game_over[playerNr] = false;
			score[playerNr] = 0;
			level[playerNr] = 1;
		}

		if (menuCursor == 1) {
			if (!game_over[1 - playerNr]) {
				level[playerNr] = 1;
				playerNr = 1 - playerNr;
				Audio::SFX[8].PlayPause();
				menu = L02;
				introTimer = currentFrame;
			}
			else {

				playerLife[0] = 5;
				playerLife[1] = 5;
				menuCursor = 0;
				game_over[0] = false;
				game_over[1] = false;

				score[0] = 0;
				score[1] = 0;

				level[0] = 1;
				level[1] = 1;
				playerNr = 0;
			}
		}
	}
}

void generatorScreen(float currentFrame) {
	if (rightButton.Simple())
		if (++geX > 28)
			geX = 28;
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}

	if (leftButton.Simple())
		if (--geX < 1)
			geX = 1;
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}

	if (up.Simple())
		if (++geY > 16)
			geY = 16;
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}
			

	if (down.Simple())
		if (--geY < 1)
			geY = 1;
		else {
			Audio::SFX[9].StopAndRewind();
			Audio::SFX[9].PlayPause();
		}

	//chaning element upwards
	if (rightDigButton.Simple()) {

		if (++gen[geX][geY] > 9)
			gen[geX][geY] = 0;

		Audio::SFX[10].StopAndRewind();
		Audio::SFX[10].PlayPause();
	}			

	//chaning element downwards
	if (leftDigButton.Simple()) {
		if (--gen[geX][geY] < 0)
			gen[geX][geY] = 9;

		Audio::SFX[10].StopAndRewind();
		Audio::SFX[10].PlayPause();
	}			

	//yellow cursor block
	if (int(3 * currentFrame) % 2)
		DrawLevel(geX, geY, 54,0);
	
	//drawing generator elements
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 17; j++) {
			int ref = -1;
			if (gen[i][j] == 1)
				ref = 0;
			if (gen[i][j] == 2)
				ref = 6;
			if (gen[i][j] == 3)
				ref = 12;
			if (gen[i][j] == 4)
				ref = 18;
			if (gen[i][j] == 5)
				ref = 24;
			if (gen[i][j] == 6)
				ref = 30;
			if (gen[i][j] == 7)
				ref = 36;
			if (gen[i][j] == 8)
				ref = 42;
			if (gen[i][j] == 9)
				ref = 48;

			if (ref >= 0)
				DrawLevel(i, j, ref,0);
		}

	if (enter.Simple()) {

		gameTime = 0;
		curSessionStartTime = currentFrame;
		prevSessionSum = 0;

		Enemy::enemyNr = 1;
		Gold::goldNr = 0;
		enemies[0].Pos.x = -1;
		enemies[0].Pos.y = -1;

		for (int i = 0; i < 30; i++)
			for (int j = 0; j < 17; j++) {
				holeTimer[i][j] = 0;

				if (gen[i][j] == 0)
					layout[i][j] = empty;
				if (gen[i][j] == 1)
					layout[i][j] = brick;
				if (gen[i][j] == 2)
					layout[i][j] = concrete;
				if (gen[i][j] == 3)
					layout[i][j] = ladder;
				if (gen[i][j] == 4)
					layout[i][j] = pole;
				if (gen[i][j] == 5)
					layout[i][j] = trapDoor;
				if (gen[i][j] == 7) {

					layout[i][j] = empty;

					Vector2D Pos = { i, j };

					gold[Gold::goldNr].Initialize(Gold::goldNr, Pos);
					Gold::goldNr++;
				}
				if (gen[i][j] == 8) {

					layout[i][j] = empty;
					if (Enemy::enemyNr < 11) {
						
						Vector2D Pos;
						Pos.x = i;
						Pos.y = j;

						enemies[Enemy::enemyNr].Initialize(Enemy::enemyNr, Pos);

						Enemy::enemyNr++;
					}
				}

				if (gen[i][j] == 9) {
					layout[i][j] = empty;

					Vector2D Pos;
					Pos.x = i;
					Pos.y = j;

					enemies[0].Initialize(0, Pos);
				}
			}

		//if no player was given, put in one!
		if (enemies[0].Pos.x == -1) {
			
			int spawnX = 1;
			int spawnY = 1;

			bool foundPlaceForRunner = false;

			while (!foundPlaceForRunner) {
				if (layout[spawnX][spawnY] != empty || gen[spawnX][spawnY] == 7 || gen[spawnX][spawnY] == 8) {
					
					std::cout << "\n layout: " << layout[spawnX][spawnY] << ", gold: " << Enemy::GoldChecker(spawnX, spawnY);

					spawnX++;
					if (spawnX == 29) {
						spawnX = 1;
						spawnY++;
					}
				}
				else {

					Vector2D Pos;
					Pos.x = spawnX;
					Pos.y = spawnY;

					enemies[0].Initialize(0, Pos);
					foundPlaceForRunner = true;
				}
			}
		}

		for (int i = 0; i < Enemy::enemyNr; i++) {

			int curX = int(enemies[i].Pos.x + 0.5);
			int curY = int(enemies[i].Pos.y + 0.5);


			if ((layout[curX][curY - 1] == empty || layout[curX][curY - 1] == trapDoor || layout[curX][curY - 1] == pole) && !Enemy::EnemyCheckerGlobal(enemies[i].Pos.x, enemies[i].Pos.y - 1)) {

				enemies[i].state = falling;
				if (i == 0) {
					Audio::SFX[17].PlayPause();
					enemies[i].TextureRef = 52;
				}
					
				else
					enemies[i].TextureRef = 16;
			}

			else {
				enemies[i].state = freeRun;

				if (i == 0)
					enemies[i].TextureRef = 48;
				else
					enemies[i].TextureRef = 12;
			}
		}

		for (int i = 0; i < 50; i++)
			enemies[i].gold = gold;

		Enemy::enemies = enemies;
		Gold::remainingGoldCount = Gold::goldNr;
		menu = L04;
	}
}