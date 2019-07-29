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
class Bowl
{
public:

	glm::vec3 bowlPos = glm::vec3(0.0f, 0.0f, 0.0f);
	vector <float> vertices;
	vector <int> indices;
	vector <float> UV;
	string texturePath;

	// Physical attributes
	GLfloat halfmass = 1.5f;
	glm::vec3 bowlSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat angle = 180;
	float Radius = 3.5f;
	float velocity;
	GLfloat timedif;
	GLfloat lastFrame = 0.0f; 
	bool bowlCollision = 0;
	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	bool left = 0;
	bool right = 0;
	bool strike = 0;
	GLfloat delta;
	Bowl(string texturePath)
	{
		this->texturePath = texturePath;
	}
	void drawBawl()
	{
		int Stacks = 20;
		int Slices = 20;

		// CALCULATING VERTICES
		for (int i = 0; i <= Stacks; ++i)
		{
			float V = i / (float)Stacks;
			float phi = V * glm::pi <float>();

			for (int j = 0; j <= Slices; ++j)
			{
				float U = j / (float)Slices;
				float theta = U * (glm::pi <float>() * 2);

				float x = cosf(theta) * sinf(phi);
				float y = cosf(phi);
				float z = sinf(theta) * sinf(phi);

				vertices.push_back(x * Radius);
				vertices.push_back(y * Radius);
				vertices.push_back(z * Radius);
			}
		}

		// CALCULATING INDEX POSITIONS
		for (int i = 0; i < Slices * Stacks + Slices; ++i)
		{

			indices.push_back(i);
			indices.push_back(i + Slices + 1);
			indices.push_back(i + Slices);

			indices.push_back(i + Slices + 1);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		// UV mapping  for the textures
		for (int i = 0; i < vertices.size(); i++)
		{
			if (i % 3 == 0)
			{
				glm::vec3 d = glm::vec3(vertices[i] - bowlPos.x, vertices[i + 1] - bowlPos.y, vertices[i + 2] - bowlPos.z);
				d = glm::normalize(d);
				float u = 1 - (0.5 + (glm::atan(d[2], d[0]) / (glm::pi<float>() * 2)));
				float v = 0.5 - (glm::asin(d[1])) / glm::pi<float>();

				UV.push_back(u);
				UV.push_back(v);
			}
		}
	}

	void drawBowl(Shader ourShader)
	{
		// Build and compile our shader program
		// Shader ourShader("shader.vs", "shader.frag");
		GLuint VAO, VBO, EBO, VBO2;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &VBO2);

		//  BIND VAO
		glBindVertexArray(VAO);

		// ASSIGNING VBO AND EBO POINTERS
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(GLfloat), &UV[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(2);

		// UNBIND VAO
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, texture1);

		GLint modelLoc = glGetUniformLocation(ourShader.CurrentShader, "model");
		//ACTUAL DRAWING	// there is also a translation matrix declared here
		glBindVertexArray(VAO);

		glm::mat4 model;
		model = glm::translate(model, bowlPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// GETTING RID OF EVERYTHING BEFORE NEXT GAME LOOP
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO2);
	}

	GLuint texture1;
	void loadTexture()
	{
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		int text_w, text_h;
		unsigned char* image = SOIL_load_image(texturePath.c_str(), &text_w, &text_h, 0, SOIL_LOAD_RGB);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, text_w, text_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void updateVelocity()
	{
		GLfloat currentFrame = glfwGetTime();
		timedif = currentFrame - lastFrame;
		delta = timedif;
		lastFrame = currentFrame;

			if (strike == 0)
			{
				this->bowlSpeed.x *= 0.974;
				this->bowlSpeed.z *= 0.974;
				this->bowlPos.x += bowlSpeed.x;
				this->bowlPos.z -= bowlSpeed.z;
			}
		if (strike == 1)
		{
			this->bowlSpeed.x -= sin(RADS(this->angle))/  velocity * (delta * 15);
			this->bowlSpeed.z -= cos(RADS(this->angle))/  velocity * (delta * 15);
		}
		if (left == 1)
		{
			this->angle -= 25 * (delta * 20);
			this->left = 0;
		}
		if (right == 1)
		{
			this->angle += 25 * (delta * 20);
			this->right = 0;
		}
	}
	void setPosition(glm::vec3 position)
	{
		this->bowlPos = position;
	}
};