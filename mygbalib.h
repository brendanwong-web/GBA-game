#include "sprites.h"
#define INPUT                      (KEY_MASK & (~REG_KEYS))

#define MaxY 120
int gameTimer = 0;
int gameMode;
int currLevel = 1;
int countBounce = 0;
int collision = -1;
#define LEVEL_2 5


typedef struct gameCharacter {
   int x;
   int y;
   int vx;
   int vy;
   int dir;
};

typedef struct gameItem {
   int x;
   int y;
   int vx;
   int vy;
   int a;
   int dropped;
   
};

// Modes
#define MENU_MODE        0
#define PLAY_MODE       1
#define RESET_MODE       2
#define PAUSE_MODE 3
#define LEVEL_MODE 4

// Player
struct gameCharacter player;
#define jumpheight 6
#define cooldown 5
int cooldownTimer = 0;
void init_player(struct gameCharacter* player) {
   player->x = 100;
   player->y = MaxY;
   player->dir = 1;
}
// End Player

// Spoon
struct gameItem spoon;
void init_spoon(struct gameItem* spoon) {
   spoon->x = player.x;
   spoon->y = player.y-20;
   spoon->dropped = 0;
   spoon->vx = 0;
   spoon->vy = 0;
   spoon->a = 0;
}
// End Spoon


// Coins: No coins is based on currLevel
#define noCoins 4
int coinsPos[noCoins][2] = {{30,30}, {210, 30}, {50, 50}, {190, 50}};
struct gameItem coins[noCoins];

void init_coins() {
   int i;
   int j;
   for (i=0;i<currLevel*2;i++) {
     coins[i].x = coinsPos[i][0];
     coins[i].y = coinsPos[i][1];
   }  
   for (j=currLevel*2;j<noCoins;j++){
     coins[j].x = 240;
     coins[j].y = 160;
   }  
} 

// End Coins

// Items:
#define noItems 3
#define maxItemVy 20
struct gameItem gameItems2[noItems];
// Init item characteristics: [x, y, vx, vy]
int gameItemsC[noItems][4] = {
  {80, 30, 10, -10}, // item 0
  {50, 20, 10, -20}, // item 1
  {30, 10, 20, 0} // item 2
  };

void init_items() {
	 for (int i=0;i<currLevel;i++) {
	   gameItems2[i].x = gameItemsC[i][0];
	   gameItems2[i].y = gameItemsC[i][1];
	   gameItems2[i].vx = gameItemsC[i][2];
	   gameItems2[i].vy = gameItemsC[i][3];
	   gameItems2[i].a = 1;
	   gameItems2[i].dropped = 0;
	 }
	 for (int i=currLevel;i<noItems;i++) {
	   gameItems2[i].x = 240;
	   gameItems2[i].y = 160;
	   gameItems2[i].vx = 0;
	   gameItems2[i].vy = 0;
	   gameItems2[i].a = 0;
	   gameItems2[i].dropped = 0;
	 }   
} 

// End Items



void drawGameOver() {
    int x = 66;
    int y = 70;
    int spacing = 12;

    drawSprite8(TILE_G, 90, x + spacing*0, y); // G
    drawSprite8(TILE_A, 91, x + spacing*1, y); // A
    drawSprite8(TILE_M, 92, x + spacing*2, y); // M
    drawSprite8(TILE_E, 93, x + spacing*3, y); // E
    drawSprite8(TILE_O, 94, x + spacing*5, y); // O (väli jälkeen E)
    drawSprite8(TILE_V, 95, x + spacing*6, y); // V
    drawSprite8(TILE_E, 96, x + spacing*7, y); // E again
    drawSprite8(TILE_R, 97, x + spacing*8, y); // R
}

void drawMenu() {
    for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
    int x = 66;
    int y = 70;
    int spacing = 12;

    drawSprite8(TILE_MENU_P, 90, x + spacing*0, y); // P
    drawSprite8(TILE_MENU_R, 91, x + spacing*1, y); // R
    drawSprite8(TILE_MENU_E, 92, x + spacing*2, y); // E
    drawSprite8(TILE_MENU_S, 93, x + spacing*3, y); // S
    drawSprite8(TILE_MENU_S, 94, x + spacing*4, y); // S
}  

