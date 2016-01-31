#include "ui.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "quaternion.h"
#include "bhtree.h"

const GLchar *vshader =
"#version 330 core\n\
uniform mat4 proj; \
uniform mat4 view; \
layout(location=0) in vec3 p; \
void main() { \
	gl_Position = proj*(transpose(view)*vec4(p,1)+vec4(0,0,-0.5,0));\
}";

const GLchar *fshader =
"#version 330 core\n\
out vec4 color; \
void main() { \
	color = vec4(0.5,0.9,1.0,0.6); \
}";

static void print_shader_errors(GLuint shader) {
	GLint logsize = 0;
	char *log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
	log = malloc(logsize);
	glGetShaderInfoLog(shader,logsize,0,log);

	printf("Error during shader compilation:\n%s\n",log);
	free(log);
}

static void print_program_errors(GLuint shader) {
	GLint logsize = 0;
	char *log;

	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logsize);
	log = malloc(logsize);
	glGetProgramInfoLog(shader,logsize,0,log);

	printf("Error during shader compilation:\n%s\n",log);
	free(log);
}

static int load_shaders(GLuint *program) {
	GLint success = 0;
	GLuint vs, fs;
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vshader, 0);
	glShaderSource(fs, 1, &fshader, 0);

	glCompileShader(vs);
	glCompileShader(fs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		print_shader_errors(vs);
		return 1;
	}
	
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		print_shader_errors(fs);
		return 1;
	}

	*program = glCreateProgram();
	glAttachShader(*program,vs);
	glAttachShader(*program,fs);

	glLinkProgram(*program);
	
	glGetProgramiv(*program, GL_LINK_STATUS, &success);
	if(success == GL_FALSE) {
		print_program_errors(*program);
		return 1;
	}
	
	glDetachShader(*program,vs);
	glDetachShader(*program,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return 0;
}

static void projection_matrix(mat4 m, double n, double f, double w, double h) {
	memset(m,0,sizeof(mat4));
	m[0] = n/w*2.;
	m[5] = n/h*2.;
	m[10] = -(f+n)/(f-n);
	m[14] = -2.*f*n/(f-n);
	m[11] = -1;
}

static void view_matrix(mat4 m, quat cam, double s) {
	int i;

	quat_to_matrix(&cam, m);

	for(i = 0; i < 15; i++) {
		m[i] *= s;
	}

}

