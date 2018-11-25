#include <iostream>

//#pragma once

/* glfw and glad includes */
/**/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
/**/

/* framebuffer size callback function */
/*
This needs to be external, as the compiler complains about its type if I place this
defenition inside the Glfw class.
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
/**/

/**/
class InitGlfwGlad {
public:
	GLFWwindow * window;
	int init(int width, int height, const char* title) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		glfwWindowHint(GLFW_VISIBLE, FALSE);


		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// What about a window that doesn't ever get displayed?
		// GLFW has 
		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window);

		/**/
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}
		/**/

		/**/
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		/**/

		return GLFW_TRUE;
	};
	int createInvisibleWindow(int width, int height, const char* title) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_VISIBLE, FALSE);


		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// What about a window that doesn't ever get displayed?
		// GLFW has 
		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window);

		/**/
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}
		/**/

		/**/
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		/**/

		return GLFW_TRUE;
	};

	void setFramebufferSizeCallback() {
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	}

};
/**/



//#include "_initGlfwGlad.h"


int main()
{
	class InitGlfwGlad initGlfwGlad;
	int width{ 800 };
	int height{ 600 };

	initGlfwGlad.init( width, height, "LearnOpenGL");
	/* initializes GLFW, creates a window with gl context, and initializes GLAD */
	GLuint ssbo{ 0 };

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	bool bresult{ FALSE };
	bresult = glIsBuffer(ssbo);
	std::cout << "glIsBuffer(ssbo) returned: " << bresult << std::endl;

	/* Try to examine an SSBO parameter */
	GLint64 pi64result;
	GLint piresult;

	glGetNamedBufferParameteriv(ssbo, GL_BUFFER_USAGE, &piresult);

	//glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &piresult);

	std::cout << "READ_WRITE = " << GL_READ_WRITE << ", and piresult = " << piresult << std::endl;

	/* I am copying code from the slides and modifying it to pass a single GLint */
	/* Setting up the Shader Storage Buffer Objects in Your C Program */
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT ; // GL_MAP_INVALIDATE_BUFFER_BIT;

	GLint bufferOffset{ 0 };

	GLint *data = (GLint *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, bufferOffset, sizeof(GLint), bufMask);
	
	*data = 1; // this is a loop in the slide

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), bufMask);
	GLint readResult{ *data };

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);

	const char *comp_shader_source = R"delim(
#version 430 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout( std140, binding = 2) buffer mydata {
    int number;
};

void main(void)
{
   number = number + 1;
}
	)delim";

	unsigned int computeShader;
	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &comp_shader_source, NULL);
	glCompileShader(computeShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glUseProgram(computeShader);
	glDispatchCompute(1, 1, 1);
	//glDispatchComputeGroupSize(1, 1, 1,1, 1, 1);

	data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), bufMask);
	readResult = *data;

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	std::cout << "Please enter a character: ";
	int i;
	std::cin >> i;

	/** /
	glViewport(0, 0, width, height);

	while ( !glfwWindowShouldClose(initGlfwGlad.window) )
	{
		// Drawing commands go here
		glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// The next two lines are required to make the window show up.
		glfwSwapBuffers(initGlfwGlad.window);
		glfwPollEvents();
		//break;
	}
	/**/

	glfwTerminate();
	return 0;
}

