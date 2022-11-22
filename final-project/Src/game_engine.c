/*
 * game_engine.c
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

/* Private Includes ------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

/* Private Defines ------------------------- */
//player
#define MAX_PLAYERHP 5

/* Private Variable ------------------------ */
enum entity { player, enemyUnit };  //0 is player and 1 is enemy
//enemy
int MAX_ENEMYHP = 3;
int enemyState = 0;
int enemySteps = 20;  //Number of cycles before enemy moves

/* Private Methods ------------------------- */

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

/* Public entities ------------------------- */
entity_t enemyList[NUM_ENEMIES];
entity_t playerChar;
projectile_t projectileList[NUM_PROJECTILES];
int numProjectiles = 0;

/* Transfer functions ---------------------- */
//transfer Global player to local player struct
void getGlobalUser(user user_t){
	//transfer Global player to local player struct
	playerChar.health = user_t.health;
	playerChar.posit_x = user_t.rb.x;
	playerChar.posit_y = user_t.rb.y;
	playerChar.type = player;
}
//transfer Global enemies to local enemies list
void getGlobalEnemies(enemy* enemies_t){
	for(int eidx = 0; eidx < NUM_ENEMIES; eidx++){
		enemyList[eidx].health = enemies_t[eidx].health;
		enemyList[eidx].posit_x = enemies_t[eidx].rb.x;
		enemyList[eidx].posit_y = enemies_t[eidx].rb.y;
		enemyList[eidx].enabled = enemies_t[eidx].enabled;
		enemyList[eidx].type = enemyUnit;
	}
}
//transfer Global projectiles to local projectile list
void getGlobalProjectiles(projectile* projectiles_t){
	for(int pidx = 0; pidx < NUM_PROJECTILES; pidx++){
		projectileList[pidx].posit_x = projectiles_t[pidx].rb.x;
		projectileList[pidx].posit_y = projectiles_t[pidx].rb.y;
		projectileList[pidx].enable = projectiles_t[pidx].enabled;
	}
}
//transfer data back to global variables
void updateGlobalUser(user* user_t){
	user_t->health = playerChar.health;
	user_t->rb.x = playerChar.posit_x;
	user_t->rb.y = playerChar.posit_y;
}
void updateGlobalEnemies(enemy* enemy_t){
	for(int eidx = 0; eidx < NUM_ENEMIES; eidx++){
		enemy_t[eidx].health = enemyList[eidx].health;
		enemy_t[eidx].rb.x = enemyList[eidx].posit_x;
		enemy_t[eidx].rb.y = enemyList[eidx].posit_y;
		enemy_t[eidx].enabled = enemyList[eidx].enabled;
	}
}
void updateGlobalProjectiles(projectile* projectiles_t){
	for(int pidx = 0; pidx < NUM_PROJECTILES; pidx++){
		projectiles_t[pidx].enabled = projectileList[pidx].enable;
		projectiles_t[pidx].rb.x = projectileList[pidx].posit_x;
		projectiles_t[pidx].rb.y = projectileList[pidx].posit_y;
	}
}
/* Player functions -------------------------- */
void createPlayer(user* user_t){
	entity_t thisPlayer;
	thisPlayer.health = MAX_PLAYERHP;
	thisPlayer.posit_x = MAX_X/2;
	thisPlayer.posit_y = MAX_Y - 1;
	thisPlayer.type = player;
	playerChar = thisPlayer;
	updateGlobalUser(user_t);
}

entity_t getPlayer(){
	return playerChar;
}
/* Enemy functions --------------------------- */
void createEnemies(enemy* enemy_t) {
	for (int currEnemies = 0; currEnemies < NUM_ENEMIES; currEnemies++) {
		entity_t thisEnemy;
		int step = MAX_X/NUM_ENEMIES;
		int start = step / 2;
		thisEnemy.health = MAX_ENEMYHP;
		thisEnemy.type = enemyUnit;
		/*enemy starting position is set on row 10 side by side assumming they are only one ASCII character currently*/
		thisEnemy.posit_x = start + (currEnemies * step);
		thisEnemy.posit_y = 1;
		thisEnemy.enabled = true;
		enemyList[currEnemies] = thisEnemy;
	}
	updateGlobalEnemies(enemy_t);
}

