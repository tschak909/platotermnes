/**
 * PLATOTERM for NES
 *
 * Main Functions
 */

#include <nes.h>
#include "neslib.h"
#include <stdlib.h>
#include "screen.h"
#include "touch.h"
#include "terminal.h"
#include "io.h"
#include "keyboard.h"
#include "splash.h"

#define NES_MAPPER 2		// UxROM mapper
#define NES_CHR_BANKS 0		// CHR RAM

unsigned char already_started=false;

void main(void)
{
  screen_init();
  touch_init();
  ShowPLATO((padByte *)splash, sizeof(splash));
  terminal_initial_position();
  io_init();
  for (;;)
    {
      screen_main();
      keyboard_main();
      io_main();
      touch_main();
    }
}
