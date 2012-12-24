#define GLFW_INCLUDE_GL3 1
#include <GL/glfw.h>
#include "GL3/gl3w.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

GLint program;
GLuint buffer;
GLuint vao;

const float vertexPositions[] = {
    0.75f, 0.75f, 0.0f, 1.0f,
    0.75f, -0.75f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint createShader(GLenum type, const char* filename)
{
	GLuint shader = glCreateShader(type);
	std::ifstream file(filename);
	std::string shader_code;
	if (file)
	{
		file.seekg(0, std::ios::end);
		shader_code.resize(static_cast<size_t>(file.tellg()));
		file.seekg(0);
		file.read(&shader_code[0], shader_code.size()); 
	}
	const char* data = shader_code.c_str();
	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* log = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, log);

		const char* strShaderType = NULL;
		switch(type)
		{
		case GL_VERTEX_SHADER:
			strShaderType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			strShaderType = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			strShaderType = "geometry";
			break;
		}
		std::fprintf(stderr, "compile failure in %s shader: \n%s\n", strShaderType, log);
		delete[] log;
	}
	return shader;
}

GLuint createProgram(std::vector<GLuint>& shaderlist)
{
	GLuint program = glCreateProgram();
	for (auto g = shaderlist.begin(); g < shaderlist.end(); ++g)
		glAttachShader(program, *g);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* info = new GLchar[logLength + 1];
		glGetProgramInfoLog(program, logLength, NULL, info);
		fprintf(stderr, "Linker failure:\n%s\n", info);
		delete[] info;
	}

	for (auto g = shaderlist.begin(); g < shaderlist.end(); ++g)
		glDetachShader(program, *g);
	return program;
}

void init()
{
	std::vector<GLuint> shaders;
	shaders.push_back(createShader(GL_VERTEX_SHADER, "vertexshader.glsl"));
	shaders.push_back(createShader(GL_FRAGMENT_SHADER, "fragmentshader.frag"));
	program = createProgram(shaders);
	std::for_each(shaders.begin(), shaders.end(), glDeleteShader);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(buffer, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void display()
{
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

int main(int argc, char** argv)
{
	int running = GL_TRUE;
	
	if(!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		exit(EXIT_FAILURE);
	}

	static const int width = 640;
	static const int height = 480;
	static const int red_bits = 8;
	static const int green_bits = 8;
	static const int blue_bits = 8;
	static const int alpha_bits = 8;
	static const int depth_bits = 24;
	static const int stencil_bits = 8;
	static const int mode = GLFW_WINDOW;

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 3);
	if (!glfwOpenWindow(width, height, red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, mode))
	{
		std::cerr << "Failed to open window." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (gl3wInit())
	{
		std::cerr << "Failed on gl3w init" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!gl3wIsSupported(3, 3))
	{
		std::cerr << "OpenGL 3.3 not supported." << std::endl;
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	init();

	static const double fps = 1./60.;
	static const double ups = 1./60.;

	double last_update = 0.;
	double last_render = 0.;
	while (running)
	{
		if (glfwGetTime() - last_render > fps)
		{
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT);
			display();
			glfwSwapBuffers();
			last_render = glfwGetTime();
		}
		if (glfwGetTime() - last_update > ups)
		{
			//do updating stuff, I guess
			last_update = glfwGetTime();
		}

		running = !glfwGetKey('q') && glfwGetWindowParam(GLFW_OPENED);
		glfwSleep(std::min(fps - (glfwGetTime() - last_render), ups - (glfwGetTime() - last_update)));
	}
}