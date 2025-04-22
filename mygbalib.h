#include "sprites.h"
#define INPUT                      (KEY_MASK & (~REG_KEYS))
#define MaxY 140
extern int gameMode;

#define MENU_MODE        0
#define PLAY_MODE       1
#define RESET_MODE       2
#define noItems 3
#define cooldown 10
#define noCoins 4
#define jumpheight 6
#define maxItemVy 20
int cooldownTimer = 0;
int gameTimer = 0;
int gameMode;
int currLevel = 1;
int countBounce = 0;
int collision = 0;

void drawSprite(int numb, int N, int x, int y)
{
	// Same as CA2, make specific sprite (based on its name/numb) appear on screen, as slide number N (each sprite needs a different, arbitrary, N >= 0)
    *(unsigned short *)(0x7000000 + 8*N) = y | 0x2000; // atr0 sets y as bits 0-7 and enables 8bpp, 
    *(unsigned short *)(0x7000002 + 8*N) = x | 0x4000; // atr1 sets x and enables sprite shape 16x16
    *(unsigned short *)(0x7000004 + 8*N) = numb*8; // atr2
}

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

struct gameCharacter player;
struct gameItem spoon;
struct gameItem gameItems2[noItems];
struct gameItem coins[noCoins];

// Init item positions
int gameItemsx[noItems] = {80, 50, 30};
int gameItemsy[noItems] = {20, 40, 60};
int gameItemsvx[noItems] = {10, 5, 20};
int gameItemsvy[noItems] = {-20, -5, -10};
int gameItemsa[noItems] = {2, 1, 1};

// Init coin positions
int coinsx[noCoins] = {20, 40, 220, 200};
int coinsy[noCoins] = {20, 40, 60, 80};


void init_player(struct gameCharacter* player) {
   player->x = 100;
   player->y = MaxY;
   player->dir = 1;
}

void init_spoon(struct gameItem* spoon) {
   spoon->x = player.x;
   spoon->y = player.y-20;
   spoon->dropped = 0;
   spoon->vx = 0;
   spoon->vy = 0;
   spoon->a = 0;
}

void init_item(struct gameItem* item,int x,int y,int vx,int vy) {
   item->vy = vy;
	item->vx = vx;
	item->a = 2;
	item->x = x;
	item->y = x;
	item->dropped = 0;
}

void init_items() {
	 for (int i=0;i<=currLevel;i++) {
	   gameItems2[i].x = gameItemsx[i];
	   gameItems2[i].y = gameItemsy[i];
	   gameItems2[i].vx = gameItemsvx[i];
	   gameItems2[i].vy = gameItemsvy[i];
	   gameItems2[i].a = gameItemsa[i];
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

void init_coins() {
   int i;
   for (i=0;i<noCoins;i++) {
      coins[i].x = coinsx[i];
      coins[i].y = coinsy[i];
   }  
}  

void drawSprite8(int numb, int N, int x, int y)
{
    // Gift function: displays sprite number numb on screen at position (x,y), as sprite object N
    *(unsigned short *)(0x7000000 + 8*N) = y | 0x2000;
    *(unsigned short *)(0x7000002 + 8*N) = x;
    *(unsigned short *)(0x7000004 + 8*N) = numb*2;
}

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
    
}  

void reset_game() {
   gameMode = PLAY_MODE;
   countBounce = 0;
	 init_player(&player);
	 //init_item(&pancake, 100, 100, 10, -20);
	 init_spoon(&spoon);
	 init_items();
    drawSprite8(TILE_G, 90, 240, 160); // G
    drawSprite8(TILE_A, 91, 240, 160); // A
    drawSprite8(TILE_M, 92, 240, 160); // M
    drawSprite8(TILE_E, 93, 240, 160); // E
    drawSprite8(TILE_O, 94, 240, 160); // O (väli jälkeen E)
    drawSprite8(TILE_V, 95, 240, 160); // V
    drawSprite8(TILE_E, 96, 240, 160); // E again
    drawSprite8(TILE_R, 97, 240, 160); // R
}   

u8 checkCollisions(struct gameCharacter* item1, struct gameItem item) {
  return (item.x >= item1->x && item.x <= item1->x + 16) &&
  (item.y >= item1->y-16 && item.y <= item1->y + 16) ||
  (item1->x >= item.x && item1->x <= item.x + 16) &&
    (item1->y >= item.y-16 && item1->y <= item.y + 16);
	}
	
u8 checkCollisionsOld(struct gameCharacter* item1, struct gameItem item) {
  return (item.x >= item1->x && item.x <= item1->x + 16) &&
  (item.y >= item1->y-12 && item.y <= item1->y + 4) ||
  (item1->x >= item.x && item1->x <= item.x + 16) &&
    (item.y >= item1->y-12 && item.y <= item1->y + 4);
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

}   

void buttonS() { 
  reset_game();
}   

void buttonA() {
   if (player.y < MaxY) {return;}
   player.y -= 10;
   player.vy = -jumpheight;
}  

void buttonB() {
    if (cooldownTimer != 0) {return;}
   cooldownTimer = cooldown;
   for (int i=0;i<currLevel;i++) {
      if (collision != 0) {
         gameItems2[collision-1].vy = -40;
         if (player.dir * gameItems2[collision-1].vx < 0) {
            gameItems2[collision-1].vx = -gameItems2[collision-1].vx;
         }   
      countBounce += 1;
   	}
   }  
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
        // buttonSel();
    }
    if ((buttons & KEY_START) == KEY_START)
    {
      if (gameMode == 2) {
        buttonS(); // Enter
      }  
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
        
        //buttonD();
  
    }
}


