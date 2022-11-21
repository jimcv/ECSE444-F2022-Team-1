/*
 * game_engine.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_

#include "main.h"

//functions
int updateGame(user* user_t,enemy* enemies_t, projectile* projectiles_t, bool fired);
void createPlayer(user* user_t);
void createEnemies(enemy* enemy_t);
void createProjectile(int x, int y,projectile* projectiles_t);

#endif /* GAME_ENGINE_H_ */
