#ifndef __cplusplus
#error This file works only with C++
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <iostream>
using namespace std;
using namespace std::chrono;

#pragma warning(disable: 4711 4710 4100 4514 4626 4774 4365 4625 4464 4571 4201 5026 5027 5039 4819)

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // for glm::to_string()
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr( )

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#include "./common.c"

const unsigned int WIN_W = 500; // window size in pixels, (Width, Height)
const unsigned int WIN_H = 500;
const unsigned int WIN_X = 100; // window position in pixels, (X, Y)
const unsigned int WIN_Y = 100;

const char* vertFileName = "c71-rect.vert";
const char* fragFileName = "c71-rect.frag";

GLuint vert = 0; // vertex shader ID number
GLuint frag = 0; // fragment shader ID number
GLuint prog = 0; // shader program ID number

void initFunc(void) {
	const char* vertSource = loadFile( vertFileName );
	const char* fragSource = loadFile( fragFileName );
	char buf[1024]; // mesg buffer
	GLint status; // for glGetShaderiv()
	// vert: vertex shader
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSource, nullptr);
	glCompileShader(vert); // compile to get .OBJ
	glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
	printf("vert compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(vert, sizeof(buf), nullptr, buf);
	printf("vert log = [%s]\n", buf);
	// frag: fragment shader
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSource, nullptr);
	glCompileShader(frag); // compile to get .OBJ
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	printf("frag compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(frag, sizeof(buf), nullptr, buf);
	printf("frag log = [%s]\n", buf);
	// prog: program
	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog); // link to get .EXE
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	printf("prog link status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(prog, sizeof(buf), nullptr, buf);
	printf("link log = [%s]\n", buf);
	glValidateProgram(prog);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	printf("prog validate status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(prog, sizeof(buf), nullptr, buf);
	printf("validate log = [%s]\n", buf);
	fflush(stdout);
	// execute it!
	glUseProgram(prog);
	// done
	free( (void*)vertSource );
	free( (void*)fragSource );
}

const GLfloat map_size_x = 0.5f;
const GLfloat map_size_y = 0.5f;

glm::vec4 vertPos[] = { // 4 vertices for a rectangle
	{ -map_size_x, -map_size_y, 0.0f, 1.0f }, // v0
	{ +map_size_x, -map_size_y, 0.0f, 1.0f }, // v1
	{ -map_size_x, +map_size_y, 0.0f, 1.0f }, // v2
	{ +map_size_x, +map_size_y, 0.0f, 1.0f }, // v3
};

glm::vec4 vertColor[] = {
	{ 1.0F, 0.5F, 0.5F, 1.0F }, // red
	{ 1.0F, 0.5F, 0.5F, 1.0F }, // red
	{ 1.0F, 1.0F, 0.5F, 1.0F }, // yellow
	{ 1.0F, 1.0F, 0.5F, 1.0F }, // yellow
};

float angle = 0.0F;
float dir = 0.0F; // -1, 0, +1 

void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_R:
		if (action == GLFW_PRESS) {
			angle = 0.0F;
			dir = 0.0F;
		}
		break;
	case GLFW_KEY_Q: dir = +1.0F; break;
	case GLFW_KEY_A: dir =  0.0F; break;
	case GLFW_KEY_Z: dir = -1.0F; break;
	}
}

system_clock::time_point lastTime = system_clock::now();

void updateFunc(void) {
	system_clock::time_point curTime = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(curTime - lastTime); // in millisecond
	GLfloat theta = (elapsedTimeMSEC.count() / 1000.0F) * 2 * (float)M_PI; // in <math.h>, M_PI = pi
	angle += theta * dir;
	lastTime = curTime;
	// update vertex positions: remember that: size_x = size y = 0.5f
	vertPos[2].x = -map_size_x + 0.2f * cosf( 1.25f * angle ) - 0.2f;
	vertPos[2].y = +map_size_y + 0.2f * sinf( 1.25f * angle );
	vertPos[3].x = +map_size_x + 0.1f * cosf( 1.00f * angle ) - 0.1f;
	vertPos[3].y = +map_size_y + 0.1f * sinf( 1.00f * angle );
}

void drawFunc(GLFWwindow* window) {
	// z-buffer setting
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	// set viewport
	int win_w, win_h;
	glfwGetWindowSize(window, &win_w, &win_h);
	GLint vp_x, vp_y;
	GLsizei vp_w, vp_h;
	GLfloat aspect = (GLfloat)WIN_W / (GLfloat)WIN_H; // 320 / 240 = 1.333
	if (win_w < win_h * aspect) { // portrait case
		vp_w = win_w;
		vp_h = (GLsizei)(win_w / aspect);
		vp_x = 0;
		vp_y = (win_h - vp_h) / 2;
	} else { // landscape case
		vp_h = win_h;
		vp_w = (GLsizei)(win_h * aspect);
		vp_y = 0;
		vp_x = (win_w - vp_w) / 2;
	}
	glViewport(vp_x, vp_y, vp_w, vp_h);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// provide the vertex attributes
	GLuint locPos = glGetAttribLocation(prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertPos[0]));
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertColor[0]));
	// draw a rectangle
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// done
	glFinish();
}

void refreshFunc(GLFWwindow* window) {
	// refresh
	drawFunc(window);
	// GLFW action
	glfwSwapBuffers(window);
}

int main(int argc, char* argv[]) {
	const char* basename = getBaseName( argv[0] );
	// start GLFW & GLEW
	glfwInit();
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, basename, nullptr, nullptr);
	glfwSetWindowPos(window, WIN_X, WIN_Y);
	glfwMakeContextCurrent(window);
	glewInit();
	// prepare
	glfwSetWindowRefreshCallback(window, refreshFunc);
	glfwSetKeyCallback(window, keyFunc);
	glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
	// main loop
	initFunc();
	while (! glfwWindowShouldClose(window)) {
		// animation loop
		updateFunc();
		drawFunc(window);
		// GLFW actions
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// done
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("error code = %#x\n", err);
	}
	glfwTerminate();
	return 0;
}
