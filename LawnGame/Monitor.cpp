#define GLEW_STATIC
#define PI 3.14159265
#define RADS(x) (x * PI / 180.0)
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/detail/type_vec.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Shaders.h"
#include "Bawls.h"
#include "Terrain.h"
#include "SOIL2/SOIL2.h"  //SOIL
#include <glfw/glfw3.h> //GLFW
bool camera1 = true;
bool camera2 = false;
float power;
glm::vec3 cameraPos;
glm::vec3 cameraPos2;
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

bool bowlsCollision = 0;			// checks if any kind of ball collision is happening
bool space = 0;

// Window dimensions
const GLuint WIDTH = 1024, HEIGHT = 800;		// predefined windows sizes

string texture_paths[5] = { { "./Textures/cray2.bmp" },{ "./Textures/Cage.jpg" }, { "./Textures/star.bmp" }, {"./Textures/launch.bmp" }, { "./Textures/grass.bmp" } };

GLfloat terrainV[] = //vertices
{
	// vertex position			// texture coordinates
	70.0f,  -1.0f, -140.0f,		1.0f, 1.0f,
	70.0f, -1.0f,   140.0f,		1.0f, 0.0f,
	-70.0f, -1.0f,  140.0f,		0.0f, 0.0f,
	-70.0f, -1.0f, -140.0f,		0.0f, 1.0f,

	// Ending 
	70.0f, -1.0f, 140.0f,        1.0f, 0.0f,
	90.0f, +5.0f, 140.0f,      1.0f, 1.0f,
	70.0f, -1.0f, -140.0f,       0.0f, 1.0f,
	90.0f, +5.0f, -140.0f,     0.0f, 0.0f,


};
// Linking
GLint terrainI[] =
{
	// Base
	0, 1, 3,
	1, 2, 3,

	// Left 
	4, 5, 6,
	5, 7, 6,

	// Right 
	8, 9, 10,
	9, 11, 10,

	// Bottom 
	12, 13, 14,
	13, 15, 14,

	// Top 
	16, 17, 18,
	17, 19, 18
};

Terrain thisTerrain(glm::vec3(0.0, -1.3, 0.0), texture_paths[4], terrainV, terrainI);			// terrain constructor call
																													

vector <Bowl> bowls;			// collection of all the bowls

								
								// FURSTRUM / PROJECTION VIEW PRORIETIES 
glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);




void setBowlsInitPosition()
{
	for (int i = 0; i <= 3; i++) // ADD N TEXTURES ON BOWLS + Spawn
	{
		Bowl myBowl(texture_paths[i]);
		myBowl.drawBawl();
		myBowl.loadTexture();
		bowls.push_back(myBowl);
	}

	//float d = 2.3;		// distance between the bowls inside the bowl triangle formation

	bowls[0].bowlPos.z = 60.0;
	bowls[1].bowlPos.x = 24.0;
	bowls[2].bowlPos.z = 10.0;
	bowls[3].bowlPos.x = 40.0;
}

