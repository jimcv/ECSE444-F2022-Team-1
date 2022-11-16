/*
 * game_engine.c
 *
 *  Created on: Nov 16, 2022
 *      Author: Thomas Song
 */

/* Private Defines ------------------------- */
#define MAX_ENEMYHP 3
#define MAX_NUM_ENEMIES 10
#define MAX_PLAYERHP 5

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
/* Private Lists ------------------------- */
entity_t enemyList[MAX_NUM_ENEMIES];


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
