/**
 * PLATOTERM NES - Mono bitmap functions
 */

#ifndef MONOBITMAP_H
#define MONOBITMAP_H

#include <stdlib.h>

// call every frame to split screen
void monobitmap_split(void);

// set a pixel at (x,y) color 1=set, 0=clear
void monobitmap_set_pixel(int x, int y, unsigned char color);

// draw a line from (x0,y0) to (x1,y1)
void monobitmap_draw_line(int x1, int y1, int x2, int y2, unsigned char color);

// write values 0..255 to nametable
void monobitmap_put_256inc(void);

// sets up attribute table
void monobitmap_put_attrib(void);

// clears pattern table
void monobitmap_clear(void);

// sets up PPU for monochrome bitmap
void monobitmap_setup(void);

#endif /* MONOBITMAP_H */
