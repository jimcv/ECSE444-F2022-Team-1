/*
 * game_engine.c
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

/* Private Includes ------------------------ */
#include <stdio.h>
#include <stdlib.h>

/* Private Defines ------------------------- */
//enemy
#define MAX_ENEMYHP 3
#define MAX_NUM_ENEMIES 10
//player
#define MAX_PLAYERHP 5
//projectile
#define MAX_PROJECTILES 5

/* Private Variable ------------------------ */
enum entity { player, enemy };  //0 is player and 1 is enemy
enum projectiledirection { up, down };  //0 is up 1 is down

/* Private Methods ------------------------- */

/* Private Structs ------------------------- */
typedef struct ENTITY {
	int posit_x;
	int posit_y;
	int health;
	enum entity type;
}entity_t;

typedef struct PROJECTILE {
	int posit_x;
	int posit_y;
	int enable;
	enum projectiledirection direction;
}projectile_t;

/* Public entities ------------------------- */
entity_t enemyList[MAX_NUM_ENEMIES];
entity_t playerChar;
projectile_t projectileList[MAX_PROJECTILES];
int numProjectiles = 0;

//Player functions
void createPlayer(){
	entity_t thisPlayer;
	thisPlayer.health = MAX_PLAYERHP;
	thisPlayer.posit_x = 24;
	thisPlayer.posit_y = 49;
	thisPlayer.type = player;
	playerChar = thisPlayer;
}

entity_t getPlayer(){
	return playerChar;
}
//Enemy functions
void createEnemies() {
	for (int currEnemies = 0; currEnemies < MAX_NUM_ENEMIES; currEnemies++) {
		entity_t thisEnemy;
		thisEnemy.health = MAX_ENEMYHP;
		thisEnemy.type = enemy;
		/*enemy starting position is set on row 10 side by side assumming they are only one ASCII character currently*/
		thisEnemy.posit_x = 19 + currEnemies;
		thisEnemy.posit_y = 10;
		enemyList[currEnemies] = thisEnemy;
	}
}

entity_t* getEnemyList(){
	return enemyList;
}
//Projectile functions
void createProjectile(int x, int y, enum projectiledirection direction){
	projectile_t newProjectile;
	newProjectile.posit_x = x;
	newProjectile.posit_y = y;
	newProjectile.enable = 1;
	newProjectile.direction = direction;
	projectileList[numProjectiles] = newProjectile;
	numProjectiles++;
}
//Movement functions
void moveRight(entity_t* entity){
	entity->posit_x++;
}
void moveLeft(entity_t* entity){
	entity->posit_x--;
}
//Collision detections
void collisionDetection() {
	for (int j = 0; j < MAX_PROJECTILES; j++) {
		if(projectileList[j].posit_x == playerChar.posit_x && projectileList[j].posit_y == playerChar.posit_y){
			if(playerChar.health > 0){
				projectileList[j].enable = 0;
				playerChar.health--;
			}
		}
	}
}
//Update Game
void update() {
	//Projectile movement
	for (int pidx = 0; pidx < numProjectiles; pidx++) {
		if (projectileList[pidx].enable == 1) {
			if (projectileList[pidx].posit_y == 49
					|| projectileList[pidx].posit_y == 0) {
				projectileList[pidx].direction = up;
				projectileList[pidx].posit_x = 0;
				projectileList[pidx].posit_y = 0;
				projectileList[pidx].enable = 0;
			} else {
				if (projectileList[pidx].direction = down) {
					projectileList[pidx].posit_y--;
				} else {
					projectileList[pidx].posit_y++;
				}
			}
		}
	}
}
