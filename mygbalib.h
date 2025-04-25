#include "sprites.h"
#define INPUT                      (KEY_MASK & (~REG_KEYS))

#define MaxY 130
int gameTimer = 0;
int gameMode;
int currLevel = 1;
int countBounce = 0;
int collision = -1;
#define LEVEL_2 10
#define LEVEL_3 20
#define WIN 30


typedef struct gameCharacter {
   int x;
   int y;
   int vx;
   int vy;
   int dir;
   int frame;
};

typedef struct gameItem {
   int x;
   int y;
   int vx;
   int vy;
   int a;
   int dropped;
   int frame;
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
#define dash 10
int dashTimer = 0;

void init_player(struct gameCharacter* player) {
   player->x = 100;
   player->y = MaxY;
   player->dir = 1;
   player->frame = 0;
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
   spoon->frame = 0;
}
// End Spoon


// Coins: No coins is based on currLevel
#define noCoins 6
int coinsPos[noCoins][2] = {{20,30}, {220, 30}, {60, 70}, {170, 70}, {50, 100}, {200, 100}};
struct gameItem coins[noCoins];

void init_coins() {
   int i;
   for (i=0;i<noCoins;i++) {
     if (i<currLevel*2) {
     coins[i].x = coinsPos[i][0];
     coins[i].y = coinsPos[i][1];
     } else {
       coins[i].x = 240;
       coins[i].y = 160;
     }  
   coins[i].frame = 0;
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
  {170, 10, -10, -30} // item 2
  };

void init_items() {
	 for (int i=0;i<noItems;i++) {
	   if (i<currLevel) {
	   gameItems2[i].x = gameItemsC[i][0];
	   gameItems2[i].y = gameItemsC[i][1];
	   gameItems2[i].vx = gameItemsC[i][2];
	   gameItems2[i].vy = gameItemsC[i][3];
	   gameItems2[i].a = 1;
	 } else {
     gameItems2[i].x = 240;
	   gameItems2[i].y = 160;
	   gameItems2[i].vx = 0;
	   gameItems2[i].vy = 0;
	   gameItems2[i].a = 0;
   }    
	   gameItems2[i].dropped = 0;
	   gameItems2[i].frame = 0;
	 }
} 

// End Items



void drawGameOver() {
    int x = 60;
    int y = 8;
    int spacing = 12;

    drawSprite8(TILE_G, 58, x + spacing*0, y); // G
    drawSprite8(TILE_A, 59, x + spacing*1, y); // A
    drawSprite8(TILE_M, 60, x + spacing*2, y); // M
    drawSprite8(TILE_E, 53, x + spacing*3, y); // E
    drawSprite8(TILE_O, 54, x + spacing*5, y); // O (väli jälkeen E)
    drawSprite8(TILE_V, 55, x + spacing*6, y); // V
    drawSprite8(TILE_E, 56, x + spacing*7, y); // E again
    drawSprite8(TILE_R, 57, x + spacing*8, y); // R
}

void drawMenu() {
    for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
    int y = 100;
    int spacing = 12;
    int x = (240-spacing*10-8)/2;


    drawSprite8(TILE_MENU_P, 90, x + spacing*0, y); // P
    drawSprite8(TILE_MENU_R, 91, x + spacing*1, y); // R
    drawSprite8(TILE_MENU_E, 92, x + spacing*2, y); // E
    drawSprite8(TILE_MENU_S, 93, x + spacing*3, y); // S
    drawSprite8(TILE_MENU_S, 94, x + spacing*4, y); // S
    
    drawSprite8(TILE_MENU_S, 95, x + spacing*6, y); // P
    drawSprite8(TILE_MENU_T, 96, x + spacing*7, y); // R
    drawSprite8(TILE_MENU_A, 97, x + spacing*8, y); // E
    drawSprite8(TILE_MENU_R, 98, x + spacing*9, y); // S
    drawSprite8(TILE_MENU_T, 99, x + spacing*10, y); // S
}  

void pause() {
    gameMode = PAUSE_MODE;
    int x = 60;
    int y = 6;
    int spacing = 12;

    drawSprite8(TILE_MENU_P, 55, x + spacing*0, y); // G
    drawSprite8(TILE_MENU_A, 56, x + spacing*1, y); // A
    drawSprite8(TILE_MENU_U, 57, x + spacing*2, y); // M
    drawSprite8(TILE_MENU_S, 58, x + spacing*3, y); // E
    drawSprite8(TILE_MENU_E, 59, x + spacing*4, y); // O (väli jälkeen E)
    drawSprite8(TILE_MENU_D, 60, x + spacing*5, y); // 
}  

void reset_game() {
   for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
   collision = -1;
   countBounce = 0;
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
      player.x+=3;
}   

 
void buttonL() {
   if (player.x <= 1) {
      return;
   } 
   player.dir = -1;
 	player.x-=3;
} 

void buttonU() {
   
}

void buttonD() {
  switch(gameMode) {
    case LEVEL_MODE:
      reset_game();
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
      for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
     gameMode = PLAY_MODE;
   }  

}  


void buttonA() {
  if (dashTimer != 0) {return;}
     player.x += 30*player.dir;
     dashTimer = dash;
     player.frame+=1;
}  

void buttonB() {
   if (cooldownTimer != 0) {return;}
   if (collision > -1) {
      gameItems2[collision].vy = -40;
      gameItems2[collision].frame += 1;
      if (player.dir * gameItems2[collision].vx < 0) {
        gameItems2[collision].vx = -gameItems2[collision].vx;
   	     }
   	  countBounce += 1;
   }
    cooldownTimer = cooldown;
    spoon.frame += 1;
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
      coins[i].x = 17+i*16;
      coins[i].y = 140;
      countBounce += 3;
      }
      
  }
  
  if (cooldownTimer > 0) {   
    cooldownTimer -= 1;
  }  
  
  if (dashTimer > 0) {
    dashTimer -= 1;
  }
  
  gameTimer += 1;


  
  switch(currLevel) {
    case 1:
      if (countBounce > LEVEL_2) {
        nextLevel(2);
        break;
      }  
    case 2:
       if (countBounce > LEVEL_3) {
        nextLevel(3);
        break;
      }  
    case 3:
      if (countBounce > WIN) {
        nextLevel(4);
        break;
      }  
  }  
  
}

