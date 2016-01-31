#include "file.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "parts.h"
/* very crude. don’t care about byte order or floating point. */

int file_open_read(File *f, const char *filename) {
	memset(f, 0, sizeof(File));
	f->f = fopen(filename, "rb");
	if(f->f == 0)
		return 1;

	int rc = fread(&f->N, sizeof(int32_t), 1, f->f);
	if(rc != 1 || f->N <= 0)
		return 1;
	
	f->coords = malloc(3*f->N*sizeof(float));
	return 0;
}

int file_open_write(File *f, const char *filename, int N) {
	memset(f, 0, sizeof(File));
	f->f = fopen(filename, "wb");
	if(f->f == 0)
		return 1;

	int32_t N32 = N;
	int rc = fwrite(&N32, sizeof(int32_t), 1, f->f);
	if(rc != 1)
		return 1;
	
	f->coords = malloc(3*N*sizeof(float));

	return 0;
}

void file_close(File *f) {
	fclose(f->f);
	free(f->coords);
}

int file_write_frame(File *f, Parts *p) {
	int32_t size = 3*p->N;
	fwrite(&size,4,1,f->f);

	for(int i = 0; i < p->N; i++) {
		for(int j = 0; j < 3; j++) {
			f->coords[3*i+j] = p->parts[i].x[j];
		}
	}
	fwrite(f->coords,sizeof(float), p->N*3, f->f);

	return 0;
}

int file_read_frame(File *f) {
	int32_t size;
	fread(&size, 4,1,f->f);
	int rc = fread(f->coords, sizeof(float), size, f->f);
	if(rc != size)
		return 1;
	return 0;
}
