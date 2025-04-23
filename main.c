// -----------------------------------------------------------------------------
// C-Skeleton to be used with HAM Library from www.ngine.de
// -----------------------------------------------------------------------------
// "numbers.h"
#include "gba.h"
#include "mygbalib.h"


void Handler(void)
{
    REG_IME = 0x00; // Stop all other interrupt handling, while we handle this current one
    
    if ((REG_IF & INT_TIMER1) == INT_TIMER1) // TODO: replace XXX with the specific interrupt you are handling
    {
       checkbutton();
       gameLogic();
       // Check modess
       switch (gameMode) {
         case PLAY_MODE: {
           for(int j = 0; j < 128; j++){drawSprite(0, j, 240,160);}
           redrawFrame();
           break;
         }  
         case RESET_MODE: {
           drawGameOver(); 
           break;
         }  
         case MENU_MODE: {
           drawMenu();
           break ;
         }  
         case PAUSE_MODE: {
           pause(); //pauses
           break;
         }  
       }  

    }
    
    if ((REG_IF & INT_TIMER2) == INT_TIMER2) // TODO: replace XXX with the specific interrupt you are handling
    {
       // key_poll();
       gameLogicPs();
    }
    
    
    REG_IF = REG_IF; // Update interrupt table, to confirm we have handled this interrupt
    
    REG_IME = 0x01;  // Re-enable interrupt handlings
}


// -----------------------------------------------------------------------------
// Project Entry Point
// -----------------------------------------------------------------------------
int main(void)
{   
    // test comments
 
    *(unsigned short *) 0x4000000 = 0x40 | 0x2 | 0x1000; // Set Mode 2 DO NOT CHANGE!!
    
	 fillSprites();
	 fillPalette();
	 gameMode = MENU_MODE; // Start game in play mode
	 init_player(&player);
	 init_spoon(&spoon); 
   init_items();   
   init_coins(); //init coinss
    // Set Handler Function for interrupts and enable selected interrupts
    REG_INT = (int)&Handler;
    REG_IE |= INT_TIMER1 | INT_TIMER2;		// Enable Timer 2
    REG_IME = 0x01;		// Enable interrupt handling
    // Set Timer Mode (fill that section and replace TMX with selected timer number)
    REG_TM1D =	0xDE03;		// Runs game at approx 24fps
    REG_TM1CNT |= TIMER_FREQUENCY_64 | TIMER_ENABLE | TIMER_INTERRUPTS;
    
    REG_TM2D = 59100;
    REG_TM2CNT |= TIMER_FREQUENCY_256 | TIMER_ENABLE | TIMER_INTERRUPTS; //This timer interrupts every 0.1s
  	 while(1){
     }
}
