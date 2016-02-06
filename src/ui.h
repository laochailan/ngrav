#ifndef UI_H
#define UI_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "parts.h"
#include "quaternion.h"
#include "file.h"

struct quat;

typedef struct UI UI;
struct UI {
	GLFWwindow *win;

	GLuint program;
	GLuint projmatloc;
	GLuint viewmatloc;

	GLuint vb;
	GLfloat *coords;
	
	GLuint quadvb;

	quat cam;
	double scale;
	int traces;
	int bhtree;

	uint8_t *pixels;
};

int ui_init(UI *ui, int N);
void ui_deinit(UI *ui);

void ui_draw(UI *ui, Parts *p);
void ui_draw_file(UI *ui, File *f);
void ui_poll_events(UI *ui);

int ui_should_quit(UI *ui);
double ui_time(void);

int ui_save_frame(UI *ui, char *filename);

#endif