// any logic here runs at 24fps
void gameLogic(void) {
     // NO IDEA WHY THIS IS THE FIX BUT IT WORKS
  if (checkCollisions(&spoon, gameItems2[0])) {
     collision = 0;
   }  else if (checkCollisions(&spoon, gameItems2[1]))
   { collision = 1;
 } else if (checkCollisions(&spoon, gameItems2[2]))
   { collision = 2;
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
    player.x = player.x > 224 ? 224 : player.x;
    player.x = player.x < 16 ? 16: player.x;
    
    
    // Spoon logic
    spoon.x = player.x + 16*player.dir;
}   

void animate(int frames) {
  if (player.frame == 0) {
    return;
  } else if (player.frame == frames){  
    player.frame = 0;
    return;
  }  
    player.frame += 1;    
}  

void animateSpoon(int frames) {
  if (spoon.frame == 0) {
    return;
  } else if (spoon.frame == frames){
    spoon.frame = 0;
    return;
  }  
    spoon.frame += 1;    
}  

void animateItems(int frames) {
  int i;
  for (i=0;i<currLevel;i++){
    if (gameItems2[i].frame == 0) {
      continue;
    } else if (gameItems2[i].frame == frames) {
      gameItems2[i].frame = 0;
      continue;
    }  
    gameItems2[i].frame += 1;
    
  }  
  
}  
void animateCoins(int frames) {
  int i;
  for (i=0;i<noCoins;i++){
    if (coins[i].frame == 0) {
      continue;
    } else if (coins[i].frame == frames) {
      coins[i].frame = 0;
      continue;
    }  
    coins[i].frame += 1;
  }  
}  

void fillPalette(void)
{
    int     i;

	// Fill the palette in GBA memory
    for (i = 0; i < NCOLS; i++)
        spritePal[i] = palette[i];
        bgPal[i] = palette[i];     
}


void fillSprites(void)
{
    int     i;
    int     j;

	// Load all sprites in GBA memory
    for (i = 0; i < 128*16*16; i++) {
        spriteData[i] = (sprites[i*2+1] << 8) + sprites[i*2];      
    }  

	// draw all sprites on screen, but all of them outside of the screen (starting at position (240,160) the bottom right corner of the GBA screen)
    for(i = 0; i < 128; i++) {
        drawSprite(0, i, 240,160);
    }  
} 

void redrawFrame() {
  //for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);} //clear all sprites at the start of every frame;

   int textSpacing = 10;
   switch(player.dir) {
      case 1:
         {
            drawSprite(CHEF_R+player.frame, 0, player.x, player.y);
            drawSprite(SPOON_R+spoon.frame, 1, spoon.x, player.y-14);
            break;
         }
  		case -1:
  		   {
  			   drawSprite(CHEF_L+player.frame, 0, player.x, player.y);
  			   drawSprite(SPOON_L+spoon.frame, 1, spoon.x, player.y-14);
  			   break;
  			 }			   
       }
   // Draw bounce counter
   int scorex = 170;
   drawSprite8(TILE_MENU_S, 98, scorex+textSpacing*0, 7); 
   drawSprite8(TILE_MENU_C, 99, scorex+textSpacing*1, 7); 
   drawSprite8(TILE_MENU_O, 95, scorex+textSpacing*2, 7); 
   drawSprite8(TILE_MENU_R, 96, scorex+textSpacing*3, 7); 
   drawSprite8(TILE_MENU_E, 97, scorex+textSpacing*4, 7); 
   int res1 = NUMBERS + countBounce%10;
   int res2 = NUMBERS+ countBounce/10;
   drawSprite8(res1, 3, scorex + textSpacing*5 + 8, 6);  
   drawSprite8(res2, 4, scorex + textSpacing*5, 6);  
   
  // Draw game timer
   int timerx = 6;
   int dig1 = NUMBERS + (gameTimer/10)%10;
   int dig2 = NUMBERS+ gameTimer/100;
   drawSprite(HOURGLASS, 112, timerx, 6);
   drawSprite8(dig1, 5, timerx+24, 10);  
   drawSprite8(dig2, 6, timerx+16, 10);  
   
   
   // Draw items on screen
   for (int i=1;i<currLevel;i++) {
    drawSprite(PANCAKE+gameItems2[i].frame, i+10, gameItems2[i].x, gameItems2[i].y);   
   }   
   // Draw first item as a special item to interact with coins
   drawSprite(EGG+gameItems2[0].frame, 10, gameItems2[0].x, gameItems2[0].y); 
   
   // Draw coins
   for (int i=0;i<noCoins;i++) {
    drawSprite(COIN+coins[i].frame, 100+i, coins[i].x, coins[i].y);
   }   
   
   // Debug
   if (collision >-1) {
   drawSprite8(CIRCLE, 19, player.x+4, player.y-20);
   } else {
        drawSprite8(CIRCLE, 19,126, 160);
   }    
   // Draw cooldown
   if (cooldownTimer > 0) {
     drawSprite(SNOWFLAKE, 7, 60, 7);
   } else {
     drawSprite(SNOWFLAKE, 7, 240, 160);
   }  
   
   if (dashTimer > 0) {
     drawSprite(LIGHTNING, 8, 80, 7);
   } else {
     drawSprite(LIGHTNING, 8, 240, 160);
   }  
   
   // draw floor
    for (int i=0;i<240/16;i++) {
        drawSprite(TILE_WOOD, 113+i, i*16, 144);
   } 
   
   for (int i=0;i<currLevel*2;i++){
     drawSprite(BOXBOX, 107+i, 16+i*16, 140);
   }  
   
   
   // Draw level
   int textx = 190;
   int texty = 18;
   drawSprite8(TILE_MENU_L, 90, textx+textSpacing*0, texty); 
   drawSprite8(TILE_MENU_V, 91, textx+textSpacing*1, texty); 
   drawSprite8(TILE_MENU_L, 92, textx+textSpacing*2, texty); 
   drawSprite8(NUMBERS+currLevel, 9, textx+textSpacing*4, texty);
 }  


