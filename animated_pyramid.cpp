#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SOIL2/SOIL2.h"

//GLM Math Header inclusions
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <GL/glm/gtc/type_ptr.hpp>


using namespace std;			//Standard Namespace

#define WINDOW_TITLE "Textured Animated Pyramid" 			//Window title


//Shader program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif


//Variable declarations for shader, window size intialization, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, texture;
GLfloat degrees = glm::radians(0.0f);		//Convert float to degrees


//Function Prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);


//Vertex Shader Source Code
const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;			//Vertex data from attrib pointer 0
		layout (location = 2) in vec2 textureCoordinate;

		out vec2 mobileTextureCoordinate;

		//Global variables for the transform matrices
			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(position, 1.0f);		//transforms vertices to clip coordinates
			mobileTextureCoordinate = vec2(textureCoordinate.x, 1.0f - textureCoordinate.y);			//flips texture horizontal
		}
	);

//Fragment Shader Source Code
const GLchar * fragmentShaderSource = GLSL(330,

		in vec2 mobileTextureCoordinate;

		out vec4 gpuTexture;						//variable to pass color data to GPU

		uniform sampler2D uTexture;				//Useful when working with multiple textures

	void main() {
			gpuTexture = texture(uTexture, mobileTextureCoordinate);
		}
	);



//*******MAIN PROGRAM*******
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);


	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
			{
				std::cout<< "Failed to initialize GLEW" << std::endl;
				return -1;
			}

	UCreateShader();
	UCreateBuffers();
	UGenerateTexture();

	//Use the Shader Program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 			//Set background color

	glutDisplayFunc(URenderGraphics);

	glutMainLoop();

	//Destroys Buffer Objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);


	return 0;
}

//Resize Window
void UResizeWindow(int w, int h) {
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);

}

//Renders graphics
void URenderGraphics(void) {

	glEnable(GL_DEPTH_TEST);			//Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			//Clears the screen

	glBindVertexArray(VAO);				//Activates the vertex arra object before rendering and transforming them

	//Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));		//place the object at the center  of the viewport

	model = glm::rotate(model, glutGet(GLUT_ELAPSED_TIME) * -0.0005f, glm::vec3(0.0f, 1.0f, 0.0f));	//rotate the object 45 degrees on the x axis
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));			//sets scale of an object to 2

	//Tramsform the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));			//Moves world .5 on X, -.5 on Z


	//Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the SHader Program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	glBindTexture(GL_TEXTURE_2D, texture);

	//Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);					//Deactivates the vertex array object

	glutSwapBuffers();						//Flips the back buffer with the front buffer ever frame.

}


//Creates the Shader Program
void UCreateShader(){


	//Vertex Shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);	//Creates the vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);			//Attaches vertex shader to source code
	glCompileShader(vertexShader);						//Compiles Fragment Shader


	//Fragment Shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	//Creates the fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);			//Attaches fragment shader to source code
	glCompileShader(fragmentShader);							//Compiles fragment shader

	//Shader Program
	shaderProgram = glCreateProgram(); 			//Creates the shader program and returns an id
	glAttachShader(shaderProgram, vertexShader);		//Attach vertex shader to shader program
	glAttachShader(shaderProgram, fragmentShader);		//Attach a fragment shader to the shader program
	glLinkProgram(shaderProgram);				//Link vertex and fragment shaders to shader program

	//Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void UCreateBuffers() {

	GLfloat vertices[] = {

					//Positions				//Normals				//Texture Coordinates
					//Back Face
					-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,		0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,		1.0f, 0.0f,
					 0.0f,  0.5f,  0.0f,    0.0f,  0.0f, -1.0f,		0.5f, 1.0f,

					 //Left Face
					 0.0f,  0.5f,  0.0f,    -1.0f,  0.0f, 0.0f, 	0.5f, 1.0f,
					-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f, 0.0f,		0.0f, 0.0f,
					-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f, 0.0f, 	1.0f, 0.0f,

					//Front Face
					-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,		0.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f, 	1.0f, 0.0f,
					 0.0f,  0.5f,  0.0f,    0.0f,  0.0f,  1.0f,		0.5f, 1.0f,

					 //Right Face
					 0.0f,  0.5f,  0.0f,    1.0f,  0.0f,  0.0f, 	0.5f, 1.0f,
					 0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f, 	1.0f, 0.0f,

					 //Bottom Face
					 0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,		0.0f, 0.0f,
					-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f, 	1.0f, 0.0f,
					-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,		1.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f, 	0.0f, 1.0f
			};

	//generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//Activate the vertex array object before binding it to any VBO's/ attrib pointers
	glBindVertexArray(VAO);


	//Activate VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);			//copy vertices to VBO

	//Set attribute poiner 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0); 			//enables vertex attribute

	//Set attribute pointer 1 to hold Normal data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	//Set attribute poiner 1 to hold color data
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2); 			//enables vertex attribut

		glBindVertexArray(0);			//Deactivates VAO which is good practice
}


void UGenerateTexture() {

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);


	int width, height;

	unsigned char* image = SOIL_load_image("brick_pic.jpg", &width, &height, 0, SOIL_LOAD_RGB); 		//Loads texture file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);								//unbind texture
}
