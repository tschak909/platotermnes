
#include "keyboard.h"
#include "key.h"
#include "protocol.h"
#include "io.h"
#include "screen.h"

unsigned char ch;

void keyboard_out(int platoKey)
{
  if (platoKey==0xff)
    {
      return;
    }
  
  if (platoKey>0x7F)
    {
      Key(ACCESS);
      Key(ACCESS_KEYS[platoKey-0x80]);
      return;
    }
  Key(platoKey);
  return;
}

void keyboard_main(void)
{
}

void keyboard_clear(void)
{
}

void keyboard_out_tty(int ch)
{
  io_send_byte(ch);
}