void collision(int check, Bowl i)
{

	for (int j = 0; j < bowls.size(); j++)
	{
		if (j == check)
		{
			j++;
		}
		if (glm::distance(bowls[j].bowlPos, i.bowlPos) <= (2 * i.Radius))
		{
			if (!glm::distance(bowls[j].bowlPos, i.bowlPos) == 0)
			{
				i.bowlCollision = 1;
				bowls[j].bowlCollision = 1;
				if (i.bowlCollision == 1 && bowls[j].bowlCollision == 1)
				{

					glm::vec3 OppDirection = 0.1f *(bowls[j].bowlPos - i.bowlPos);
					bowls[j].bowlPos += OppDirection;
					i.bowlPos -= OppDirection;

					glm::vec3 colPoint = glm::vec3((i.bowlPos.x + bowls[j].bowlPos.x) / 2, 0.0f, (i.bowlPos.z + bowls[j].bowlPos.z) / 2);
					glm::vec3 colPC = colPoint - i.bowlPos;

					glm::vec3 otherBowlL = glm::normalize(colPC);
					glm::vec3 thisBowlL = glm::normalize(glm::cross(colPC, glm::vec3(0.0f, 1.0f, 0.0f)));

					i.force = thisBowlL;
					bowls[j].force = otherBowlL;

					glm::vec3 thisBowlNewVel;
					glm::vec3 otherBowlNewVel;

					thisBowlNewVel.x = (i.bowlSpeed.x + (2 * bowls[j].Radius * bowls[j].bowlSpeed.x)) / (i.Radius + bowls[j].Radius);
					thisBowlNewVel.y = 0.0f;
					thisBowlNewVel.z = (i.bowlSpeed.z + (2 * bowls[j].Radius * bowls[j].bowlSpeed.z)) / (i.Radius + bowls[j].Radius);

					otherBowlNewVel.x = (bowls[j].bowlSpeed.x + (2 * i.Radius * i.bowlSpeed.x)) / (i.Radius + bowls[j].Radius);
					otherBowlNewVel.y = 0.0f;
					otherBowlNewVel.z = (bowls[j].bowlSpeed.z + (2 * i.Radius * i.bowlSpeed.z)) / (i.Radius + bowls[j].Radius);



					i.bowlSpeed.x = i.force.x*sqrt(thisBowlNewVel.x*thisBowlNewVel.x + thisBowlNewVel.y*thisBowlNewVel.y + thisBowlNewVel.z*thisBowlNewVel.z);
					i.bowlSpeed.z = i.force.z*sqrt(thisBowlNewVel.x*thisBowlNewVel.x + thisBowlNewVel.y*thisBowlNewVel.y + thisBowlNewVel.z*thisBowlNewVel.z);

					bowls[j].bowlSpeed.x = bowls[j].force.x*sqrt(otherBowlNewVel.x*otherBowlNewVel.x + otherBowlNewVel.y*otherBowlNewVel.y + otherBowlNewVel.z*otherBowlNewVel.z);
					bowls[j].bowlSpeed.z = bowls[j].force.z*sqrt(otherBowlNewVel.x*otherBowlNewVel.x + otherBowlNewVel.y*otherBowlNewVel.y + otherBowlNewVel.z*otherBowlNewVel.z);

					i.bowlSpeed.x -= sin(RADS(i.angle)) / 20 * (i.velocity * 10);
					i.bowlSpeed.z -= cos(RADS(i.angle)) / 20 * (i.velocity * 10);
					bowls[j].bowlSpeed.x -= sin(RADS(bowls[j].angle)) / 10 * (bowls[j].velocity * 10);
					bowls[j].bowlSpeed.z -= cos(RADS(bowls[j].angle)) / 10 * (bowls[j].velocity * 10);

					i.bowlCollision = 1;
					bowls[j].bowlCollision = 1;
				}
			}
		}
	}
}



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//Shader theShader("VertexSH.vs", "FragmentSH.frag");
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Q)
	{
		bowls[0].velocity -= 1.0f;
		if (bowls[0].velocity <= 0)
		{
			cout << "Velocity is blocked at level 0" << endl;
			bowls[0].velocity = 0; // refresh
		}
		else
		{
			cout << "Velocity set to:" << bowls[0].velocity << endl;
		}
	}
	if (key == GLFW_KEY_E)
	{
		bowls[0].velocity += 1.0f;
		if (bowls[0].velocity >= 25.0f)
		{
			cout << "Velocity blocked at: Level 25" << endl;
			bowls[0].velocity = 25.0f; // refresh
		}
		else
		{
			cout << "Velocity set to:" << bowls[0].velocity << endl;
		}
	}


	if (key == GLFW_KEY_SPACE && action == GLFW_REPEAT)
	{
		bowls[0].strike = 1;

	}

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		if (power < 0.6f)
		{
			bowls[0].bowlSpeed += power;
			bowls[0].strike = 0;
		}
		if (power >= 0.6f)
		{
			bowls[0].bowlSpeed += power;
			bowls[0].bowlPos.y += 6;
			bowls[0].strike = 0;
		}

		power = 0.0f;
	}
	if (key == GLFW_KEY_A && action == GLFW_REPEAT)

	{
		bowls[0].left = 0.2;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		bowls[0].left = 0;
		bowls[0].angle = bowls[0].angle;
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT)
	{
		bowls[0].right = 0.2;

	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		bowls[0].right = 0;
		bowls[0].angle = bowls[0].angle;
	}
	if (key == GLFW_KEY_V)
	{
		camera1 = false;
		camera2 = true;
		cout << "Switch to Camera 2" << endl;
	}
	if (key == GLFW_KEY_B)
	{
		camera2 = false;
		camera1 = true;
		cout << "Switch to Camera 1" << endl;
	}
}
int main()
{

	std::cout << "Welcome To Lawn-Game, Ver. 0.07a" << std::endl;
	cout << "Press V / B to Change Between Cameras \n Press Space To Strike, \n A / D for directions" << endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);			// We set the version to 3.3 of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);			//
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		// "GLFW_OPENGL_CORE_PROFILE" enables us to use the modern OpenGL, if we were to change this value to "GLFW_OPENGL_COMP_PROFILE"
																		// then we would be allowed to use legacy OpenGL functionalities
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);							// sets if the window can be resizable

																		// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LawnGame", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);		// depth test so that the textures and colors won't overlapse 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (window == nullptr)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialise glew." << std::endl;
		return EXIT_FAILURE;
	}

	Shader thisShader("VertexSH.vs", "FragmentSH.frag");// loading the shaders 
															
	thisTerrain.loadTexture();


	setBowlsInitPosition();		// placing the jack

