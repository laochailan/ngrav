#ifndef FILE_H
#define FILE_H

#include <stdio.h>

/* Very crude file saving functionality. */

struct Parts;

typedef struct File File;
struct File {
	FILE *f;
	int N;
	float *coords;
};

int file_open_read(File *f, const char *filename);
int file_open_write(File *f, const char *filename, int N);
void file_close(File *f);

int file_write_frame(File *f, struct Parts *p);
int file_read_frame(File *f);

#endif
