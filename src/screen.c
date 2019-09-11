#include "font.h"
#include "protocol.h"
#include "io.h"
#include "neslib.h"
#include <nes.h>
#include "monobitmap.h"
#include <stdlib.h>

#pragma warn (unused-param, off)

unsigned char CharWide=8;
unsigned char CharHigh=16;
padPt TTYLoc;
padPt statusLoc={0,0};
unsigned char current_foreground=1;
unsigned char current_background=0;
extern padBool FastText; /* protocol.c */
extern const unsigned char font[];
extern unsigned char fontm23[];
extern unsigned char FONT_SIZE_X;
extern unsigned char FONT_SIZE_Y;
extern bool ppu_is_on;
extern uint16_t mul0375(uint16_t val);
#define mul05(a) ((a>>1)+12)

#define scalex(x) mul05(x)
#define scaley(y) mul0375(((y)^0x1FF)&0x1FF)+32

// Miscellaneous math functions needed for coordinate translation.
short max(short a, short b) { return ( a > b ) ? a : b; }
short min(short a, short b) { return ( a < b ) ? a : b; }


#define FONTPTR(a) (a*6)

/*{pal:"nes",layout:"nes"}*/
const byte MONOBMP_PALETTE[16] = {
  0x03,
  0x30, 0x03, 0x30,  0x00,
  0x03, 0x30, 0x30,  0x00,
  0x30, 0x03, 0x30,  0x00,
  0x03, 0x30, 0x30
};



/**
 * screen_init() - Set up the screen
 */
void screen_init(void)
{
  monobitmap_setup();
  pal_bg(MONOBMP_PALETTE);
  ppu_is_on=true;
  ppu_on_all();
}

/**
 * screen_wait(void) - Sleep for approx 16.67ms
 */
void screen_wait(void)
{
}

/**
 * screen_main(void) - Main wait loop
 */
void screen_main(void)
{
  ppu_wait_nmi();
  monobitmap_split();
}

/**
 * screen_beep(void) - Beep the terminal
 */
void screen_beep(void)
{
}

/**
 * screen_clear - Clear the screen
 */
void screen_clear(void)
{
  monobitmap_setup();
}

/**
 * screen_set_pen_mode(void) - Set the current pen mode
 */
void screen_set_pen_mode(void)
{
}

/**
 * screen_block_draw(Coord1, Coord2) - Perform a block fill from Coord1 to Coord2
 */
void screen_block_draw(padPt* Coord1, padPt* Coord2)
{
  unsigned char x1=min(scalex(Coord1->x),scalex(Coord2->x));
  unsigned char x2=max(scalex(Coord1->x),scalex(Coord2->x));
  unsigned char y1=min(scaley(Coord1->y),scaley(Coord2->y));
  unsigned char y2=max(scaley(Coord1->y),scaley(Coord2->y));
  unsigned char y;
  unsigned char c=((CurMode==ModeInverse||CurMode==ModeErase) ? 0 : 1);  

  for(y=y1;y<y2;y++)
    monobitmap_draw_line(x1,y,x2,y,c);
}

/**
 * screen_dot_draw(Coord) - Plot a mode 0 pixel
 */
void screen_dot_draw(padPt* Coord)
{
  unsigned char c=((CurMode==ModeInverse||CurMode==ModeErase) ? 0 : 1);

  monobitmap_set_pixel(scalex(Coord->x),scaley(Coord->y),c);
}

/**
 * screen_line_draw(Coord1, Coord2) - Draw a mode 1 line
 */
void screen_line_draw(padPt* Coord1, padPt* Coord2)
{
  unsigned char x1=scalex(Coord1->x);
  unsigned char x2=scalex(Coord2->x);
  unsigned char y1=scaley(Coord1->y);
  unsigned char y2=scaley(Coord2->y);
  unsigned char c=((CurMode==ModeInverse||CurMode==ModeErase) ? 0 : 1);  

  monobitmap_draw_line(x1,y1,x2,y2,c);
}

/**
 * screen_char_draw(Coord, ch, count) - Output buffer from ch* of length count as PLATO characters
 */
