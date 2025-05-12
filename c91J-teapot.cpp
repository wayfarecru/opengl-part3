#ifndef __cplusplus
#error This file works only with C++
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace std::chrono;

#pragma warning(disable: 4711 4710 4100 4514 4626 4774 4365 4625 4464 4571 4201 5026 5027 5039 4819 4305)

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
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

const char* vertFileName = "c91-set-mat4.vert";
const char* fragFileName = "c91-set-mat4.frag";

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

int levelTeapot = 2;
int numVertTeapot = 0;
glm::vec4 vertTeapot[102400];
glm::vec4 colorTeapot[102400];

void quad(const glm::vec3& v11, const glm::vec3& v21, const glm::vec3& v22, const glm::vec3& v12) {
	// two triangles
	vertTeapot[numVertTeapot] = glm::vec4(v11.x, v11.y, v11.z, 1.0F);
	vertTeapot[numVertTeapot+1] = glm::vec4(v22.x, v22.y, v22.z, 1.0F);
	vertTeapot[numVertTeapot+2] = glm::vec4(v21.x, v21.y, v21.z, 1.0F);
	vertTeapot[numVertTeapot+3] = glm::vec4(v22.x, v22.y, v22.z, 1.0F);
	vertTeapot[numVertTeapot+4] = glm::vec4(v11.x, v11.y, v11.z, 1.0F);
	vertTeapot[numVertTeapot+5] = glm::vec4(v12.x, v12.y, v12.z, 1.0F);
	// color
	glm::vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
	color.r = ((rand() % 1000) / 1000.0F) * 0.25f + 0.75f;
	color.g = ((rand() % 1000) / 1000.0F) * 0.25f + 0.75f;
	color.b = ((rand() % 1000) / 1000.0F) * 0.25f + 0.75f;
	colorTeapot[numVertTeapot] = color;
	colorTeapot[numVertTeapot+1] = color;
	colorTeapot[numVertTeapot+2] = color;
	colorTeapot[numVertTeapot+3] = color;
	colorTeapot[numVertTeapot+4] = color;
	colorTeapot[numVertTeapot+5] = color;
	numVertTeapot += 6;
} 

void divCurve(
	const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 p3, 
	glm::vec3& l0, glm::vec3& l1, glm::vec3& l2, glm::vec3& l3,
	glm::vec3& r1, glm::vec3& r2, glm::vec3& r3) {
	l0 = p0;
	r3 = p3;
	l1 = (p0 + p1) / 2.0F;
	r2 = (p2 + p3) / 2.0F;
	l2 = (l1 + (p1 + p2) / 2.0F) / 2.0F;
	r1 = (r2 + (p1 + p2) / 2.0F) / 2.0F;
	l3 = (l2 + r1) / 2.0F;
}

void generatePatch(int level,
	const glm::vec3 v11, const glm::vec3 v12, const glm::vec3 v13, const glm::vec3 v14, 
	const glm::vec3 v21, const glm::vec3 v22, const glm::vec3 v23, const glm::vec3 v24, 
	const glm::vec3 v31, const glm::vec3 v32, const glm::vec3 v33, const glm::vec3 v34, 
	const glm::vec3 v41, const glm::vec3 v42, const glm::vec3 v43, const glm::vec3 v44) {
	if (level <= 0) {
		quad(v11, v21, v22, v12);
		quad(v12, v22, v23, v13);
		quad(v13, v23, v24, v14);
		quad(v21, v31, v32, v22);
		quad(v22, v32, v33, v23);
		quad(v23, v33, v34, v24);
		quad(v31, v41, v42, v32);
		quad(v32, v42, v43, v33);
		quad(v33, v43, v44, v34);
	} else {
		// first stage: division along u-direction
		glm::vec3 u[4][7];
		divCurve(v11, v12, v13, v14, u[0][0], u[0][1], u[0][2], u[0][3], u[0][4], u[0][5], u[0][6]);
		divCurve(v21, v22, v23, v24, u[1][0], u[1][1], u[1][2], u[1][3], u[1][4], u[1][5], u[1][6]);
		divCurve(v31, v32, v33, v34, u[2][0], u[2][1], u[2][2], u[2][3], u[2][4], u[2][5], u[2][6]);
		divCurve(v41, v42, v43, v44, u[3][0], u[3][1], u[3][2], u[3][3], u[3][4], u[3][5], u[3][6]);
		// second stage: division along v-direction
		glm::vec3 w[7][7];
		for (int i = 0; i < 7; ++i) {
			divCurve(u[0][i], u[1][i], u[2][i], u[3][i], 
					w[0][i], w[1][i], w[2][i], w[3][i], w[4][i], w[5][i], w[6][i]);
		}
		// sub patches
		generatePatch(level - 1,
			w[0][0], w[0][1], w[0][2], w[0][3],
			w[1][0], w[1][1], w[1][2], w[1][3],
			w[2][0], w[2][1], w[2][2], w[2][3],
			w[3][0], w[3][1], w[3][2], w[3][3]);
		generatePatch(level - 1,
			w[0][3], w[0][4], w[0][5], w[0][6],
			w[1][3], w[1][4], w[1][5], w[1][6],
			w[2][3], w[2][4], w[2][5], w[2][6],
			w[3][3], w[3][4], w[3][5], w[3][6]);
		generatePatch(level - 1,
			w[3][0], w[3][1], w[3][2], w[3][3],
			w[4][0], w[4][1], w[4][2], w[4][3],
			w[5][0], w[5][1], w[5][2], w[5][3],
			w[6][0], w[6][1], w[6][2], w[6][3]);
		generatePatch(level - 1,
			w[3][3], w[3][4], w[3][5], w[3][6],
			w[4][3], w[4][4], w[4][5], w[4][6],
			w[5][3], w[5][4], w[5][5], w[5][6],
			w[6][3], w[6][4], w[6][5], w[6][6]);
	 }
}

