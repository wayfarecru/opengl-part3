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

#define EMBEDDED
#include "./c83J-rot-two-objs-dual.cpp"

// check for new syntax with texture objects
static const bool use_new_tex_syntax = true;

const unsigned int WIN_W = 500; // window size in pixels, (Width, Height)
const unsigned int WIN_H = 500;
const unsigned int WIN_X = 100; // window position in pixels, (X, Y)
const unsigned int WIN_Y = 100;

const char* vertFileName = "c83-single-tex.vert";
const char* fragFileName = "c83-single-tex.frag";

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

glm::vec2 texCoord[] = {
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f },
};

GLuint texObj_color = 0;
GLuint texObj_depth = 0;

void initTex(void) {
	// make a tex object for color buffer
	glGenTextures(1, &texObj_color);
	printf("tex object = %d\n", texObj_color);
	// bind the texture object to the texture unit #0
	glActiveTexture(GL_TEXTURE0); // select tex unit #0
	glBindTexture(GL_TEXTURE_2D, texObj_color);
	// new for OpenGL 4.5+: load an image to the texture object
	if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
		printf("using OpenGL 4.5 syntax\n");
		glTextureStorage2D(texObj_color, 1, GL_RGBA8, RTT_WIN_W, RTT_WIN_H);
		glGenerateTextureMipmap(texObj_color);
		glTextureParameterf(texObj_color, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameterf(texObj_color, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else { // original style: load an image to the texture unit (and its associated texture object)
		printf("using OpenGL 3.3+ syntax\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RTT_WIN_W, RTT_WIN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	// make another tex object for depth buffer
	glGenTextures(1, &texObj_depth);
	printf("tex object = %d\n", texObj_depth);
	// bind the texture object to the texture unit #1
	glActiveTexture(GL_TEXTURE1); // select tex unit #1
	glBindTexture(GL_TEXTURE_2D, texObj_depth);
	// new for OpenGL 4.5+: load an image to the texture object
	if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
		printf("using OpenGL 4.5 syntax\n");
		glTextureStorage2D(texObj_depth, 1, GL_DEPTH_COMPONENT32F, RTT_WIN_W, RTT_WIN_H);
		glGenerateTextureMipmap(texObj_depth);
		glTextureParameterf(texObj_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameterf(texObj_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else { // original style: load an image to the texture unit (and its associated texture object)
		printf("using OpenGL 3.3+ syntax\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RTT_WIN_W, RTT_WIN_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

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
	case GLFW_KEY_1:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj_color, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		} else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		printf("MAG_FILTER = NEAREST\n");
		break;
	case GLFW_KEY_2:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj_color, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		printf("MAG_FILTER = LINEAR\n");
		break;
	}
}

system_clock::time_point lastTime = system_clock::now();

void updateFunc(void) {
	system_clock::time_point curTime = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(curTime - lastTime); // in millisecond
	GLfloat theta = (elapsedTimeMSEC.count() / 1000.0F) * 2 * (float)M_PI; // in <math.h>, M_PI = pi
	angle += theta * dir;
	lastTime = curTime;
	// update vertex positions
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
	glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(prog);
	// provide the vertex attributes
	GLuint locPos = glGetAttribLocation(prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertPos[0]));
	GLuint locTexCoord = glGetAttribLocation(prog, "aTexCoord");
	glEnableVertexAttribArray(locTexCoord);
	glVertexAttribPointer(locTexCoord, 2, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(texCoord[0]));
	// provide texture unit #0 to the texture sampler
	GLuint locTexSampler = glGetUniformLocation(prog, "texSampler");
	glUniform1i(locTexSampler, 0);
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

GLuint rbo_color = 0;
GLuint fbo = 0; // FBO

void initFBOandRBO(void) {
	// prepare RBO's
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, RTT_WIN_W, RTT_WIN_H);
	// prepare FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// bind texture to FBO
	glActiveTexture(GL_TEXTURE0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj_color, 0);
	glActiveTexture(GL_TEXTURE1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texObj_depth, 0);
	// bind RBO to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, rbo_color);
	// buffer settings
	static GLenum val[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glNamedFramebufferDrawBuffers(fbo, 2, val);
	glNamedFramebufferReadBuffer(fbo, GL_COLOR_ATTACHMENT1);
#if NEEDED
	glBindFragDataLocation(rtt_prog, 0, "FragColor");
	glBindFragDataLocation(rtt_prog, 1, "FragAnother");
#endif
}

#include "svpng.inc"

void rtt_readFunc(void) {
	static int count = 0;
	if (count++ == 29) { // dump first several frames !
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
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, basename, nullptr, nullptr);
	glfwSetWindowPos(window, WIN_X, WIN_Y);
	glfwMakeContextCurrent(window);
	glewInit();
	// prepare
	glfwSetWindowRefreshCallback(window, refreshFunc);
	glfwSetKeyCallback(window, keyFunc);
	// main loop
	rtt_initFunc();
	initFunc();
	initTex();
	initFBOandRBO();
	while (! glfwWindowShouldClose(window)) {
		// update texture image with render-to-texture support
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		rtt_updateFunc();
		rtt_drawFunc();
		rtt_readFunc();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
