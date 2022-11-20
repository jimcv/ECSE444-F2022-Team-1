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
int update(user* user_t,enemy* enemies_t, projectile* projectiles_t);
void createPlayer();
void createEnemies();
void createProjectile(int x, int y);

#endif /* GAME_ENGINE_H_ */
