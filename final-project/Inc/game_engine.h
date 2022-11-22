/*
 * game_engine.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_
/* Private Defines ------------------------- */
//player
#define MAX_PLAYERHP 5
#define MAX_ENEMYHP 3
#define enemySteps 20  //Number of cycles before enemy moves

#include "main.h"

/* Private enums ------------------------- */
enum entity { player, enemyUnit };  //0 is player and 1 is enemy

/* Private Structs ------------------------- */
typedef struct ENTITY {
	int posit_x;
	int posit_y;
	int health;
	bool enabled;
	enum entity type;
}entity_t;

typedef struct gameProjectile {
	int posit_x;
	int posit_y;
	char enable;
}projectile_t;

//functions
uint32_t updateGame(uint32_t gameObjectsSV, bool fired, float pEulerData);
void createPlayer(user* user_t);
void createEnemies(enemy* enemy_t);
void createProjectile(int x, int y);

#endif /* GAME_ENGINE_H_ */