void pause() {
    gameMode = PAUSE_MODE;
    int x = 66;
    int y = 70;
    int spacing = 12;

    drawSprite8(TILE_MENU_P, 90, x + spacing*0, y); // G
    drawSprite8(TILE_MENU_S, 91, x + spacing*1, y); // A
    drawSprite8(TILE_MENU_E, 92, x + spacing*2, y); // M
    drawSprite8(TILE_MENU_R, 93, x + spacing*3, y); // E
    drawSprite8(TILE_MENU_S, 94, x + spacing*4, y); // O (väli jälkeen E)
}  

void reset_game() {
   for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
   collision = -1;
   init_player(&player);
	 init_spoon(&spoon);
	 init_coins();
	 init_items();
	 gameMode = PLAY_MODE;
}   

u8 checkCollisions(struct gameCharacter* item1, struct gameItem item) {
  return (item.x >= item1->x && item.x <= item1->x + 16) &&
  (item.y >= item1->y-16 && item.y <= item1->y + 16) ||
  (item1->x >= item.x && item1->x <= item.x + 16) &&
    (item1->y >= item.y-16 && item1->y <= item.y + 16);
	}
	
void buttonR() {
   if (player.x >= 230) {
      return;
   } 
      player.dir  = 1;
      player.x++;
      spoon.x++;

}   

 
void buttonL() {
   if (player.x <= 1) {
      return;
   } 
   player.dir = -1;
 	player.x--;
 	spoon.x--;
} 

void buttonU() {
   
}

void buttonD() {
  switch(gameMode) {
    case LEVEL_MODE:
      collision = -1;
       init_player(&player);
    	 init_spoon(&spoon);
    	 init_items();
    	 init_coins();
    	 gameMode = PLAY_MODE;
  }  
}   

void buttonS() { 
  switch(gameMode) {
    case MENU_MODE:
      gameMode = PLAY_MODE;
    case RESET_MODE:
      reset_game();
  }  
}   

void buttonSel() {
   if (gameMode == PLAY_MODE) {
       pause();
   } else {
     gameMode = PLAY_MODE;
   }  

}  

  

void buttonA() {
//implement some function
}  

void buttonB() {
   if (cooldownTimer != 0) {return;}
   if (collision > -1) {
      gameItems2[collision].vy = -40;
      if (player.dir * gameItems2[collision-1].vx < 0) {
        gameItems2[collision].vx = -gameItems2[collision].vx;
   	     }
   	  countBounce += 1;
   }
    cooldownTimer = cooldown;
}  

void checkbutton(void)
{
	// Gift function to show you how a function that can be called upon button interrupt to detect which button was pressed and run a specific function for each button could look like. You would have to define each buttonA/buttonB/... function yourself.
    u16 buttons = INPUT;
    
    if ((buttons & KEY_A) == KEY_A)
    {
        buttonA(); // x Key
    }
    if ((buttons & KEY_B) == KEY_B)
    {
        buttonB(); // z Key
    }if ((buttons & KEY_SELECT) == KEY_SELECT)
    {
        buttonSel();
    }
    if ((buttons & KEY_START) == KEY_START)
    {
        buttonS(); // Enter
      
    }
    if ((buttons & KEY_RIGHT) == KEY_RIGHT)
    {
        buttonR(&player);
    }
    if ((buttons & KEY_LEFT) == KEY_LEFT)
    {
        buttonL(&player);
    }
    if ((buttons & KEY_UP) == KEY_UP)
    {
        buttonU();
    }
    if ((buttons & KEY_DOWN) == KEY_DOWN)
    {
        
        buttonD();
  
    }
}

void nextLevel(int level) {
  gameMode = LEVEL_MODE;
  currLevel = level;
  countBounce = 0;
  gameTimer = 0;
  collision = 0;
  init_coins();
}  

void gameLogicPs(void) {
  // Iterate over all coins
  for (int i=0;i<noCoins;i++) {
    if (checkCollisions(&gameItems2[0], coins[i])){
      coins[i].x = i*16;
      coins[i].y = 140;
      countBounce += 3;
      }
  }
  
  if (cooldownTimer > 0) {   
    cooldownTimer -= 1;
  }  
  
  gameTimer += 1;
  
  switch(currLevel) {
    case 1:
      if (countBounce > LEVEL_2) {
        nextLevel(2);
        break;
      }  
    case 2:
      break;
  }  
  
}

