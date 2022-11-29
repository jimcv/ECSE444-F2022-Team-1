/*
 * game_engine.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_
/* Private Includes ------------------------- */
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

/* Private enums ------------------------- */
enum entity { player, enemyUnit };  //0 is player and 1 is enemy

/* Private Structs ------------------------- */
typedef struct ENTITY {
	float posit_x;
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

/**
 * Initialize entities.
 * @param reconfigurationRequested whether to manually write configuration.
 * @param gameObjects pointer to the global game objects store.
 */
void initEngine(bool reconfigurationRequested, game_objects *gameObjects);

/**
 * Update the game structures after an iteration.
 * @param gameObjectsSV shared variable identifier for the game objects.
 * @param fired whether the button was just pressed.
 * @param pEulerData the rotation of the board in degrees from the y-axis.
 */
uint32_t updateGame(uint32_t gameObjectsSV, bool fired, float pEulerData);

/**
 * Initialize the player character.
 * @param user_t the global user structure to write to.
 */
void createPlayer(user* user_t);

/**
 * Initialize the enemy array.
 * @param enemy_t the global enemy array to write to.
 */
void createEnemies(enemy enemy_t[NUM_ENEMIES]);

/**
 * Create a projectile entity.
 * @param x the x-coordinate of the projectile.
 * @param y the y-coordinate of the projectile.
 */
void createProjectile(int x, int y);

#endif /* GAME_ENGINE_H_ */
