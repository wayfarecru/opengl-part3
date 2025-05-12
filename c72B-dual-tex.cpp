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

// check for new syntax with texture objects
static const bool use_new_tex_syntax = true;

const unsigned int WIN_W = 500; // window size in pixels, (Width, Height)
const unsigned int WIN_H = 500;
const unsigned int WIN_X = 100; // window position in pixels, (X, Y)
const unsigned int WIN_Y = 100;

const char* vertFileName = "c72-dual-tex.vert";
const char* fragFileName = "c72-dual-tex.frag";

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

const GLfloat map_size_x = 0.9f;
const GLfloat map_size_y = 0.9f;

glm::vec4 vertPos[] = { // 4 vertices for a rectangle
	{ -map_size_x, -map_size_y, 0.0f, 1.0f }, // v0
	{ +map_size_x, -map_size_y, 0.0f, 1.0f }, // v1
	{ -map_size_x, +map_size_y, 0.0f, 1.0f }, // v2
	{ +map_size_x, +map_size_y, 0.0f, 1.0f }, // v3
};

#include "c72-rose-img.cpp"

glm::vec2 texCoordRose[] = {
#if defined(REVERSED) // reversed texture image
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f },
#else // corrected: y-axis reversed
	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f },
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
#endif
};

#include "c72-logo-img.cpp"

glm::vec2 texCoordLogo[] = {
#if defined(REVERSED) // reversed texture image
	{ 0.0f, 0.0f },
	{ 3.0f, 0.0f },
	{ 0.0f, 4.0f },
	{ 3.0f, 4.0f },
#else // corrected: y-axis reversed
	{ 0.0f, 4.0f },
	{ 3.0f, 4.0f },
	{ 0.0f, 0.0f },
	{ 3.0f, 0.0f },
#endif
};

GLuint texObj[2]; // 0 = rose, 1 = logo

void initTex(void) {
	// make tex objects
	glGenTextures(2, texObj);
	printf("tex object = %d, %d\n", texObj[0], texObj[1]);
	// bind the texture object #0 to the texture unit #0
	glActiveTexture(GL_TEXTURE0); // select tex unit #0
	glBindTexture(GL_TEXTURE_2D, texObj[0]);
	// new for OpenGL 4.5+: load an image to the texture object
	if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
		printf("using OpenGL 4.5 syntax\n");
		glTextureStorage2D(texObj[0], 1, GL_RGBA8, ROSE_IMG_WIDTH, ROSE_IMG_HEIGHT);
		glTextureSubImage2D(texObj[0], 0, 0, 0, ROSE_IMG_WIDTH, ROSE_IMG_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, rose_img);
		glGenerateTextureMipmap(texObj[0]);
		glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameterf(texObj[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else { // original style: load an image to the texture unit (and its associated texture object)
		printf("using OpenGL 3.3+ syntax\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ROSE_IMG_WIDTH, ROSE_IMG_HEIGHT, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, rose_img);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	// bind the texture object #1 to the texture unit #1
	glActiveTexture(GL_TEXTURE1); // select tex unit #1
	glBindTexture(GL_TEXTURE_2D, texObj[1]);
	// new for OpenGL 4.5+: load an image to the texture object
	if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
		printf("using OpenGL 4.5 syntax\n");
		glTextureStorage2D(texObj[1], 1, GL_RGBA8, LOGO_IMG_WIDTH, LOGO_IMG_HEIGHT);
		glTextureSubImage2D(texObj[1], 0, 0, 0, LOGO_IMG_WIDTH, LOGO_IMG_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, logo_img);
		glGenerateTextureMipmap(texObj[1]);
		glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameterf(texObj[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else { // original style: load an image to the texture unit (and its associated texture object)
		printf("using OpenGL 3.3+ syntax\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, LOGO_IMG_WIDTH, LOGO_IMG_HEIGHT, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, logo_img);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	// done
	fflush(stdout);
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
			glTextureParameterf(texObj[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameterf(texObj[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		printf("MAG/MIN_FILTER = NEAREST\n");
		break;
	case GLFW_KEY_2:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameterf(texObj[1], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		printf("MAG/MIN_FILTER = LINEAR\n");
		break;
	case GLFW_KEY_3:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		printf("MIN_FILTER = NEAREST_MIPMAP_NEAREST\n");
		break;
	case GLFW_KEY_4:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		printf("MIN_FILTER = LINEAR_MIPMAP_NEAREST\n");
		break;
	case GLFW_KEY_5:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		printf("MIN_FILTER = NEAREST_MIPMAP_LINEAR\n");
		break;
	case GLFW_KEY_6:
		if (glewIsSupported("GL_VERSION_4_5") && (use_new_tex_syntax == true)) {
			glTextureParameterf(texObj[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameterf(texObj[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} else {
			glActiveTexture(GL_TEXTURE0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glActiveTexture(GL_TEXTURE0 + 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		printf("MIN_FILTER = LINEAR_MIPMAP_LINEAR\n");
		break;
	}
}

system_clock::time_point lastTime = system_clock::now();

void updateFunc(void) {
	system_clock::time_point curTime = system_clock::now();
	milliseconds elapsedTimeMSEC = duration_cast<milliseconds>(curTime - lastTime); // in millisecond
	GLfloat theta = (elapsedTimeMSEC.count() / 1000.0F) * 4 * (float)M_PI; // in <math.h>, M_PI = pi
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// provide the vertex attributes
	GLuint locPos = glGetAttribLocation(prog, "aPos");
	glEnableVertexAttribArray(locPos);
	glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertPos[0]));
	GLuint locTexCoordRose = glGetAttribLocation(prog, "aTexCoordRose");
	glEnableVertexAttribArray(locTexCoordRose);
	glVertexAttribPointer(locTexCoordRose, 2, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(texCoordRose[0]));
	GLuint locTexCoordLogo = glGetAttribLocation(prog, "aTexCoordLogo");
	glEnableVertexAttribArray(locTexCoordLogo);
	glVertexAttribPointer(locTexCoordLogo, 2, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(texCoordLogo[0]));
	// provide texture unit #0 to the texture sampler
	GLuint locTexSamplerRose = glGetUniformLocation(prog, "texSamplerRose");
	glUniform1i(locTexSamplerRose, 0);
	// provide texture unit #1 to the texture sampler
	GLuint locTexSamplerLogo = glGetUniformLocation(prog, "texSamplerLogo");
	glUniform1i(locTexSamplerLogo, 1);
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
	initTex();
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