//  loop
	while (!glfwWindowShouldClose(window))
	{
		// checking wall collision using vector reflections
		for (int i = 0; i < 3; i++)
		{
			if (bowls[i].bowlPos.x < -70 || bowls[i].bowlPos.x > 70)
				bowls[i].bowlSpeed.x = -(bowls[i].bowlSpeed.x);   // Barrier + BounceBack
			if (bowls[i].bowlPos.z < -140 || bowls[i].bowlPos.z > 140)
				bowls[i].bowlSpeed.z = -(bowls[i].bowlSpeed.z);   // Barrier + BounceBack
		}

		// Events
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.5f, 0.8f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// depth test so that the textures won't overlapse

		thisShader.Use();

		glm::mat4 view, model, projection;

		cameraPos = glm::vec3(bowls[0].bowlPos.x + 10 * sin(RADS(bowls[0].angle)), bowls[0].bowlPos.y + 5, bowls[0].bowlPos.z - 10 * cos(RADS(bowls[0].angle)));
		cameraPos2 = glm::vec3(0.0f, 40.0f, 0.0f);

		if (camera1 == true)
		{
			view = glm::lookAt(cameraPos2, bowls[1].bowlPos, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (camera2 == true)
		{
			view = glm::lookAt(cameraPos, bowls[0].bowlPos, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		model = glm::rotate(model, -120.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		projection = glm::perspective(90.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);



		GLint viewLoc = glGetUniformLocation(thisShader.CurrentShader, "view");
		GLint projLoc = glGetUniformLocation(thisShader.CurrentShader, "projection");
		GLint modelLoc = glGetUniformLocation(thisShader.CurrentShader, "model");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// terrain
		thisTerrain.drawTerrain(thisShader);

		// Drawing the jack
		for (int i = 0; i < 3; i++)
		{
			bowls[i].drawBowl(thisShader);
		}
		// COLLISION 
		for (int j = 0; j < 2; j++)
		{
			collision(j, bowls[j]);
		}
		for (int s = 0; s < 2; s++)
		{
			bowls[s].updateVelocity();
		}

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}