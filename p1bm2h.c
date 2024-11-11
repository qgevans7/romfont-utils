#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *line_buffer = NULL;
size_t line_buffer_size = 0;
ssize_t line_length = 0;
size_t line_index = 0;
FILE *input_stream;
  
bool get_next_bit_or_die();
ssize_t getline_ignoring_comments();

int main()
{
  input_stream = stdin;
  
  line_length = getline(&line_buffer, &line_buffer_size, input_stream);

  char *magic = NULL;
  
  if(sscanf(line_buffer, "%2ms%zn", &magic, &line_index) <= 0 || strcmp(magic, "P1")) {
    fprintf(stderr, "Invalid PBM header magic.\n");
    if(magic[1] == '4') {
      fprintf(stderr, "This file appears to be \"raw\" (binary) PBM. This utility only supports P1 ASCII-format PBM.\n");
    }
    free(magic);
    free(line_buffer);
    exit(-1);
  }
  free(magic);

  size_t width = 0, height = 0;

  while(!width) {
    size_t line_index_increment = 0;
    int status = sscanf(line_buffer + line_index, " %zu%n", &width, &line_index_increment);
    line_index += line_index_increment;
    line_index_increment = 0;
    if(status <= 0) {
      if(status == 0) {
	sscanf(line_buffer + line_index, " #%n", &line_index_increment);
	if(!line_index_increment) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: invalid data found where width should be.\n");
	  exit(-1);
	}
      }
      while((line_length = getline_ignoring_comments()) <= 0) {
	if(line_length < 0) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: Error reading file before width value found.\n");
	  exit(-1);
	}
      }
      line_index = 0;
    }
  }

  if(width != 4) {
    free(line_buffer);
    fprintf(stderr, "Invalid PBM data: widths other than 4 are not currently supported.\n");
    exit(-1);
  }

  while(!height) {
    size_t line_index_increment = 0;
    int status = sscanf(line_buffer + line_index, " %zu%n", &height, &line_index_increment);
    line_index += line_index_increment;
    line_index_increment = 0;
    if(status <= 0) {
      if(status == 0) {
	sscanf(line_buffer + line_index, " #%n", &line_index_increment);
	if(!line_index_increment) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: invalid data found where height should be.\n");
	  exit(-1);
	}
      }
      while((line_length = getline_ignoring_comments()) <= 0) {
	if(line_length < 0) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: Error reading file before height value found.\n");
	  exit(-1);
	}
      }
      line_index = 0;
    }
  }
  
  if(height % 256) {
    free(line_buffer);
    fprintf(stderr, "Invalid input: file does not contain 256 characters' worth of pixel data.\n");
    exit(-1);
  }
  
  size_t character_width = width;
  size_t character_height = height / 256;

  printf("#ifndef FONT_H\n");
  printf("#define FONT_H\n\n");
  printf("#define FONT_HEIGHT %zu\n", character_height);
  printf("#define FONT_WIDTH %zu\n\n", character_width);
  printf("unsigned char font_pixels[128][%u] = {\n", character_height);

  unsigned char character_buffer[character_height];
  size_t character_y = 0;
  for(; character_y < character_height; character_y++) character_buffer[character_y] = 0;
  
  for(size_t current_character = 0; current_character < 256; current_character++) {
    if(!(current_character & 1)) {
      for(character_y = 0; character_y < character_height; character_y++) {
	for(uint8_t i = 0; i < character_width; i++) {
	  if(get_next_bit_or_die()) character_buffer[character_y] |= 1 << (7 - i);
	}
      }
    } else {
      for(character_y = 0; character_y < character_height; character_y++) {
	for(uint8_t i = 0; i < character_width; i++) {
	  if(get_next_bit_or_die()) character_buffer[character_y] |= 1 << (3 - i);
	}
      }
      printf("  { ");
      for(character_y = 0; character_y < character_height; character_y++) {
	printf("%#4hhx", character_buffer[character_y]);
	if(character_y < (character_height - 1)) {
	  printf(", ");
	} else {
	  printf(" },\n");
	}
	character_buffer[character_y] = 0;
      }
    }
  }

  printf("};\n");
  printf("\n#endif\n");

  return 0;
}

bool get_next_bit_or_die()
{
  uint8_t digit, index_increment;
  ssize_t status = sscanf(line_buffer + line_index," %1hhu%hhn", &digit, &index_increment);
  if(status < 0) {
    line_index = 0;
    for(ssize_t line_length = getline(&line_buffer, &line_buffer_size, input_stream);
	line_length == 0;
	line_length = getline(&line_buffer, &line_buffer_size, input_stream));
    if(line_length < 0) {
      free(line_buffer);
      fprintf(stderr, "Bad PBM data: Unexpected EOF or error reading file.\n");
      exit(-1);
    }
    return get_next_bit_or_die();
  }
  if(!status) {
    free(line_buffer);
    fprintf(stderr, "Bad PBM data: Invalid (non-digit) data found in raster area.\n");
    exit(-1);
  }
  line_index += index_increment;
  switch(digit) {
  case 1:
    return true;
  case 0:
    return false;
  default:
    free(line_buffer);
    fprintf(stderr, "Bad PBM data: Invalid (greater than 1) \"bit\" found in raster area.\n");
    exit(-1);
  }
}

ssize_t getline_ignoring_comments()
{
  ssize_t line_length = 0;

  do {
    line_length = getline(&line_buffer, &line_buffer_size, input_stream);
  } while(line_length >= 0 && line_buffer[0] == '#');

  return line_length;
}
