#include "sprites.h"
#define INPUT                      (KEY_MASK & (~REG_KEYS))
#define MaxY 140
extern int gameMode;

#define MENU_MODE        0
#define PLAY_MODE       1
#define RESET_MODE       2
int mode;
#define noItems 3
extern int gameMode;

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
int gameItemsx[noItems] = {80, 50, 30};
int gameItemsy[noItems] = {20, 40, 60};
int gameItemsvx[noItems] = {10, 5, 20};
int gameItemsvy[noItems] = {-20, -5, -10};
int gameItemsa[noItems] = {2, 1, 1};
struct gameItem gameItems2[noItems];

void init_player(struct gameCharacter* player) {
   player->x = 100;
   player->y = MaxY;
   player->dir = 1;
}

void init_spoon(struct gameItem* spoon) {
   spoon->x = player.x;
   spoon->y = player.y-16;
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

void reset_game() {
    mode = 1;
	 init_player(&player);
	 //init_item(&pancake, 100, 100, 10, -20);
	 init_spoon(&spoon);

}   

u8 checkCollisions(struct gameCharacter* item1, struct gameItem item) {
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
   for (int i=0;i<noItems;i++) {
      if (checkCollisions(&spoon, gameItems2[i])) {
         gameItems2[i].vy = -gameItems2[i].vy;
         int res = player.dir * gameItems2[i].vx;
         if (res < 0) {
            gameItems2[i].vx = -gameItems2[i].vx;
         }   
   	}
   }   
   
}

void buttonD() {
   reset_game();
}   

void drawSprite(int numb, int N, int x, int y)
{
	// Same as CA2, make specific sprite (based on its name/numb) appear on screen, as slide number N (each sprite needs a different, arbitrary, N >= 0)
    *(unsigned short *)(0x7000000 + 8*N) = y | 0x2000; // atr0 sets y as bits 0-7 and enables 8bpp, 
    *(unsigned short *)(0x7000002 + 8*N) = x | 0x4000; // atr1 sets x and enables sprite shape 16x16
    *(unsigned short *)(0x7000004 + 8*N) = numb*8; // atr2
}

void checkbutton(void)
{
	// Gift function to show you how a function that can be called upon button interrupt to detect which button was pressed and run a specific function for each button could look like. You would have to define each buttonA/buttonB/... function yourself.
    u16 buttons = INPUT;
    
    if ((buttons & KEY_A) == KEY_A)
    {
        // buttonA();
    }
    if ((buttons & KEY_B) == KEY_B)
    {
        // buttonB();
    }if ((buttons & KEY_SELECT) == KEY_SELECT)
    {
        // buttonSel();
    }
    if ((buttons & KEY_START) == KEY_START)
    {
        // buttonS();
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
        if (mode == 2) {
        buttonD();
     }   
    }
}


void gameLogicPs(void) {
   for (int i=0;i<noItems;i++) {
      if (gameItems2[i].y <= 140 && gameItems2[i].dropped == 0) {
	   		gameItems2[i].vy += gameItems2[i].a;
	   		gameItems2[i].y += gameItems2[i].vy/5;
		} else {
		   gameItems2[i].y = 142;
		   gameItems2[i].vx = 0;
			gameItems2[i].dropped = 1;
		}
		gameItems2[i].x += gameItems2[i].vx/6;
		if (gameItems2[i].x >= 224) {
		 gameItems2[i].x = 220;
		 gameItems2[i].vx = -gameItems2[i].vx;
		} else if (gameItems2[i].x <= 0) {
		   gameItems2[i].x = 0;
		   gameItems2[i].vx = -gameItems2[i].vx;
		}  
		if (gameItems2[i].dropped == 1) {
		   drawSprite(14, 5, 50, 50);
      	gameMode = 2;
			return;
		}   else {
      drawSprite(14, 5, 200, 160);
   }   
   }
		
		
}

void gameLogic(void) {
   
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
   switch(gameMode) {
      case 0: // valikko, jos teet sen myöhemmin
         break;
      case 1: // peli
         switch(player.dir) {
            case 1:
               drawSprite(0, 0, player.x, player.y);
               drawSprite(4, 4, spoon.x, spoon.y);
               break;
            case -1:
               drawSprite(2, 0, player.x, player.y);
               drawSprite(5, 4, spoon.x, spoon.y);
               break;
         }
         for (int i=0;i<noItems;i++) {
            drawSprite(10, i+5, gameItems2[i].x, gameItems2[i].y);  
            if (checkCollisions(&spoon, gameItems2[i])) {
                drawSprite(8, 3, 0, 0);
            } else {
                drawSprite(8, 3, 240, 160);
            }
         }
         break;

      case 2: // game over
         drawGameOver(); // ?? tähän se pitää tulla!
         break;
   }
}