// any logic here runs at 24fps
void gameLogic(void) {
     // NO IDEA WHY THIS IS THE FIX BUT IT WORKS
  if (checkCollisions(&spoon, gameItems2[0])) {
     collision = 0;
   }  else if (checkCollisions(&spoon, gameItems2[1]))
   { collision = 1;
 } else {
   collision = -1;
 }    
 
   for (int i=0;i<currLevel;i++) {

       // While item is in the air
       if (gameItems2[i].y <= MaxY && gameItems2[i].dropped == 0) {
          gameItems2[i].vy > maxItemVy ? gameItems2[i].vy = maxItemVy : (gameItems2[i].vy += gameItems2[i].a);
  	   		gameItems2[i].y += gameItems2[i].vy/6;
  	   		gameItems2[i].x += gameItems2[i].vx/8;
       } else {
       // Item dropped
    	 gameItems2[i].y = MaxY;
    	 gameItems2[i].vx = 0;
    	 gameItems2[i].dropped = 1;
    	 gameMode = RESET_MODE;
      } 
    
     if (gameItems2[i].x >= 224) { //Item bounce off right screen
  		 gameItems2[i].x = 220;
  		 gameItems2[i].vx = -gameItems2[i].vx;
  		} 
     else if (gameItems2[i].x <= 0) { //Item bounce off left screen
  		   gameItems2[i].x = 0;
  		   gameItems2[i].vx = -gameItems2[i].vx;
  		} 

    }  
    
    
      // Player logic
    if (player.y < MaxY) {
      player.y += player.vy;
      player.vy += 1;
    } else {
      player.y = MaxY;
    }  
    
    // Spoon logic
    spoon.x = player.x + 16*player.dir;
}   


void fillPalette(void)
{
    int     i;

	// Fill the palette in GBA memory
    for (i = 0; i < NCOLS; i++)
        spritePal[i] = palette[i];
}


void fillSprites(void)
{
    int     i;

	// Load all sprites in GBA memory
    for (i = 0; i < 128*16*16; i++)
        spriteData[i] = (sprites[i*2+1] << 8) + sprites[i*2];

	// draw all sprites on screen, but all of them outside of the screen (starting at position (240,160) the bottom right corner of the GBA screen)
    for(i = 0; i < 128; i++)
        drawSprite(0, i, 240,160);
} 

void redrawFrame() {
  //for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);} //clear all sprites at the start of every frame;
   switch(player.dir) {
      case 1:
         {
            drawSprite(A_R_NF, 0, player.x, player.y);
            drawSprite(PLATFORM_R, 1, spoon.x, player.y-14);
            break;
         }
  		case -1:
  		   {
  			   drawSprite(A_L_NF, 0, player.x, player.y);
  			   drawSprite(PLATFORM_L, 1, spoon.x, player.y-14);
  			   break;
  			 }			   
       }
   // Draw bounce counter
   int res1 = NUMBERS + countBounce%10;
   int res2 = NUMBERS+ countBounce/10;
   drawSprite8(res1, 25, 232-8, 8);  
   drawSprite8(res2, 26, 232-16, 8);  
   
  // Draw game timer
   int dig1 = NUMBERS + (gameTimer/10)%10;
   int dig2 = NUMBERS+ gameTimer/100;
   drawSprite8(dig1, 27, 200-8, 8);  
   drawSprite8(dig2, 28, 200-16, 8);  
   
   
   // Draw items on screen
   for (int i=1;i<currLevel;i++) {
    drawSprite(10, i+50, gameItems2[i].x, gameItems2[i].y);   
   }   
   // Draw first item as a special item to interact with coins
   drawSprite(METEOR, 50, gameItems2[0].x, gameItems2[0].y); 
   
   // Draw coins
   for (int i=0;i<noCoins;i++) {
    drawSprite(ROCK_2, i+40, coins[i].x, coins[i].y);
   }   
   
   // Debug
   drawSprite8(NUMBERS+collision, 60, 140, 0);

   // Draw cooldown
   if (cooldownTimer > 0) {
     drawSprite(SATV_1, 70, 100, 8);
   } else {
     drawSprite(SATV_1, 70, 240, 160);
   }  
   
   // Draw level
   drawSprite8(NUMBERS+currLevel, 75, 60, 8);
 }  


