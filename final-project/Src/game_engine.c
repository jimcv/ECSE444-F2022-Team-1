/*
 * game_engine.c
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

/* Private Includes ------------------------ */
#include <stdio.h>

/* Private Defines ------------------------- */
//enemy
#define MAX_ENEMYHP 3
#define MAX_NUM_ENEMIES 10
//player
#define MAX_PLAYERHP 5
//projectile
#define MAX_PROJECTILE 5

/* Private Variable ------------------------ */
enum entity { player, enemy };

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
}projectile_t;

/* Public entities ------------------------- */
entity_t enemyList[MAX_NUM_ENEMIES];
entity_t playerChar;
projectile_t projectileList[MAX_PROJECTILE];

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
//Movement functions
void moveRight(entity_t entity){
	entity.posit_x++;
}
void moveLeft(entity_t entity){
	entity.posit_x--;
}
//Collision detections
void collisionDetection() {
	for (int j = 0; j < MAX_PROJECTILE; j++) {
		if(projectileList[j].posit_x == playerChar.posit_y && projectileList[j].posit_x == playerChar.posit_y){
			if(playerChar.health > 0){
				playerChar.health--;
			}
		}
	}
}
