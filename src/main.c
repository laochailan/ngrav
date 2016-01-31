#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

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
		printf("Error opening '%s': %s", filename, strerror(errno));
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
		printf("Error opening '%s': %s", filename, strerror(errno));
		return 1;
	}

	if(ui_init(&ui,f.N) != 0) {
		puts("Error initializing UI.");
		return 1;
	}

	while(!ui_should_quit(&ui)) {
		if(file_read_frame(&f) != 0)
			break;

		ui_draw_file(&ui, &f);
		ui_poll_events(&ui);
	}

	file_close(&f);
	ui_deinit(&ui);
	return 0;
}

int main(int argc, char **argv) {
	UI ui;
	Parts p;

	if(argc == 1) {
		char *fn = "cache";
		printf("Recording to '%s'.\n", fn);	

		return record_loop(fn);
	} else if(argc == 2) {
		printf("Rendering from '%s'.\n", argv[1]);

		return show_loop(argv[1]);
	} else {
		printf("Too many arguments. Calculate and render at the same time.\n");
	}

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
	return 0;
}