void gameLogicPs(void) {
  // Iterate over all items
   for (int i=0;i<currLevel;i++) {
     // While item is in the air
     if (gameItems2[i].y <= 140 && gameItems2[i].dropped == 0) {
        gameItems2[i].vy > maxItemVy ? gameItems2[i].vy = 15 : (gameItems2[i].vy += gameItems2[i].a);
	   		gameItems2[i].y += gameItems2[i].vy/5;
	   		gameItems2[i].x += gameItems2[i].vx/6;
     } else {
     // Item dropped
  	 gameItems2[i].y = 142;
  	 gameItems2[i].vx = 0;
  	 gameItems2[i].dropped = 1;
    } 
  }  
  // Iterate over all coins
  for (int i=0;i<noCoins;i++) {
    if (checkCollisions(&gameItems2[0], coins[i])){
      coins[i].x = i*16;
      coins[i].y = 140;
      }
  }
  // Player logic
  if (player.y < MaxY) {
    player.y += player.vy;
    player.vy += 1;
  } else {
    player.y = MaxY;
  }  
    
  if (cooldownTimer > 0) {   
    cooldownTimer -= 1;
  }  
  gameTimer += 1;
}

void gameLogic(void) {
  // any logic here runs at 24fps
   for (int i=0;i<currLevel;i++) {
     if (gameItems2[i].x >= 224) { //Item bounce off right screen
  		 gameItems2[i].x = 220;
  		 gameItems2[i].vx = -gameItems2[i].vx;
  		} 
      else if (gameItems2[i].x <= 0) { //Item bounce off left screen
  		   gameItems2[i].x = 0;
  		   gameItems2[i].vx = -gameItems2[i].vx;
  		} 
  		
  		// Check if item is dropped on the ground
  		if (gameItems2[i].dropped == 1) {
        	gameMode = RESET_MODE;
      }  
      
      if (checkCollisions(&spoon, gameItems2[i])) {
        collision = i+1;
      }  
      } 
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
            drawSprite(0, 0, player.x, player.y);
            drawSprite(4, 4, spoon.x, player.y-20);
            break;
         }
		case -1:
		   {
			   drawSprite(2, 0, player.x, player.y);
			   drawSprite(5, 4, spoon.x, player.y-20);
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
   for (int i=1;i<noItems;i++) {
    drawSprite(10, i+5, gameItems2[i].x, gameItems2[i].y);   
   }   
   // Draw first item as a special item to interact with coins
   drawSprite(METEOR, 5, gameItems2[0].x, gameItems2[0].y); 
   
   // Draw coins
   for (int i=0;i<noCoins;i++) {
    drawSprite(ROCK_2, i+40, coins[i].x, coins[i].y);   
   }   
   
   // Debug
   for (int i=0;i<currLevel;i++) {
        if (checkCollisions(&spoon, gameItems2[i])) {
          drawSprite(LIFE_1, 95, 100, 0);
        } else {
         drawSprite(LIFE_1, 95, 240, 160);
        }  
   }  

   // Draw cooldown
   if (cooldownTimer > 0) {
     drawSprite(SATV_1, 70, 100, 8);
   } else {
     drawSprite(SATV_1, 70, 240, 160);
   }  
 }  


