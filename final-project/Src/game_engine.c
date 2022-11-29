/*
 * game_engine.c
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

/* Private Includes ------------------------ */
#include "game_engine.h"
#include "flash_config.h"

/* Private Variable ------------------------ */
// configuration model
engine_configuration engineConfig;

//enemy
int enemyState = 0;
int numProjectiles = 0;

entity_t enemyList[NUM_ENEMIES];
entity_t playerChar;
projectile_t projectileList[NUM_PROJECTILES];

/**
 * Initialize entities.
 * @param reconfigurationRequested whether to manually write configuration.
 * @param gameObjects pointer to the global game objects store.
 */
void initEngine(bool reconfigurationRequested, game_objects *gameObjects)
{
  if (reconfigurationRequested)
  {
    engineConfig.enemySteps = 120; //must be divisible by 6 because 6 states.
    engineConfig.maxEnemyHP = 3;
    engineConfig.maxPlayerHP = 5;
    setEngineConfiguration(&engineConfig);
  }
  else
  {
    getEngineConfiguration(&engineConfig);
  }

  createPlayer(&gameObjects->user);
  createEnemies(gameObjects->enemies);
  for (uint32_t i = 0; i < NUM_PROJECTILES; ++i)
  {
    projectileList[i].enable = false;
    gameObjects->projectiles[i].enabled = false;
  }
}

/* Transfer functions ---------------------- */
//transfer data back to global variables
void updateGlobalUser(user* user_t){
  user_t->enabled = playerChar.health > 0;
  user_t->x = round(playerChar.posit_x);
  user_t->y = playerChar.posit_y;
}

void updateGlobalEnemies(enemy* enemy_t){
  for(int eidx = 0; eidx < NUM_ENEMIES; eidx++){
    enemy_t[eidx].enabled = enemyList[eidx].enabled;
    enemy_t[eidx].x = round(enemyList[eidx].posit_x);
    enemy_t[eidx].y = enemyList[eidx].posit_y;
  }
}

void updateGlobalProjectiles(projectile* projectiles_t){
  for(int pidx = 0; pidx < NUM_PROJECTILES; pidx++){
    projectiles_t[pidx].enabled = projectileList[pidx].enable;
    projectiles_t[pidx].x = projectileList[pidx].posit_x;
    projectiles_t[pidx].y = projectileList[pidx].posit_y;
  }
}

void updateGlobalGameObjects(void *gameObjectsPtr)
{
  game_objects *gameObjects = (game_objects*)gameObjectsPtr;

  updateGlobalUser(&gameObjects->user);
  updateGlobalEnemies(gameObjects->enemies);
  updateGlobalProjectiles(gameObjects->projectiles);
}

/* Player functions -------------------------- */
/**
 * Initialize the player character.
 * @param user_t the global user structure to write to.
 */
