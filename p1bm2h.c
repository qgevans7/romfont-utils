#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

ssize_t getline_ignoring_comments(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);

int main()
{
  char *line_buffer = NULL;
  size_t line_buffer_size = 0;
  ssize_t line_length = 0;
  
  line_length = getline(&line_buffer, &line_buffer_size, stdin);

  size_t index = 0;
  char *magic = NULL;
  
  if(sscanf(line_buffer, "%2ms%zn", &magic, &index) <= 0 || strcmp(magic, "P1")) {
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

  while(!height) {
    size_t index_increment = 0;
    int status = sscanf(line_buffer + index, " %zu%n", &height, &index_increment);
    printf("Status: %d\nHeight: %zu\n", status, height);
    index += index_increment;
    index_increment = 0;
    if(status <= 0) {
      if(status == 0) {
	sscanf(line_buffer + index, " #%n", &index_increment);
	if(!index_increment) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: invalid data found where height should be.\n");
	  exit(-1);
	}
      }
      while((line_length = getline_ignoring_comments(&line_buffer, &line_buffer_size, stdin)) <= 0) {
	if(line_length < 0) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: Error reading file before height value found.\n");
	  exit(-1);
	}
      }
      index = 0;
    }
  }

  while(!height) {
    size_t index_increment = 0;
    int status = sscanf(line_buffer + index, " %zu%n", &height, &index_increment);
    index += index_increment;
    index_increment = 0;
    if(status <= 0) {
      if(status == 0) {
	sscanf(line_buffer + index, " #%n", &index_increment);
	if(!index_increment) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: invalid data found where height should be.\n");
	  exit(-1);
	}
      }
      while((line_length = getline_ignoring_comments(&line_buffer, &line_buffer_size, stdin)) <= 0) {
	if(line_length < 0) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: Error reading file before height value found.\n");
	  exit(-1);
	}
      }
      index = 0;
    }
  }
  
  if(height % 256) {
    free(line_buffer);
    fprintf(stderr, "Invalid input: file does not contain 256 characters' worth of pixel data.\n");
    exit(-1);
  }
  
  while(!width) {
    size_t index_increment = 0;
    int status = sscanf(line_buffer + index, " %zu%n", &width, &index_increment);
    index += index_increment;
    index_increment = 0;
    if(status <= 0) {
      if(status == 0) {
	sscanf(line_buffer + index, " #%n", &index_increment);
	if(!index_increment) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: invalid data found where width should be.\n");
	  exit(-1);
	}
      }
      while((line_length = getline_ignoring_comments(&line_buffer, &line_buffer_size, stdin)) <= 0) {
	if(line_length < 0) {
	  free(line_buffer);
	  fprintf(stderr, "Invalid PBM header: Error reading file before width value found.\n");
	  exit(-1);
	}
      }
      index = 0;
    }
  }

  if(width != 4) {
    free(line_buffer);
    fprintf(stderr, "Invalid PBM data: widths other than 4 are not currently supported.\n");
    exit(-1);
  }

  for(line_length = getline_ignoring_comments(&line_buffer, &line_buffer_size, stdin);
      line_length >= 0;
      line_length = getline_ignoring_comments(&line_buffer, &line_buffer_size, stdin)) {
    printf("%s", line_buffer);
  }

  return 0;
}

ssize_t getline_ignoring_comments(char **restrict lineptr, size_t *restrict n, FILE *restrict stream)
{
  ssize_t line_length = 0;

  do {
    line_length = getline(lineptr, n, stream);
  } while(line_length >= 0 && *lineptr[0] == '#');

  return line_length;
}