#include "c91J-teapotdata.h"
 
void generateTeapot(int level) {
	for (int i = 0; i < kTeapotNumPatches; ++i) {
		glm::vec3 v[16];
		for (int j = 0; j < 16; ++j) {
			int ind = teapotPatches[i][j] - 1;
			v[j] = glm::vec3(0.25F) * glm::vec3(teapotVertices[ind][0], teapotVertices[ind][1], teapotVertices[ind][2]);
		}
		generatePatch(levelTeapot,
			v[0], v[1], v[2], v[3],
			v[4], v[5], v[6], v[7],
			v[8], v[9], v[10], v[11],
			v[12], v[13], v[14], v[15]
		);
	}
}

glm::mat4 mat = glm::mat4( 1.0F ); // mat = matDrag * matModel
glm::mat4 matDrag = glm::mat4( 1.0F );
glm::mat4 matUpdated = glm::mat4( 1.0F );

void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_R:
		mat = matDrag = matUpdated = glm::mat4( 1.0F ); // reset every thing
		break;
	}
}

int mousePressed = GL_FALSE;
glm::vec2 dragStart; // mouse dragging start point

glm::vec3 calcUnitVec(const glm::vec2& raw) {
	glm::vec2 scr;
	// debug for out of window conditions
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L) // C++17 or later
	scr.x = std::clamp(raw.x, 0.0F, (float)WIN_W);
	scr.y = std::clamp(raw.y, 0.0F, (float)WIN_H);
#else
	scr.x = (org.x < 0.0F ? 0.0F : (org.x > WIN_W ? WIN_W : org.x));
	scr.y = (org.y < 0.0F ? 0.0F : (org.y > WIN_H ? WIN_H : org.y));
#endif
	// normal processing
	const GLfloat radius = sqrtf(WIN_W * WIN_W + WIN_H * WIN_H) / 2.0F;
	glm::vec3 v;
	v.x = (scr.x - WIN_W / 2.0F) / radius;
	v.y = (WIN_H / 2.0F - scr.y) / radius;
	v.z = sqrtf(1.0F - v.x * v.x - v.y * v.y);
	return v;
}

glm::mat4 calcTrackball(const glm::vec2& start, const glm::vec2& cur) {
	glm::vec3 org = calcUnitVec( start );
	glm::vec3 dst = calcUnitVec( cur );
	glm::quat q = glm::rotation( org, dst );
	glm::mat4 m = glm::toMat4( q );
	return m;
}

void cursorEnterFunc(GLFWwindow* win, int entered) {
	printf("cursor %s the window\n", (entered == GL_FALSE) ? "leaving" : "entering");
	fflush(stdout);
}

void cursorPosFunc(GLFWwindow* win, double xscr, double yscr) {
	if (mousePressed == GL_TRUE) {
		glm::vec2 dragCur = glm::vec2((GLfloat)xscr, (GLfloat)yscr);
		matDrag = calcTrackball( dragStart, dragCur );
		mat = matDrag * matUpdated;
	}
}

void mouseButtonFunc(GLFWwindow* win, int button, int action, int mods) {
	GLdouble x, y;
	switch (action) {
	case GLFW_PRESS:
		mousePressed = GL_TRUE;
		glfwGetCursorPos(win, &x, &y);
		dragStart = glm::vec2((GLfloat)x, (GLfloat)y);
		break;
	case GLFW_RELEASE:
		mousePressed = GL_FALSE;
		glfwGetCursorPos(win, &x, &y);
		glm::vec2 dragCur = glm::vec2((GLfloat)x, (GLfloat)y);
		matDrag = calcTrackball( dragStart, dragCur );
		mat = matDrag * matUpdated;
		matDrag = glm::mat4(1.0F); // reset
		matUpdated = mat; // update to the object matrix
		break;
	}
	fflush(stdout);
}

void updateFunc(void) {
	// do nothing
}

void drawFunc(void) {
	// z-buffer setting
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	// clear in gray color
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// provide the vertex attributes
	GLuint locPos = glGetAttribLocation(prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertTeapot[0]));
	GLuint locColor = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(colorTeapot[0]));
	GLuint locMat = glGetUniformLocation(prog, "uMat");
	glUniformMatrix4fv(locMat, 1, GL_FALSE, glm::value_ptr(mat));
	// draw the sphere
	glDrawArrays(GL_TRIANGLES, 0, numVertTeapot);
	// done
	glFinish();
}

void refreshFunc(GLFWwindow* window) {
	// refresh
	drawFunc();
	// GLFW action
	glfwSwapBuffers(window);
}

int main(int argc, char* argv[]) {
	const char* basename = getBaseName( argv[0] );
	// argument processing
	if (argc > 1) {
		levelTeapot = strtol( argv[1], nullptr, 10 );
	}
	generateTeapot( levelTeapot );
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
	glfwSetCursorEnterCallback(window, cursorEnterFunc);
	glfwSetCursorPosCallback(window, cursorPosFunc);
	glfwSetMouseButtonCallback(window, mouseButtonFunc);
	glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
	// main loop
	initFunc();
	while (! glfwWindowShouldClose(window)) {
		// animation loop
		updateFunc();
		drawFunc();
		// GLFW actions
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// done
	glfwTerminate();
	return 0;
}