int ui_init(UI *ui, int N) {
	memset(ui, 0, sizeof(UI));
	ui->scale = 2.5;

	if(!glfwInit())
		return 1;
	glfwWindowHint(GLFW_RESIZABLE,0);
	ui->win = glfwCreateWindow(400,400,"ngrav",0,0);
	if(!ui->win) {
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(ui->win);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	ui->coords = malloc(sizeof(float)*N*3);
	if(!ui->coords) {
		return 1;
	}

	if(load_shaders(&ui->program))
		return 1;
	
	ui->projmatloc = glGetUniformLocation(ui->program, "proj");
	ui->viewmatloc = glGetUniformLocation(ui->program, "view");
	glUseProgram(ui->program);
	
	mat4 proj;
	projection_matrix(proj, 0.1, 20, 1, 1);
	glUniformMatrix4fv(ui->projmatloc,1,0,(GLfloat *)proj);

	ui->cam.v[0]=1;

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		
	glGenBuffers(1,&ui->vb);
	glBindBuffer(GL_ARRAY_BUFFER, ui->vb);
	glBufferData(GL_ARRAY_BUFFER, N*3, 0, GL_STREAM_DRAW);

	
	return 0;
}


void ui_deinit(UI *ui) {
	glDeleteBuffers(1, &ui->vb);
	free(ui->coords);
	glfwTerminate();
}

int ui_should_quit(UI *ui) {
	return glfwWindowShouldClose(ui->win);
}

void draw_rect(UI *ui, vec p, double w) {
	int i,j,k;
	for(i = 0; i < 2; i++) {
		for(j = 0; j <2; j++) {
			k = 2*i+j;
			ui->coords[3*k+0] = p[0]+w/2*(1-2*i);
			ui->coords[3*k+1] = p[1]+w/2*(1-2*j)*(1-2*i);
			ui->coords[3*k+2] = 0;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, 4*3*sizeof(GLfloat), ui->coords, GL_STREAM_DRAW);
	glDrawArrays(GL_LINE_LOOP,0,4);
}


void draw_bhtree(UI *ui, BHNode *n) {
	if(n == 0)
		return;
	if(n->children[0] == 0) {
		draw_rect(ui, n->divp, n->width);
		return;
	}
	int i;
	for(i = 0; i < 8; i++)
		draw_bhtree(ui, n->children[i]);
}

void ui_draw(UI *ui, Parts *p) {
	int i;
	mat4 view;
	
	if(!ui->traces)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	view_matrix(view, ui->cam, ui->scale);
	glUniformMatrix4fv(ui->viewmatloc,1,0,(GLfloat *)view);

	for(i = 0; i < p->N; i++) {
		ui->coords[3*i] = p->parts[i].x[0];
		ui->coords[3*i+1] = p->parts[i].x[1];
		ui->coords[3*i+2] = p->parts[i].x[2];
	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ui->vb);
	glBufferData(GL_ARRAY_BUFFER, p->N*3*sizeof(GLfloat), ui->coords, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS,0,p->N);

	if(ui->bhtree)
		draw_bhtree(ui, p->bhtree);
	glDisableVertexAttribArray(0);

	
	glfwSwapBuffers(ui->win);
}

void ui_draw_file(UI *ui, File *f) {
	mat4 view;
	
	if(!ui->traces)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	view_matrix(view, ui->cam, ui->scale);
	glUniformMatrix4fv(ui->viewmatloc,1,0,(GLfloat *)view);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ui->vb);
	glBufferData(GL_ARRAY_BUFFER, f->N*3*sizeof(GLfloat), f->coords, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS,0,f->N);

	glDisableVertexAttribArray(0);

	
	glfwSwapBuffers(ui->win);
}

void ui_poll_events(UI *ui) {
	glfwPollEvents();
	vec axis1 = {0,1,0};
	vec axis2 = {1,0,0};
	quat tmp, tmp2;


	if(glfwGetKey(ui->win,GLFW_KEY_UP) == GLFW_PRESS) {
		memcpy(&tmp2,&ui->cam,sizeof(quat));
		rot_quat(&tmp,0.1,axis2);
		qmul(&ui->cam, &tmp2, &tmp);
	}
	if(glfwGetKey(ui->win,GLFW_KEY_DOWN) == GLFW_PRESS) {
		memcpy(&tmp2,&ui->cam,sizeof(quat));
		rot_quat(&tmp,-0.1,axis2);
		qmul(&ui->cam, &tmp2, &tmp);
	}
	if(glfwGetKey(ui->win,GLFW_KEY_LEFT) == GLFW_PRESS) {
		memcpy(&tmp2,&ui->cam,sizeof(quat));
		rot_quat(&tmp,-0.1,axis1);
		qmul(&ui->cam, &tmp2, &tmp);
	}
	if(glfwGetKey(ui->win,GLFW_KEY_RIGHT) == GLFW_PRESS) {
		memcpy(&tmp2,&ui->cam,sizeof(quat));
		rot_quat(&tmp,+0.1,axis1);
		qmul(&ui->cam, &tmp2, &tmp);
	}
	if(glfwGetKey(ui->win,GLFW_KEY_Z) == GLFW_PRESS)
		ui->scale *= 0.98;
	if(glfwGetKey(ui->win,GLFW_KEY_X) == GLFW_PRESS)
		ui->scale /= 0.98;
	if(glfwGetKey(ui->win,GLFW_KEY_C) == GLFW_PRESS)
		ui->traces = 1;
	else ui->traces = 0;
	if(glfwGetKey(ui->win,GLFW_KEY_T) == GLFW_PRESS)
		ui->bhtree = 1;
	else ui->bhtree = 0;

	
}

double ui_time(void) {
	return glfwGetTime();
}
