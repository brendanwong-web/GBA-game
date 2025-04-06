// -----------------------------------------------------------------------------
// C-Skeleton to be used with HAM Library from www.ngine.de
// -----------------------------------------------------------------------------
// "numbers.h"
#include "gba.h"
#include "mygbalib.h"


// define all global variables like player positions here
u16 key_curr = 0, key_prev = 0;

// Game runs on timer interrupts

void drawSprite8(int numb, int N, int x, int y)
{
    // Gift function: displays sprite number numb on screen at position (x,y), as sprite object N
    *(unsigned short *)(0x7000000 + 8*N) = y | 0x2000;
    *(unsigned short *)(0x7000002 + 8*N) = x;
    *(unsigned short *)(0x7000004 + 8*N) = numb*2;
}
 
void key_poll()
{
    key_prev = key_curr;
    key_curr= INPUT;
}

// Basic state checas
u32 key_curr_state()         {   return key_curr;          }
u32 key_prev_state()         {   return key_prev;          }
u32 key_is_down(u32 key)     {   return key_curr & key;   }
u32 key_is_up(u32 key)       {   return ~key_curr & key;   }
u32 key_was_down(u32 key)    {   return  key_prev & key;   }
u32 key_was_up(u32 key)      {   return ~key_prev & key;   }

void vid_vsync()
{
   while(REG_VCOUNT >= 160);   // wait till VDraw
   while(REG_VCOUNT < 160);    // wait till VBlank
}

void Handler(void)
{
    REG_IME = 0x00; // Stop all other interrupt handling, while we handle this current one
    if ((REG_IF & INT_TIMER1) == INT_TIMER1) // TODO: replace XXX with the specific interrupt you are handling
    {
       // update variables as functions
       // key_pollst();
       checkbutton();
       gameLogic();
       redrawFrame();
    }
    
    if ((REG_IF & INT_TIMER2) == INT_TIMER2) // TODO: replace XXX with the specific interrupt you are handling
    {
       // update variables as functions
       // key_poll();
       gameLogicPs();
    }
    
    
    REG_IF = REG_IF; // Update interrupt table, to confirm we have handled this interrupt
    
    REG_IME = 0x01;  // Re-enable interrupt handling
}


// -----------------------------------------------------------------------------
// Project Entry Point
// -----------------------------------------------------------------------------
int main(void)
{

	
    // Set Mode 2 DO NOT CHANGE!!
    *(unsigned short *) 0x4000000 = 0x40 | 0x2 | 0x1000;
	 fillSprites();
	 fillPalette();
	 init_player(&player);
	 init_spoon(&spoon);
	 for (int i=0;i<noItems;i++) {
	   gameItems2[i].x = gameItemsx[i];
	   gameItems2[i].y = gameItemsy[i];
	   gameItems2[i].vx = gameItemsvx[i];
	   gameItems2[i].vy = gameItemsvy[i];
	   gameItems2[i].a = gameItemsa[i];
	   int dropped = 0;
	 }   

	 //tsstgt]sss
    // Set Handler Function for interrupts and enable selected interrupts
    REG_INT = (int)&Handler;
    REG_IE |= INT_TIMER1 | INT_TIMER2;		// Enable Timer 2
    REG_IME = 0x01;		// Enable interrupt handling
    // Set Timer Mode (fill that section and replace TMX with selected timer number)
    REG_TM1D =	0xDE03;		// Runs game at approx 24fps
    REG_TM1CNT |= TIMER_FREQUENCY_64 | TIMER_ENABLE | TIMER_INTERRUPTS;
    
    REG_TM2D = 55535;
    REG_TM2CNT |= TIMER_FREQUENCY_256 | TIMER_ENABLE | TIMER_INTERRUPTS;
    while(1) {
	    vid_vsync();
		 };

	return 0;
} 

