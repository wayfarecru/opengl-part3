#ifndef __cplusplus
#error This file works only with C++
#endif

#ifndef EMBEDDED // excluded when embedded into a render-to-texture program

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
#pragma warning(disable: 4711 4710 4100)

#include "./common.c"

#endif // excluded when embedded into a render-to-texture program

const unsigned int RTT_WIN_W = 500; // window size in pixels, (Width, Height)
const unsigned int RTT_WIN_H = 500;
const unsigned int RTT_WIN_X = 100; // window position in pixels, (X, Y)
const unsigned int RTT_WIN_Y = 100;

const char* rtt_vertFileName = "c83-mvp.vert";
const char* rtt_fragFileName = "c83-mvp.frag";

GLuint rtt_vert = 0; // vertex shader ID number
GLuint rtt_frag = 0; // fragment shader ID number
GLuint rtt_prog = 0; // shader program ID number

void rtt_initFunc(void) {
	const char* rtt_vertSource = loadFile( rtt_vertFileName );
	const char* rtt_fragSource = loadFile( rtt_fragFileName );
	char buf[1024]; // mesg buffer
	GLint status; // for glGetShaderiv()
	// vert: vertex shader
	rtt_vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(rtt_vert, 1, &rtt_vertSource, nullptr);
	glCompileShader(rtt_vert); // compile to get .OBJ
	glGetShaderiv(rtt_vert, GL_COMPILE_STATUS, &status);
	printf("rtt_vert compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(rtt_vert, sizeof(buf), nullptr, buf);
	printf("rtt_vert log = [%s]\n", buf);
	// frag: fragment shader
	rtt_frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(rtt_frag, 1, &rtt_fragSource, nullptr);
	glCompileShader(rtt_frag); // compile to get .OBJ
	glGetShaderiv(rtt_frag, GL_COMPILE_STATUS, &status);
	printf("rtt_frag compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(rtt_frag, sizeof(buf), nullptr, buf);
	printf("rtt_frag log = [%s]\n", buf);
	// prog: program
	rtt_prog = glCreateProgram();
	glAttachShader(rtt_prog, rtt_vert);
	glAttachShader(rtt_prog, rtt_frag);
	glLinkProgram(rtt_prog); // link to get .EXE
	glGetProgramiv(rtt_prog, GL_LINK_STATUS, &status);
	printf("rtt_prog link status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(rtt_prog, sizeof(buf), nullptr, buf);
	printf("link log = [%s]\n", buf);
	glValidateProgram(rtt_prog);
	glGetProgramiv(rtt_prog, GL_VALIDATE_STATUS, &status);
	printf("rtt_prog validate status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(rtt_prog, sizeof(buf), nullptr, buf);
	printf("validate log = [%s]\n", buf);
	fflush(stdout);
	// execute it!
	glUseProgram(rtt_prog);
	// done
	free( (void*)rtt_vertSource );
	free( (void*)rtt_fragSource );
}

glm::vec4 rtt_vertCube[] = { // 12 * 3 = 36 (vertices + color)
	// face 0,1: v0-v3-v2, v0-v2-v1, red
	{ -0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v0
	{ -0.5F, -0.5F, -0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v3
	{ +0.5F, -0.5F, -0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v2
	{ -0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v0
	{ +0.5F, -0.5F, -0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v2
	{ +0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F }, // v1
	// face 2,3: v1-v2-v6, v1-v6-v5, blue
	{ +0.5F, -0.5F, +0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v1
	{ +0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v2
	{ +0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v6
	{ +0.5F, -0.5F, +0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v1
	{ +0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v6
	{ +0.5F, +0.5F, +0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F }, // v5
	// face 4,5: v2-v3-v7, v2-v7-v6, green
	{ +0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v2
	{ -0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v3
	{ -0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v7
	{ +0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v2
	{ -0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v7
	{ +0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F }, // v6
	// face 6,7: v3-v0-v4, v3-v4-v7, cyan
	{ -0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v3
	{ -0.5F, -0.5F, +0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v0
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v4
	{ -0.5F, -0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v3
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v4
	{ -0.5F, +0.5F, -0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F }, // v7
	// face 8,9: v1-v5-v4, v1-v4-v0, magenta
	{ +0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v1
	{ +0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v5
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v4
	{ +0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v1
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v4
	{ -0.5F, -0.5F, +0.5F, 1.0F }, { 1.0F, 0.3F, 1.0F, 1.0F }, // v0
	// face 10,11: v4-v5-v6, v4-v6-v7, yellow
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v4
	{ +0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v5
	{ +0.5F, +0.5F, -0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v6
	{ -0.5F, +0.5F, +0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v4
	{ +0.5F, +0.5F, -0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v6
	{ -0.5F, +0.5F, -0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F }, // v7
	// your reference
#if 0
	{ -0.5F, -0.5F, -0.5F, 1.0F }, // v0
	{ +0.5F, -0.5F, -0.5F, 1.0F }, // v1
	{ +0.5F, -0.5F, +0.5F, 1.0F }, // v2
	{ -0.5F, -0.5F, +0.5F, 1.0F }, // v3
	{ -0.5F, +0.5F, -0.5F, 1.0F }, // v4
	{ +0.5F, +0.5F, -0.5F, 1.0F }, // v5
	{ +0.5F, +0.5F, +0.5F, 1.0F }, // v6
	{ -0.5F, +0.5F, +0.5F, 1.0F }, // v7
#endif
};

glm::vec4 rtt_vertPyramid[] = { // 6 * 3 = 18 (vertices + color)
	// face 0: v0-v1-v2, red
	{ 0.0F, 0.5F, 0.0F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F, }, // v0
	{ 0.5F, -0.3F, 0.0F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F, }, // v1
	{ 0.0F, -0.3F, -0.5F, 1.0F }, { 1.0F, 0.3F, 0.3F, 1.0F, }, // v2
	// face 1: v0-v2-v3, green
	{ 0.0F, 0.5F, 0.0F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F, }, // v0
	{ 0.0F, -0.3F, -0.5F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F, }, // v2
	{ -0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 1.0F, 0.3F, 1.0F, }, // v3
	// face 2: v0-v3-v4, blue
	{ 0.0F, 0.5F, 0.0F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F, }, // v0
	{ -0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F, }, // v3
	{ 0.0F, -0.3F, 0.5F, 1.0F }, { 0.3F, 0.3F, 1.0F, 1.0F, }, // v4
	// face 3: v0-v4-v1, yellow
	{ 0.0F, 0.5F, 0.0F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F, }, // v0
	{ 0.0F, -0.3F, 0.5F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F, }, // v4
	{ 0.5F, -0.3F, 0.0F, 1.0F }, { 1.0F, 1.0F, 0.3F, 1.0F, }, // v1
	// face 4: v1-v4-v3, cyan
	{ 0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v1
	{ 0.0F, -0.3F, 0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v4
	{ -0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v3
	// face 5: v1-v3-v2, cyan
	{ 0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v1
	{ -0.5F, -0.3F, 0.0F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v3
	{ 0.0F, -0.3F, -0.5F, 1.0F }, { 0.3F, 1.0F, 1.0F, 1.0F, }, // v2
	// your reference
#if 0
	{ 0.0F, 0.5F, 0.0F, 1.0F }, // v0
	{ 0.5F, -0.3F, 0.0F, 1.0F }, // v1
	{ 0.0F, -0.3F, 0.5F, 1.0F }, // v2
	{ -0.5F, -0.3F, 0.0F, 1.0F }, // v3
	{ 0.0F, -0.3F, -0.5F, 1.0F }, // v4
#endif
};

glm::mat4 rtt_matPyramid = glm::mat4( 1.0F );
glm::mat4 rtt_matCube = glm::mat4( 1.0F );
glm::mat4 rtt_matView = glm::mat4( 1.0F );
glm::mat4 rtt_matProj = glm::mat4( 1.0F );

float rtt_theta = 0.0F;
system_clock::time_point rtt_lastTime = system_clock::now();

void rtt_updateFunc(void) {
	system_clock::time_point rtt_curTime = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(rtt_curTime - rtt_lastTime); // in millisecond
	rtt_theta = (elapsedTimeMSEC.count() / 1000.0F) * (float)M_PI_2; // in <math.h>, M_PI_2 = pi/2
	// model transform for the pyramid
	rtt_matPyramid = glm::mat4( 1.0F );
	rtt_matPyramid = glm::translate( rtt_matPyramid, glm::vec3(-0.4F, 0.0F, 0.0F));
	rtt_matPyramid = glm::rotate( rtt_matPyramid, 2.0F * rtt_theta, glm::vec3(0.0F, 1.0F, 0.0F));
	rtt_matPyramid = glm::scale( rtt_matPyramid, glm::vec3(0.5F, 0.5F, 0.5F));
	// model transform for the cube
	rtt_matCube = glm::mat4( 1.0F );
	rtt_matCube = glm::translate( rtt_matCube, glm::vec3(0.4F, 0.0F, 0.0F));
	rtt_matCube = glm::rotate( rtt_matCube, 2.0F * rtt_theta, glm::vec3(1.0F, 0.0F, 0.0F));
	rtt_matCube = glm::scale( rtt_matCube, glm::vec3(0.3F, 0.3F, 0.3F));
	// viewing transform
	const GLfloat radius = 2.0F;
	rtt_matView = glm::lookAtRH(
	              glm::vec3( radius * sinf(rtt_theta), 20 * 0.05F, radius * cosf(rtt_theta) ),
	              glm::vec3( 0.02F, 0.0F, 0.0F ),
	              glm::vec3( 0.0F, 1.0F, 0.0F )
	          );
	// projection matrix
	const GLfloat zoom = 0.5F;
	rtt_matProj = glm::frustumRH(
	              -1.0F * zoom, +1.0F * zoom,
	              -0.75F * zoom, +0.75F * zoom,
	              +1.0F, +3.0F
	          );
	// print out
#if 1
	static bool first = true;
	if (first == true) {
		std::cout << glm::to_string( rtt_matProj ) << std::endl;
		first = false;
	}
#endif
}

void rtt_drawFunc(void) {
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	// clear in gray color
	glClearColor(0.1F, 0.4F, 0.4F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(rtt_prog);
	// matrix settings
	GLuint locView = glGetUniformLocation(rtt_prog, "uView");
	glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(rtt_matView));
	GLuint locProj = glGetUniformLocation(rtt_prog, "uProj");
	glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(rtt_matProj));
	// draw the pyramid
	GLuint locPos = glGetAttribLocation(rtt_prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), glm::value_ptr(rtt_vertPyramid[0]));
	GLuint locColor = glGetAttribLocation(rtt_prog, "aColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), glm::value_ptr(rtt_vertPyramid[1]));
	GLuint locModel = glGetUniformLocation(rtt_prog, "uModel");
	glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(rtt_matPyramid));
	glDrawArrays(GL_TRIANGLES, 0, 18); // 18 vertices
	// draw the cube
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), glm::value_ptr(rtt_vertCube[0]));
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), glm::value_ptr(rtt_vertCube[1]));
	glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(rtt_matCube));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices
	// done
	glFinish();
}

#ifndef EMBEDDED // excluded when embedded into a render-to-texture program

void refreshFunc(GLFWwindow* window) {
	// refresh
	rtt_drawFunc();
	// GLFW action
	glfwSwapBuffers(window);
}

void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_R:
		if (action == GLFW_PRESS) {
			rtt_lastTime = system_clock::now();
		}
		break;
	}
}

GLuint rbo_color = 0;
GLuint rbo_depth = 0;
GLuint fbo = 0;

void initFBOandRBO(void) {
	// prepare RBO's
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, RTT_WIN_W, RTT_WIN_H);
#if 1 // to turn off depth buffer, comment out this block
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RTT_WIN_W, RTT_WIN_H);
#endif
	// prepare FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// bind RBO to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
}

#include "svpng.inc"

void rtt_readFunc(void) {
	static int count = 0;
	if (count++ < 25) { // dump first several frames !
		GLubyte val[RTT_WIN_W * RTT_WIN_H * 4];
		glReadPixels(0, 0, RTT_WIN_W, RTT_WIN_H, GL_RGBA, GL_UNSIGNED_BYTE, val);
		glFinish();
		printf("framebuffer read completed\n");
		char name[80];
		sprintf(name, "opengl-saved-image-%03d.png", count);
		FILE* fp = fopen(name, "wb");
		svpng(fp, RTT_WIN_W, RTT_WIN_H, val, 1); // 1 = RGBA format
		fclose(fp);
		printf("image file save completed\n");
	}
}

int main(int argc, char* argv[]) {
	const char* basename = getBaseName( argv[0] );
	// start GLFW & GLEW
	glfwInit();
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(RTT_WIN_W, RTT_WIN_H, basename, nullptr, nullptr);
	glfwSetWindowPos(window, RTT_WIN_X, RTT_WIN_Y);
	glfwMakeContextCurrent(window);
	glewInit();
	// prepare
	glfwSetWindowRefreshCallback(window, refreshFunc);
	glfwSetKeyCallback(window, keyFunc);
	// main loop
	rtt_initFunc();
	initFBOandRBO();
	while (! glfwWindowShouldClose(window)) {
		// animation loop
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		rtt_updateFunc();
		rtt_drawFunc();
		rtt_readFunc();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.4F, 0.4F, 0.4F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		// GLFW actions
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// done
	glfwTerminate();
	return 0;
}

#endif // excluded when embedded into a render-to-texture program
