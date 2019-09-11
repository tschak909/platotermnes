
#include "touch.h"

#define SPR_MOUSE0 0
#define SPR_MOUSE1 1

#pragma warn (unused-param, off)

unsigned int pointerx;
unsigned int pointery;
int touch_enabled;
int left_button_pressed;
padPt touchCoord;

void touch_sprite_pos(unsigned int n, unsigned int r, unsigned int c)
{
}

/**
 * touch_init() - Set up touch screen
 */
void touch_init(void)
{
}

/* Multiply by 2.(6) = 8/3 */
unsigned int touch_scale_y(unsigned int y)
{
  return 0;
}

/* Multiply by 2 = n<<4 for TI99 */
unsigned int touch_scale_x(unsigned int x)
{
  return 0;
}

/**
 * touch_main() - Main loop for touch screen
 */
void touch_main(void)
{
}

/**
 * touch_allow - Set whether touchpanel is active or not.
 */
void touch_allow(padBool allow)
{
}

/**
 * touch_done() - Stop the mouse driver
 */
void touch_done(void)
{
}
