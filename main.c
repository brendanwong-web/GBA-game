// -----------------------------------------------------------------------------
// C-Skeleton to be used with HAM Library from www.ngine.de
// -----------------------------------------------------------------------------
// "numbers.h"

#include "gba.h"
#include "mygbalib.h"

void Handler(void)
{
    REG_IME = 0x00; // Stop all other interrupt handling, while we handle this current oness
    
    if ((REG_IF & INT_TIMER1) == INT_TIMER1)
    {
        checkbutton();
		
        switch (gameMode) {
            case PLAY_MODE:
                gameLogic();
                redrawFrame();
                break;

            case RESET_MODE:
                drawGameOver(); 
                break;

            case MENU_MODE:
                drawMenu();
                break;

            case PAUSE_MODE:
                pause(); //pauses
                break;

            case LEVEL_MODE:
                for(int j = 0; j < 128; j++){drawSprite(0, j, 240, 160);}
                int x = 50;
                int y = 60;
                int spacing = 12;
                // DIrty but works.
                if (currLevel==4){
                  drawSprite(WINWIN, 103, 50, 30);
                  drawSprite8(TILE_G, 90, x + spacing*0, y); 
                  drawSprite8(TILE_MENU_O, 91, x + spacing*1, y); 
                  drawSprite8(TILE_MENU_O, 92, x + spacing*2, y); 
                  drawSprite8(TILE_MENU_D, 93, x + spacing*3, y); 
                }  else {
                    
                        drawSprite8(TILE_MENU_P, 90, x + spacing*0, y); 
                        drawSprite8(TILE_MENU_R, 91, x + spacing*1, y); 
                        drawSprite8(TILE_MENU_E, 92, x + spacing*2, y); 
                        drawSprite8(TILE_MENU_S, 93, x + spacing*3, y); 
                        drawSprite8(TILE_MENU_S, 94, x + spacing*4, y); 
                      
                        drawSprite8(TILE_MENU_D, 104, x + spacing*6, y); 
                        drawSprite8(TILE_MENU_O, 96, x + spacing*7, y);
                        drawSprite8(TILE_MENU_W, 97, x + spacing*8, y); 
                        drawSprite8(TILE_MENU_N, 98, x + spacing*9, y); 
                        
                        drawSprite8(TILE_MENU_L, 99, x + spacing*0, y+20); 
                        drawSprite8(TILE_MENU_E, 100, x + spacing*1, y+20);
                        drawSprite8(TILE_MENU_V, 101, x + spacing*2, y+20); 
                        drawSprite8(TILE_MENU_E, 102, x + spacing*3, y+20);
                        drawSprite8(TILE_MENU_L, 103, x + spacing*4, y+20); 
                        drawSprite8(NUMBERS + currLevel, 95, x + spacing*6, y+20);
                  }  
                break;
        }
    }

    if ((REG_IF & INT_TIMER2) == INT_TIMER2)
    {
        // Inline assembly to increment gameTimer
        __asm__ volatile (
            "ldr r0, =gameTimer\n\t"   // Load address of gameTimer into r0
            "ldr r1, [r0]\n\t"         // Load current value of gameTimer into r1
            "add r1, r1, #1\n\t"       // Increment r1 by 1
            "str r1, [r0]\n\t"         // Store new value back to gameTimer
        );

        gameLogicPs();
    }

    if ((REG_IF & INT_TIMER3) == INT_TIMER3)
    {
        animate(2);
        animateItems(2); // animates items
        animateCoins(1);
        animateSpoon(1);
        for (int i=0;i<noCoins;i++){
          if ((gameTimer%5)==0){
        coins[i].frame = 1;
      }  
    }  
    }

    REG_IF = REG_IF; // Acknowledge interrupts
    REG_IME = 0x01;  // Re-enable interrupts
}

inline void vsync()
{
  while (REG_VCOUNT >= 160);
  while (REG_VCOUNT < 160);
}


// -----------------------------------------------------------------------------
// Project Entry Point
// -----------------------------------------------------------------------------
int main(void)
{   
 
    REG_DISPCNT = 0x40 | BG1_ENABLE | OBJ_ENABLE; // Set Mode 2 DO NOT CHANGE!!
    
	 fillSprites();
	 fillPalette();
	 gameMode = MENU_MODE; // Start game in menu modess
   init_player(&player);
	 init_spoon(&spoon);
	 init_items();
	 init_coins();
    // Set Handler Function for interrupts and enable selected interruptssss
    REG_INT = (int)&Handler;
    REG_IE |= INT_TIMER1 | INT_TIMER2 | INT_TIMER3;		// Enable Timers
    REG_IME = 0x01;		// Enable interrupt handling
    
    // Set Timer Mode (fill that section and replace TMX with selected timer number)
    REG_TM1D =	0xDE03;		// Runs game at approx 24fps
    REG_TM1CNT |= TIMER_FREQUENCY_64 | TIMER_ENABLE | TIMER_INTERRUPTS;
    
    REG_TM2D = 59100; //59100
    REG_TM2CNT |= TIMER_FREQUENCY_256 | TIMER_ENABLE | TIMER_INTERRUPTS; //This timer interrupts every 0.1s
    
    REG_TM3D = 60535; //stssss
    REG_TM3CNT |= TIMER_FREQUENCY_1024 | TIMER_ENABLE | TIMER_INTERRUPTS; //This timer interrupts every 0.1s
    
  	 while(1){
  	   vsync();
     }
}
