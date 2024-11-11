#ifndef FONT_H
#error ERROR: No font header included to test.
#endif

#include <stdio.h>
#include <ctype.h>

int main()
{
  for(unsigned char i = 0; i < 0xff; i++) {
    printf("Codepoint %hhu:", i);
    if(isprint(i)) printf(" %c", i);
    printf("\n");
    for(unsigned char j = 0; j < FONT_HEIGHT; j++) {
      unsigned char tmp = 0;
      if(!(i & 1)) tmp = font_pixels[i >> 1][j] >> 4;
      else tmp = font_pixels[i >> 1][j] & 0xF;
      for(unsigned char k = 0; k < FONT_WIDTH; k++) {
	if(tmp & (1 << (3 - k))) printf("#");
	else printf(" ");
      }
      printf("\n");
    }
  }
}
