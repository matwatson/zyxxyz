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
	class InitGlfwGlad initGlfwGlad; /* InitGlfwGlad, a class to encapsulate initialization code. */
	int width{ 800 };
	int height{ 600 };

	/* initGlfwGlad.init(), initializes GLFW, creates a window with gl context, and initializes GLAD */
	initGlfwGlad.init( width, height, "LearnOpenGL");

	GLuint ssbo{ 0 };

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	bool bresult{ FALSE };
	bresult = glIsBuffer(ssbo);
	std::cout << "glIsBuffer(ssbo) returned: " << bresult << std::endl;

	/* Try to examine an SSBO parameter */
	//GLint64 pi64result;
	GLint piresult;

	glGetNamedBufferParameteriv(ssbo, GL_BUFFER_USAGE, &piresult);

	/* I recall that GLAD did not support this particular function for some reason. */
	//glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &piresult);

	/* piresult has the value 35004, which corresponds to GL_BUFFER_MAPPED 0x88BC */
	std::cout << "READ_WRITE = " << GL_READ_WRITE << ", and piresult = " << piresult << std::endl;

	/* 
	glBuffer Data - creates and initializes a buffer object's data store.
	The buffer object is bound to the target:  GL_SHADER_STORAGE_BUFFER
	creates storage of size: 4*sizeof(GLint)
	the data argument is: NULL, which means no data is to be copied for initializaion purposes
	the usage argument is: GL_STATIC_DRAW,
	usage is a hint. This hint indicates modified once / read many (STATIC) and used as the source from drawing commands (DRAW)
	*/
	//glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLint), NULL, GL_STATIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT ; // GL_MAP_INVALIDATE_BUFFER_BIT;

	GLint bufferOffset{ 0 };

	//GLint *data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, bufferOffset, 4 * sizeof(GLint), bufMask);
	GLint *data{ 0 };
	data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, bufferOffset, sizeof(GLint), bufMask);

	/* initialize the first three slots begining at *data */	
	*data = 31; // A prime number. The first two digits Pi. */


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	data = (GLint *)0;
	data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), bufMask);
	{
		GLint readResult{ *data };
		std::cout << "readResult should equal 31. readResult = " << readResult << std::endl;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);

	/* The compute shader code comprises the text between the string literal delimiters "delim( and )delim" */
	/* I recall std430 avoids alignment issues. That should not make a difference here because there is only one number. */
	const char *comp_shader_source = R"delim(
#version 430 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout( std430, binding = 2) buffer mydata {
    int mynumber;
};

void main(void)
{
   mynumber = 13;
}
)delim";
	/* end of compute shader source code */

	unsigned int computeShader;
	/* glCreateShader - creates an empty shader object and returns a non-zero value by which it can be referenced. */
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

	{
		GLuint block_index{ 0 };
		block_index = glGetProgramResourceIndex(computeShader, GL_SHADER_STORAGE_BLOCK, "mydata");

		std::cout << "after shader compilation, the block index correspoding to mydata is, block_index = " << block_index << std::endl;
		if (block_index == GL_INVALID_INDEX) std::cout << "block_index == GL_INVALID_INDEX" << std::endl;
	}

	glDispatchCompute(1, 1, 1);
	//glDispatchComputeGroupSize(1, 1, 1,1, 1, 1);
	glFinish();
	glFlush();


	//data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(GLint), bufMask);
	data = (GLint *)0;
	data = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), bufMask);
	{
		GLint readResult{ *data };
		std::cout << "readResult should now equal 13. readResult = " << readResult << std::endl;
	}

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