entity_t* getEnemyList(){
	return enemyList;
}
/* Projectile functions ------------------------ */
void createProjectile(int x, int y){
	projectile_t newProjectile;
	newProjectile.posit_x = x;
	newProjectile.posit_y = y;
	newProjectile.enable = 1;
	for(int pidx = 0; pidx < NUM_PROJECTILES; pidx++){
		if(projectileList[pidx].enable == 0){
			projectileList[pidx] = newProjectile;
			break;
		}
	}
}
/* Movement functions -------------------------- */
void moveRight(entity_t *entity) {
	if (entity->posit_x >= MAX_X - 1) {
		entity->posit_x = 0;
	} else {
		entity->posit_x++;
	}
}
void moveLeft(entity_t *entity) {
	if (entity->posit_x <= 0) {
		entity->posit_x = MAX_X - 1;
	} else {
		entity->posit_x--;
	}

}
/* Collision Detection -------------------------- */
void collisionDetection() {
	for (int pidx = 0; pidx < NUM_PROJECTILES; pidx++) {
		if (projectileList[pidx].enable == 1) {
			for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
				if (projectileList[pidx].posit_x == enemyList[eidx].posit_x
						&& projectileList[pidx].posit_y
								== enemyList[eidx].posit_y) {
					projectileList[pidx].enable = 0;
					projectileList[pidx].posit_x = 0;
					projectileList[pidx].posit_y = 0;
					if (enemyList[eidx].health > 0) {
						enemyList[eidx].health--;
					}
					if(enemyList[eidx].health <= 0){
						enemyList[eidx].enabled = false;
					}
					break;
				}
			}
		}
	}
}
/* Enemy AI ------------------------------------- */
void enemyMove() {
	if (enemyState >= enemySteps) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].health > 0) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					enemyList[eidx].posit_y++;
				}
			}
		}
		enemyState = 0;
	}
	enemyState++;
	/*switch (enemyState) {
	case 0:
		//move right 1
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].health > 0) {
				if (enemyList[eidx].posit_x < MAX_X - 1) {
					enemyList[eidx].posit_x++;
				}
			}
		}
		enemyState++;
		break;
	}*/
}
/* Enemy reached goal --------------------------- */
void enemyReached() {
	for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
		if (enemyList[eidx].posit_y >= (MAX_Y - 1)) {
			enemyList[eidx].health = 0;
			enemyList[eidx].posit_x = 0;
			enemyList[eidx].posit_y = 0;
			enemyList[eidx].enabled = false;
			if(playerChar.health > 0){
				playerChar.health--;
			}
		}
	}
}
/* Move Projectile ------------------------------ */
void moveProjectiles() {
	for (int pidx = 0; pidx < NUM_PROJECTILES; pidx++) {
		if (projectileList[pidx].enable == 1) {
			if (projectileList[pidx].posit_y == 0) {
				projectileList[pidx].posit_x = 0;
				projectileList[pidx].posit_y = 0;
				projectileList[pidx].enable = 0;
			} else {
					projectileList[pidx].posit_y--;
			}
		}
	}
}
/* Move Player ---------------------------------- */
void movePlayer(float pEulerData){
	if(pEulerData < -15){
		moveLeft(&playerChar);
	}
	else if (pEulerData > 15) {
		moveRight(&playerChar);
	}
}

/* End game ------------------------------------- */
int gameEnd(){
	int gameOver = 0;
	if(playerChar.health <= 0){
		gameOver = 1;
	}
	return gameOver;
}

//Update Game
int updateGame(user* user_t,enemy* enemies_t, projectile* projectiles_t, bool fired, float pEulerData) {

	int gameOver = 0;

	//grab global data to local
	getGlobalEnemies(enemies_t);
	getGlobalProjectiles(projectiles_t);
	getGlobalUser(*user_t);
	//player move
	movePlayer(pEulerData);
	//projectile move
	moveProjectiles();
	//fire projectile
	if(fired){
		createProjectile(playerChar.posit_x, playerChar.posit_y - 1);
	}
	//collision
	collisionDetection();
	//enemy move
	enemyMove();
	//check if enemy has reached bottom
	enemyReached();
	//gameover check
	gameOver = gameEnd();
	//update global data
	updateGlobalEnemies(enemies_t);
	updateGlobalProjectiles(projectiles_t);
	updateGlobalUser(user_t);

	return gameOver;
}
