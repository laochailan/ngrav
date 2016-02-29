#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ui.h"
#include "parts.h"
#include "file.h"

int record_loop(char *filename) {
	Parts p;
	File f;
	
	if(parts_init(&p) != 0) {
		puts("Error initializing Particles");
		return 1;
	}

	if(file_open_write(&f, filename, p.N) != 0) {
		printf("Error opening '%s': %s\n", filename, strerror(errno));
		return 1;
	}

	int frames = 0;

	srandom(time(0));
	while(1) {
		parts_step(&p);
		file_write_frame(&f, &p);

		frames++;
		if(frames % 10 == 0) {
			printf("%d frames\n", frames);
		}
	}

	parts_deinit(&p);
	file_close(&f);

	return 0;
}

int show_loop(char *filename) {
	UI ui;
	File f;

	if(file_open_read(&f, filename) != 0) {
		printf("Error opening '%s': %s\n", filename, strerror(errno));
		return 1;
	}

	if(ui_init(&ui,f.N) != 0) {
		puts("Error initializing UI.");
		return 1;
	}

	mkdir("frames",0751);

	int frames = 0;
	while(!ui_should_quit(&ui)) {
		if(file_read_frame(&f) != 0)
			break;
		if(file_read_frame(&f) != 0)
			break;

		ui_draw_file(&ui, &f);
		ui_poll_events(&ui);
		frames++;
		if(frames % 10 == 0) {
			printf("%d frames\n", frames);
		}

		/*char filename[32];
		snprintf(filename, sizeof(filename), "frames/%05d.webp", frames);

		int rc = ui_save_frame(&ui, filename);
		if(rc != 0) {
			printf("failed to save frame: %s\n", strerror(errno));
			break;
		}*/
	}
	file_close(&f);
	ui_deinit(&ui);
	return 0;
}


int normal_loop(void) {
	UI ui;
	Parts p;
	
	if(parts_init(&p) != 0) {
		puts("Error initializing Particles");
		return 1;
	}
	if(ui_init(&ui,p.N) != 0) {
		puts("Error initializing UI.");
		return 1;
	}

	int frames = 0;
	double t = ui_time();

	srandom(time(0));
	while(!ui_should_quit(&ui)) {
		parts_step(&p);

		ui_draw(&ui, &p);
		ui_poll_events(&ui);

		frames++;
		if(frames > 10) {
			frames = 0;
			printf("frame time: %f s\n", ui_time()-t);

			t = ui_time();
		}
	}

	ui_deinit(&ui);
	parts_deinit(&p);

	return 0;
}

static void set_mode(int *m, int s) {
	if(*m != 0) {
		printf("Error: Can only set one mode at once.");
		*m = -1;
	} else {
		*m = s;
	}
}

static void parse_options(int *mode, char **a1, int argc, char **argv) {
	int opt;
	while((opt = getopt(argc, argv, "rp:")) != -1) {
		switch(opt) {
			case 'r':
				set_mode(mode, 'r');
				break;
			case 'p':
				set_mode(mode, 'p');
				*a1 = malloc(strlen(optarg)+1);
				strcpy(*a1, optarg);
				break;
			case '?':
			case ':':
				printf("Usage: %s [-r] [-p FILE]\n", argv[0]);
				printf("\n\t-r\tRecord to cache without rendering.\n");
				printf("\t-p FILE\tPlay a recorded cache file.\n");
				*mode = -1;
				return;
		}
	}
}

int main(int argc, char **argv) {
	char *arg1 = 0;
	int mode = 0;
	int rc = 0;

	parse_options(&mode, &arg1, argc, argv);

	if(mode == 'r') {
		printf("-- Recording Mode --\n");
		time_t t = time(0);
		char fn[48];
		strftime(fn,48,"cache_%F_%T",localtime(&t));
		printf("Recording to '%s'.\n", fn);	

		rc = record_loop(fn);
	} else if(mode == 'p') {
		printf("-- Playback Mode --\n");
		printf("Rendering from '%s'.\n", arg1);

		rc = show_loop(arg1);
	} else if(mode == 0) {
		printf("-- Real-Time Mode --\n");
		rc = normal_loop();
	}

	free(arg1);
	return rc;
}
