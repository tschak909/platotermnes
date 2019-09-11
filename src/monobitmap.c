/**
 * PLATOTERM NES - Mono bitmap functions
 */

#include "monobitmap.h"
#include <nes.h>
#include "neslib.h"
#include <stdlib.h>

bool ppu_is_on = false;

// simple 6502 delay loop (5 cycles per loop)
#define DELAYLOOP(n) \
  __asm__("ldy #%b", n); \
  __asm__("@1: dey"); \
  __asm__("bne @1");

// call every frame to split screen
void monobitmap_split() {
  // split screen at line 128
  split(0,0);
  DELAYLOOP(15); // delay until end of line
  PPU.control = PPU.control ^ 0x10; // bg bank 1
}

// set a pixel at (x,y) color 1=set, 0=clear
void monobitmap_set_pixel(int x, int y, unsigned char color) {
  unsigned char b;
  // compute pattern table address
  word a = (x/8)*16 | ((y&63)/8)*(16*32) | (y&7);
  if (y & 64) a |= 8;
  if (y & 128) a |= 0x1000;
  // if PPU is active, wait for next frame
  if (ppu_is_on) {
    ppu_wait_nmi();
  }
  // read old byte
  vram_adr(a);
  vram_read(&b, 1);
  if (color) {
    b |= 128 >> (x&7); // set pixel
  } else {
    b &= ~(128 >> (x&7)); // clear pixel
  }
  // write new byte
  vram_adr(a);
  vram_put(b);
  // if PPU is active, reset PPU addr and split screen
  if (ppu_is_on) {
    vram_adr(0);
    monobitmap_split();
  }
}

// draw a line from (x0,y0) to (x1,y1)
void monobitmap_draw_line(int x1, int y1, int x2, int y2, unsigned char color) {
  int dx, dy;
  int stepx, stepy;

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx < 0)
    {
      dx = -dx;
      stepx = -1;
    }
  else
    {
    stepx = 1;
    }
  
  if (dy < 0)
    {
      dy = -dy;
      stepy = -1;
    }
  else
    {
      stepy = 1;
    }
  dx <<= 1;
  dy <<= 1;
  
  // draw first point 
  monobitmap_set_pixel (x1, y1,color);
  
  // check for shallow line

  if (dx > dy)
    {
      int fraction = dy - (dx >> 1);
      while (x1 != x2)
	{
	  if (fraction >= 0)
	    {
	      y1 += stepy;
	      fraction -= dx;
	    }
	  x1 += stepx;
	  fraction += dy;
	  monobitmap_set_pixel (x1, y1, color);
	}
    } // otherwise steep line 
  else
    {
      int fraction = dx - (dy >> 1);
      while (y1 != y2)
	{
	  if (fraction >= 0)
	    {
	      x1 += stepx;
	      fraction -= dy;
	    }
	  y1 += stepy;
	  fraction += dx;
	  monobitmap_set_pixel (x1, y1,color);
	}
    }
}

// write values 0..255 to nametable
void monobitmap_put_256inc() {
  word i;
  for (i=0; i<256; i++)
    vram_put(i);
}

// sets up attribute table
void monobitmap_put_attrib() {
  vram_fill(0x00, 0x10); // first palette
  vram_fill(0x55, 0x10); // second palette
}

// clears pattern table
void monobitmap_clear() {
  vram_adr(0x0);
  vram_fill(0x0, 0x2000);
}

// sets up PPU for monochrome bitmap
void monobitmap_setup() {
  monobitmap_clear();
  // setup nametable A and B
  vram_adr(NAMETABLE_A);
  monobitmap_put_256inc();
  monobitmap_put_256inc();
  monobitmap_put_256inc();
  monobitmap_put_256inc();
  vram_adr(NAMETABLE_A + 0x3c0);
  monobitmap_put_attrib();
  monobitmap_put_attrib();
  bank_bg(0);
  // setup sprite 0
  bank_spr(1);
  oam_clear();
  oam_size(0);
  oam_spr(247, 125, 255, 0, 0);
  // draw a pixel for it to collide with
  monobitmap_set_pixel(247, 126, 1);
  // make sprite 255 = white line
  vram_adr(0x1ff0);
  vram_fill(0xff, 0x1);
}
