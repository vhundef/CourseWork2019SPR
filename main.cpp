/* Copyright (C) Vladimir Shubarin - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Vladimir Shubarin <vhundef@gmail.com>, Feb 2019
 */

#include <cstdio>
#include <SDL/SDL.h>
#include <SDL/SDL_draw.h>
#include <iostream>
#include <fstream>
#include "Game/ship.h"
#include "Game/hud.h"
#include "Game/game_manager.h"
#include "Game/weapon.h"
#include "Game/player_ship.h"


int main(int argc, char *argv[]) {

	SDL_Surface *screen; /* объявление указателя на поверхность: */
	SDL_Event event;
	SDL_Rect bg;
	Sint16 max_x, max_y;
	int nextstep = 1; /* для цикла обработки сообщений */
	max_x = 1280;
	max_y = 720;
	screen = SDL_SetVideoMode(max_x, max_y, 16,
	                          SDL_ANYFORMAT | SDL_DOUBLEBUF);    /* инициализация библиотеки и установка видеорежима */
	if (!screen) {
		fprintf(stderr, "SDL mode failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1; /* Выход с одним кодом ошибки */
	}
	SDL_WM_SetCaption("Курсовая PRE_ALPHA 0001", NULL);
	bg.w = max_x;
	bg.h = max_y;
	bg.x = 0;
	bg.y = 0;
	bool isInShop = false;
	bool doOnce = true;
	HUD hud(screen);
	Weapon weapon(screen, max_x, max_y, true);
	GameManager gm;
	gm.setMoney(60);
	Ship *ships = nullptr;
	PlayerShip player;

	string playerName;


	int elapsed = 0, current = 0, timeSinceSecond = 0, frames = 0, next, avgFPS = 100; //avgFPS - Avg fps per seconds

	int framerate = 59; // This is proposed FPS

	int playerHit = 0;

	//Start Screen

	SDL_FillRect(screen, &bg, 0xFFFFFF);
	Draw_FillRect(screen, static_cast<Sint16>(max_x / 2 - 120), 180, 250, 60, 0x000);
	hud.drawText("Start new game", max_x / 2 - 65, 200);
	Draw_FillRect(screen, static_cast<Sint16>(max_x / 2 - 120), 280, 250, 60, 0x000);
	hud.drawText("Leaderboard", max_x / 2 - 50, 300);
	Draw_FillRect(screen, static_cast<Sint16>(max_x / 2 - 120), 380, 250, 60, 0x000);
	hud.drawText("Quit", max_x / 2 - 15, 400);
	SDL_UpdateRect(screen, 0, 0, 1280, 720);
	int mouseX, mouseY;
	while (true) {
		SDL_PollEvent(&event);
		SDL_GetMouseState(&mouseX, &mouseY);
		if ((mouseX > max_x / 2 - 120) && (mouseX < max_x / 2 + 130) && (mouseY > 180) && (mouseY < 240) &&
		    (event.type == SDL_MOUSEBUTTONDOWN)) {
			break;
		}
		if ((mouseX > max_x / 2 - 120) && (mouseX < max_x / 2 + 130) && (mouseY > 280) && (mouseY < 340) &&
		    (event.type == SDL_MOUSEBUTTONDOWN)) {
			nextstep = -20;
			break;
		}
		if ((mouseX > max_x / 2 - 120) && (mouseX < max_x / 2 + 130) && (mouseY > 380) && (mouseY < 440) &&
		    (event.type == SDL_MOUSEBUTTONDOWN)) {
			nextstep = 0;
			break;
		}
	}



	// THIS IS GAME IT SELF

	SDL_FillRect(screen, &bg, 0x0d34f6);
	bg.w = max_x;
	bg.h = max_y - 200;
	bg.x = 0;
	bg.y = 120;


	while (nextstep > 0) // цикл перерисовки и обработки событий
	{
		elapsed = SDL_GetTicks() - current;
		current += elapsed;
		timeSinceSecond += elapsed;

		frames++;
		SDL_FillRect(screen, &bg, 0x0d34f6);
		if (gm.getMoney() < 0)
			nextstep = -999;
		if (SDL_PollEvent(&event)) // проверяем нажатие клавиш на клавиатуре
		{
			if (event.type == SDL_QUIT ||
			    (event.type == SDL_KEYDOWN &&
			     event.key.keysym.sym == SDLK_ESCAPE))
				nextstep = 0;
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE) {
				weapon.shoot((player.getCoords().x1 + player.getCoords().x2) / 2);
				gm.setMoney(gm.getMoney() - 5);
				gm.setShots(gm.getShots() + 1);
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_DOWN) {
				player.setMovementSpeed(0);
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_UP) {
				player.setMovementSpeed(player.getMaxMovementSpeed());
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_LEFT) {
				player.setMovementDirection(-1);
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_RIGHT) {
				player.setMovementDirection(1);
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_b) {
				for (int j = 0; j < gm.getWave() * 2; ++j) {
					ships[j].setBAllowedToShoot(false);
					ships[j].setMovementSpeed(0);
					player.setMovementSpeed(0);
					isInShop = true;
				}
			}
		}
		if (isInShop) {
			//There should be code that displays shop menu
			// but until this moment this will do
			if (doOnce) {
				cout << "= TMP SHOP =" << endl
				     << "1)Increase ship speed (200)\n2)Restore health to max (300)\n3)Increase max health(1000)\n4)Quit"
				     << endl;
				doOnce = false;
			}

			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_1) {
				if ((gm.getMoney() - 200) >= 0) {
					gm.setMoney(gm.getMoney() - 200);
					player.setMaxMovementSpeed(player.getMaxMovementSpeed() + 1);
					isInShop = false;
					player.setMovementSpeed(player.getMaxMovementSpeed());
					for (int j = 0; j < gm.getWave() * 2; ++j) {
						ships[j].setMovementSpeed(ships[j].getMaxSpeed());
					}
					doOnce = true;
				} else
					cout << "NOT ENOUGH MONEY!" << endl;

			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_2) {

				if ((gm.getMoney() - 300) >= 0) {
					gm.setMoney(gm.getMoney() - 300);
					player.setHealth(player.getMaxHealth());
					isInShop = false;
					player.setMovementSpeed(player.getMaxMovementSpeed());
					for (int j = 0; j < gm.getWave() * 2; ++j) {
						ships[j].setMovementSpeed(ships[j].getMaxSpeed());
					}
					doOnce = true;
				} else
					cout << "NOT ENOUGH MONEY!" << endl;

			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_3) {
				if ((gm.getMoney() - 1000) >= 0) {
					player.setMaxHealth(player.getMaxHealth() + 100);
					gm.setMoney(gm.getMoney() - 1000);
					isInShop = false;
					player.setMovementSpeed(player.getMaxMovementSpeed());
					for (int j = 0; j < gm.getWave() * 2; ++j) {
						ships[j].setMovementSpeed(ships[j].getMaxSpeed());
					}
					doOnce = true;
				} else
					cout << "NOT ENOUGH MONEY!" << endl;
			}
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_4) {
				isInShop = false;
				player.setMovementSpeed(player.getMaxMovementSpeed());
				for (int j = 0; j < gm.getWave() * 2; ++j) {
					ships[j].setMovementSpeed(ships[j].getMaxSpeed());
				}
				doOnce = true;

			}
		} else {
			gm.updateActionRect(screen);
			hud.reDraw(gm.getMoney(), gm.getWave());
			weapon.updateParticles();

			hud.drawText("FPS: " + to_string(avgFPS), 0, 20);
			hud.drawText("HP: " + to_string(player.getHealth()), 1200, 20);
			if (timeSinceSecond >= 1000) {
				timeSinceSecond = 0;
				avgFPS = frames;
				frames = 0;
			}

			//IF no ships left aka next wave
			if (gm.getShipsLeft() <= 0) {
				gm.setWave(gm.getWave() + 1);
				gm.setShipsLeft(gm.getWave() * 2);
				weapon.reset(gm.getWave());
				delete[] ships;
				cout << "spwn: " << gm.getWave() * 2 << endl;
				ships = new Ship[gm.getWave() * 2];
				for (int i = 0; i < gm.getWave() * 2; ++i) {
					ships[i].setScreen(screen);
				}
				player.setHealth(player.getHealth() + 25);
			}

			//Ships redraw
			for (int j = 0; j < gm.getWave() * 2; ++j) {
				if (ships[j].getHealth() > 0) {
					int hits = weapon.checkCollisions(ships[j].getCoords());
					ships[j].setHealth(ships[j].getHealth() - hits * 25);
					gm.setMoney(gm.getMoney() + 15 * hits);
					if (hits > 0) {
						gm.setHits(gm.getHits() + hits);
						ships[j].spawnHit(weapon.getHitLoc());
						cout << "Ship: " << j << "\t" << "Hits: " << hits << endl;
						cout << "Ship: " << j << "HP: " << ships[j].getHealth() << endl;
						if (ships[j].getHealth() <= 0) {
							gm.setShipsLeft(gm.getShipsLeft() - 1);
							gm.setKilledShips(gm.getKilledShips() + 1);
						}
					}
					playerHit = ships[j].weapon.checkCollisions(player.getCoords());
					player.setHealth(player.getHealth() - playerHit * 25);
					if (player.getHealth() <= 0)
						nextstep = -999;
					ships[j].reDraw(player.getCoords());
					if (playerHit > 0)
						player.spawnHit(ships[j].weapon.getHitLoc());
				}
			}

			player.reDraw(screen);
			SDL_UpdateRect(screen, 0, 0, 1280, 720);
		}
		next = SDL_GetTicks();
		if (next - current < 1000.0 / framerate) {
			SDL_Delay(1000.f / framerate - (next - current));
		}

	}


	if (nextstep == -999) {
		SDL_Rect gmOver;
		gmOver.x = max_x / 2 - 400;
		gmOver.y = max_y / 2 - 60;
		gmOver.h = 120;
		gmOver.w = 800;
		SDL_FillRect(screen, &gmOver, 0xFF12121);
		hud.drawText("GAME OVER!", max_x / 2 - 40, max_y / 2 - 20);
		hud.drawText("Press ESC to Quit", max_x / 2 - 95, max_y / 2 + 10);
		hud.drawText("Press Enter to show Stats", max_x / 2 - 95, max_y / 2 + 30);
		SDL_UpdateRect(screen, 0, 0, 1280, 720);
		ofstream file;
		file.open("LeaderBoard", std::ios_base::app);
		file << playerName << "                            " << (float) gm.getHits() / (gm.getShots() - gm.getHits()) * 100 << "                " << gm.getWave()
		     << "                " << gm.getKilledShips() << '\n';
		file.close();
		while (true)
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
					nextstep = 0;
					break;
				}
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_ENTER) {
					nextstep = -30;
					break;
				}
			}
	}
	if (nextstep == -30) {
		SDL_Rect gmOver;
		gmOver.x = 0;
		gmOver.y = 0;
		gmOver.h = max_y;
		gmOver.w = max_x;
		SDL_FillRect(screen, &gmOver, 0x000000);
		hud.drawText("Stats", max_x / 2, 70);
		hud.drawText("Ships killed:  " + to_string(gm.getKilledShips()), 100, 100);
		hud.drawText("Waves survived:  " + to_string(gm.getWave()), 100, 130);
		hud.drawText("Hits:  " + to_string(gm.getHits()), 100, 160);
		hud.drawText("Misses:  " + to_string(gm.getShots() - gm.getHits()), 100, 190);
		hud.drawText("Accuracy:  " + to_string((float) gm.getHits() / (gm.getShots() - gm.getHits()) * 100) + "%",
		             100,
		             220);
		hud.drawText("ESC to Quit", max_x / 2 - 20, max_y - 100);

		SDL_UpdateRect(screen, 0, 0, 1280, 720);

		while (true)
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
					break;
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_ENTER) {
					nextstep = -20;
					break;
				}
			}

	}
	if (nextstep == -20) {
		SDL_Rect gmOver;
		gmOver.x = 0;
		gmOver.y = 0;
		gmOver.h = max_y;
		gmOver.w = max_x;
		SDL_FillRect(screen, &gmOver, 0x000000);
		SDL_UpdateRect(screen, 0, 0, 1280, 720);
		ifstream file;
		file.open("LeaderBoard");
		string PlayerName, str;
		int i = 0;
		while (getline(file, str)) {
			i++;
			hud.drawText(str, 10, 10 + i * 20);
			SDL_UpdateRect(screen, 0, 0, 1280, 720);

		}
		while (true)
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
					break;
			}
	}

	SDL_FreeSurface(screen);
	SDL_Quit();

	return 0; /* Нормальное завершение */
}