void screen_char_draw(padPt* Coord, unsigned char* ch, unsigned char count)
{
  short offset; /* due to negative offsets */
  int x;      /* Current X and Y coordinates */
  int y;
  int* px;   /* Pointers to X and Y coordinates used for actual plotting */
  int* py;
  unsigned char i; /* current character counter */
  unsigned char a; /* current character byte */
  unsigned char j,k; /* loop counters */
  signed char b; /* current character row bit signed */
  unsigned char width=FONT_SIZE_X;
  unsigned char height=FONT_SIZE_Y;
  unsigned short deltaX=1;
  unsigned short deltaY=1;
  unsigned char mainColor=current_foreground;
  unsigned char altColor=current_background;
  unsigned char *p;
  unsigned char* curfont;

  switch(CurMem)
    {
    case M0:
      curfont=(char*)font;
      offset=-32;
      break;
    case M1:
      curfont=(char*)font;
      offset=64;
      break;
    case M2:
      curfont=fontm23;
      offset=-32;
      break;
    case M3:
      curfont=fontm23;
      offset=32;      
      break;
    }

  if (CurMode==ModeRewrite)
    {
      altColor=current_background;
    }
  else if (CurMode==ModeInverse)
    {
      altColor=current_foreground;
    }
  
  if (CurMode==ModeErase || CurMode==ModeInverse)
    mainColor=current_background;
  else
    mainColor=current_foreground;

  x=scalex((Coord->x&0x1FF));

  if (ModeBold)
    y=scaley((Coord->y+30)&0x1FF);
  else
    y=scaley((Coord->y+15)&0x1FF);  
  
  if (FastText==padF)
    {
      goto chardraw_with_fries;
    }

  /* the diet chardraw routine - fast text output. */
  
  for (i=0;i<count;++i)
    {
      a=*ch;
      ++ch;
      a+=offset;
      p=&curfont[FONTPTR(a)];
      
      for (j=0;j<FONT_SIZE_Y;++j)
  	{
  	  b=*p;
	  
  	  for (k=0;k<FONT_SIZE_X;++k)
  	    {
  	      if (b<0) /* check sign bit. */
		monobitmap_set_pixel(x,y,mainColor);
	      
	      ++x;
  	      b<<=1;
  	    }

	  ++y;
	  
	  if (y>191)
	    y-=192;
	  x-=width;
	  ++p;
  	}

      x+=width;
      y-=height;
      if (y<0)
	y=192+y;
    }

  return;

 chardraw_with_fries:
  if (Rotate)
    {
      deltaX=-abs(deltaX);
      width=-abs(width);
      px=&y;
      py=&x;
    }
    else
    {
      px=&x;
      py=&y;
    }
  
  if (ModeBold)
    {
      deltaX = deltaY = 2;
      width<<=1;
      height<<=1;
    }
  
  for (i=0;i<count;++i)
    {
      
      a=*ch;
      ++ch;
      a+=offset;
      p=&curfont[FONTPTR(a)];
      for (j=0;j<FONT_SIZE_Y;++j)
  	{
  	  b=*p;

	  if (Rotate)
	    {
	      px=&y;
	      py=&x;
	    }
	  else
	    {
	      px=&x;
	      py=&y;
	    }

  	  for (k=0;k<FONT_SIZE_X;++k)
  	    {
  	      if (b<0) /* check sign bit. */
		{
		  if (ModeBold)
		    {
		      monobitmap_set_pixel(*px+1,*py,mainColor);
		      monobitmap_set_pixel(*px,*py+1,mainColor);
		      monobitmap_set_pixel(*px+1,*py+1,mainColor);
		    }
		  
		  monobitmap_set_pixel(*px,*py,mainColor);
		}
	      else
		{
		  if (CurMode==ModeInverse || CurMode==ModeRewrite)
		    {
		      if (ModeBold)
			{
			  
			  monobitmap_set_pixel(*px+1,*py,altColor);
			  monobitmap_set_pixel(*px,*py+1,altColor);
			  monobitmap_set_pixel(*px+1,*py+1,altColor);
			}
		      monobitmap_set_pixel(*px,*py,altColor); 
		    }
		}

	      x += deltaX;
  	      b<<=1;
  	    }

	  y+=deltaY;
	  if (y>191)
	    y-=192;
	  x-=width;
	  
	  ++p;
  	}

      Coord->x+=width;
      x+=width;
      y-=height;
      
      if (y<0)
	y=192+y;
    }
  return;  
}

/**
 * screen_tty_char - Called to plot chars when in tty mode
 */
void screen_tty_char(padByte theChar)
{
  
  if ((theChar >= 0x20) && (theChar < 0x7F)) {
    screen_char_draw(&TTYLoc, &theChar, 1);
    TTYLoc.x += CharWide;
  }
  else if ((theChar == 0x0b)) /* Vertical Tab */
    {
      TTYLoc.y += CharHigh;
    }
  else if ((theChar == 0x08) && (TTYLoc.x > 7))	/* backspace */
    {
      padPt ec1,ec2;
      TTYLoc.x -= CharWide;
      ec1.x=TTYLoc.x;
      ec1.y=TTYLoc.y;
      ec2.x=TTYLoc.x+CharWide;
      ec2.y=TTYLoc.y+CharHigh;
      screen_block_draw(&ec1,&ec2);
   }
  else if (theChar == 0x0A)			/* line feed */
    TTYLoc.y -= CharHigh;
  else if (theChar == 0x0D)			/* carriage return */
    TTYLoc.x = 0;
  
  if (TTYLoc.x + CharWide > 511) {	/* wrap at right side */
    TTYLoc.x = 0;
    TTYLoc.y -= CharHigh;
  }
  
  if (TTYLoc.y < 0) {
    screen_clear();
    TTYLoc.y=495;
  }
  
}

/**
 * screen_foreground - Called to set foreground color.
 */
void screen_foreground(padRGB* theColor)
{
  
}

/**
 * screen_background - Called to set foreground color.
 */
void screen_background(padRGB* theColor)
{
}

/**
 * screen_paint - Called to paint at location.
 */
void screen_paint(padPt* Coord)
{
}

/**
 * screen_done()
 * Close down TGI
 */
void screen_done(void)
{
}
