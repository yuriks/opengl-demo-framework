#include <GL/glfw.h>
#include <iostream>

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

	if (!glfwOpenWindow(width, height, red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, mode))
	{
		std::cerr << "Failed to open window." << std::endl;
		exit(EXIT_FAILURE);
	}

	static const double fps = 1./60.;
	static const double ups = 1./60.;

	double last_update = 0.;
	double last_render = 0.;
	while (running)
	{
		if (glfwGetTime() - last_render > fps)
		{
			//do rendering stuff, you know
			glClear(GL_COLOR_BUFFER_BIT);
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