void createPlayer(user* user_t){
	entity_t thisPlayer;
	thisPlayer.health = engineConfig.maxPlayerHP;
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
/**
 * Initialize the enemy array.
 * @param enemy_t the global enemy array to write to.
 */
void createEnemies(enemy enemy_t[NUM_ENEMIES]) {
	for (int currEnemies = 0; currEnemies < NUM_ENEMIES; currEnemies++) {
		entity_t thisEnemy;
		int step = MAX_X/NUM_ENEMIES;
		int start = step / 2;
		thisEnemy.health = engineConfig.maxEnemyHP;
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
/**
 * Create a projectile entity.
 * @param x the x-coordinate of the projectile.
 * @param y the y-coordinate of the projectile.
 */
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
	if (round(entity->posit_x) >= MAX_X - 1) {
		entity->posit_x = 0;
	} else {
		entity->posit_x+=0.5;
	}
}

void moveLeft(entity_t *entity) {
	if (round(entity->posit_x) <= 0) {
		entity->posit_x = MAX_X - 1;
	} else {
		entity->posit_x-=0.5;
	}

}

/* Collision Detection -------------------------- */
void collisionDetection() {
	for (int pidx = 0; pidx < NUM_PROJECTILES; pidx++) {
		if (projectileList[pidx].enable == 1) {
			for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
				if (projectileList[pidx].posit_x == round(enemyList[eidx].posit_x)
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
	int stepping = engineConfig.enemySteps/6;
	if (enemyState == stepping) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].enabled) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					moveLeft(&enemyList[eidx]);
					moveLeft(&enemyList[eidx]);
				}
			}
		}
	}
	else if (enemyState == 2*stepping) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].enabled) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					moveLeft(&enemyList[eidx]);
					moveLeft(&enemyList[eidx]);
				}
			}
		}
	}
	else if (enemyState == 3*stepping) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].health > 0) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					enemyList[eidx].posit_y++;
				}
			}
		}
	}
	else if (enemyState == 4*stepping) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].enabled) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					moveRight(&enemyList[eidx]);
					moveRight(&enemyList[eidx]);
				}
			}
		}
	}
	else if (enemyState == 5*stepping) {
		for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
			if (enemyList[eidx].enabled) {
				if (enemyList[eidx].posit_y < MAX_Y - 1) {
					moveRight(&enemyList[eidx]);
					moveRight(&enemyList[eidx]);
				}
			}
		}
	}
	else if (enemyState >= engineConfig.enemySteps) {
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
	if(pEulerData < -10 && pEulerData > -40){
		moveLeft(&playerChar);
	}
	else if(pEulerData <= -40){
		moveLeft(&playerChar);
		moveLeft(&playerChar);
	}
	else if (pEulerData > 10 && pEulerData < 40) {
		moveRight(&playerChar);
	}
	else if(pEulerData >= 40){
		moveRight(&playerChar);
		moveRight(&playerChar);
	}
}

/* Spawn enemies -------------------------------- */
void spawnEnemies() {
	bool emptyTop = true;
	int numMissingEnemy = 0;
	for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
		if (enemyList[eidx].posit_y == 1) {
			emptyTop = false;
		}
		if (!enemyList[eidx].enabled) {
			numMissingEnemy++;
		}

	}
	if (emptyTop) {
		int currEnemy = 0;
		if (numMissingEnemy == 1) {
			for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
				if (!enemyList[eidx].enabled) {
					entity_t thisEnemy;
					int step = MAX_X / NUM_ENEMIES;
					int start = step / 2;
					thisEnemy.health = engineConfig.maxEnemyHP;
					thisEnemy.type = enemyUnit;
					int currEnemy = rand() % 5;
					thisEnemy.posit_x = start + (currEnemy * step);
					thisEnemy.posit_y = 1;
					thisEnemy.enabled = true;
					enemyList[eidx] = thisEnemy;
					break;
				}
			}
		} else {
			for (int eidx = 0; eidx < NUM_ENEMIES; eidx++) {
				if (!enemyList[eidx].enabled) {
					entity_t thisEnemy;
					int step = MAX_X / numMissingEnemy;
					int start = step / 2;
					thisEnemy.health = engineConfig.maxEnemyHP;
					thisEnemy.type = enemyUnit;
					/*enemy starting position is set on row 10 side by side assumming they are only one ASCII character currently*/
					thisEnemy.posit_x = start + (currEnemy * step);
					thisEnemy.posit_y = 1;
					thisEnemy.enabled = true;
					enemyList[eidx] = thisEnemy;
					currEnemy++;
				}
			}
		}
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

/**
 * Update the game structures after an iteration.
 * @param gameObjectsSV shared variable identifier for the game objects.
 * @param fired whether the button was just pressed.
 * @param pEulerData the rotation of the board in degrees from the y-axis.
 */
uint32_t updateGame(uint32_t gameObjectsSV, bool fired, float pEulerData) {
	int gameOver = 0;

	//player move
	movePlayer(pEulerData);
	//projectile move
	moveProjectiles();
	//fire projectile
	if(fired){
		createProjectile(round(playerChar.posit_x), playerChar.posit_y - 1);
	}

	//collision
	collisionDetection();

	//enemy move
	enemyMove();
	//check if enemy has reached bottom
	enemyReached();
	//spawn new enemies
	spawnEnemies();
	//gameover check
	gameOver = gameEnd();

	//update global data
	lockSharedVariableAndExecute(gameObjectsSV, OS_TIMEOUT, updateGlobalGameObjects);

	return gameOver;
}
