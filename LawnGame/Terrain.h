#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "SOIL2\SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shaders.h"
#include <vector>
#include <string>

using namespace std;
GLuint texture;

class Terrain
{
public:
	glm::vec3 terrainPos;
	vector <float> vertices;
	vector <int> indices;
	string texturePath;


	Terrain(glm::vec3 position, string texturePath, GLfloat bufferValues[], GLint indicesValues[])
	{
		this->terrainPos = position;
		this->texturePath = texturePath;

		for (int i = 0; i < 100; i++)
			vertices.push_back(bufferValues[i]);
		for (int i = 0; i < 30; i++)
			indices.push_back(indicesValues[i]);
	}

	void drawTerrain(Shader ourShader)
	{

		GLuint VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);		// VBO allows to pass vertices values into the shader!!!
		glGenBuffers(1, &EBO);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		// ASSIGNING VBO AND EBO POINTERS
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		// EBO is similar to VBO in a way that it also allows to pass user defined values for the vertices but we have to specify
														// an array of indices in order to draw different 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), &indices[0], GL_STATIC_DRAW);

		// position coordinate attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);		// 6 * sizeof(GLfloat) if we are including colors and texture coords besides our vertex position coordinates //
		glEnableVertexAttribArray(0);

		// texture coordinate attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);		// the number must be incremented every time you call attribute arrays

											// UNBIND VAO
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, texture);

		GLint modelLoc = glGetUniformLocation(ourShader.CurrentShader, "model");
		//ACTUAL DRAWING	// there is also a translation matrix declared here
		glBindVertexArray(VAO);

		glm::mat4 model;
		model = glm::translate(model, terrainPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}



	void loadTexture()
	{
		int text_width, text_height;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		unsigned char* image = SOIL_load_image(texturePath.c_str(), &text_width, &text_height, 0, SOIL_LOAD_RGBA);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_width, text_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};
