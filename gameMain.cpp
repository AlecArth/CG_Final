// Program Description:
// An open world using OpenGL, with random objects,
// a skybox, and collision detection with sounds.

// How to compile and run:
// Make all
// ./game

// Controls
// Esc - Escape/Exit
// W - forward
// S - backward
// A - strafe right
// D - strafe left
// Q - turn counter clockwise
// E - turn clockwise

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
// #include "SDL2/SDL.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Game.h"
#include "Shader.hpp"
#include "Parser.h"
// #include "SDL2_mixer/SDL_mixer.h"

Game *GameObject;

#define PI 3.14159265

float yRot = (PI/4)*3; // Camera starts looking down the x+ axis.
int zLoc = 0;
int xLoc = 0;
int winX = 1700;
int winY = 1700;
int toggleTex = 0;
std::vector < std::vector <int> > GameMap;

unsigned int programID;

using namespace std;

void moveDir(std::string direction);

// int playSound(std::string sound){
// 	// Init
// 	if (SDL_Init(SDL_INIT_AUDIO) != 0){
// 		std::cerr << "ERROR: " << SDL_GetError() << std::endl;
// 		return -1;
// 	}
// 	// Open Audio device
// 	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) != 0){
// 		std::cerr << "ERROR: " << Mix_GetError() << std::endl;
// 		return -1;
// 	}
// 	// Set Volume
// 	Mix_VolumeMusic(100);
// 	// Open Audio File
// 	Mix_Music* audio = Mix_LoadMUS("walking.ogg");
// 	if(sound == "crash"){
// 		audio = Mix_LoadMUS("crash.ogg");
// 	}
// 	if (audio){
// 		// Start Playback
// 		if (Mix_PlayMusic(audio, 1) == 0){
// 			// Wait
// 			while (Mix_PlayingMusic()){}
// 		}
// 		else{
// 			std::cerr << "ERROR: " << Mix_GetError() << std::endl;
// 		}
// 		// Free File
// 		Mix_FreeMusic(audio);
// 		audio = 0;
// 	}
// 	else{
// 		std::cerr << "ERROR: " << Mix_GetError() << std::endl;
// 	}
// 	// End
// 	Mix_CloseAudio();
// }

// Callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		if(toggleTex == 0){
			programID = 3;
			toggleTex = 1;
		}
		else if (toggleTex == 1) {
			programID = 6;
			toggleTex = 0;
		}
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		GameObject->toggleTexMode(); // toggle texture sampling
	}
	// Move around using the arrow keys
	else if (key == GLFW_KEY_Q) {
		yRot -= .1;
		if( yRot < 0 ) yRot = PI*2;
	}
	else if (key == GLFW_KEY_E) {
		yRot += .1;
		if( yRot > (PI*2) ) yRot = 0;
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		moveDir("forward");
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		moveDir("backward");
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		moveDir("strafe_left");
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		moveDir("strafe_right");
	}
}
// Set the projection matrix. Also called when the window is resized.
void setProjection()
{
	glm::mat4 projection;
	// glEnable(GL_DEPTH_CLAMP);
	glUseProgram(programID);

	projection = glm::perspective( 1.0f, 1.0f, 0.2f, 30.0f );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(programID, "projection_matrix");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );

}

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )
{
	winX = x;
	winY = y;
	setProjection();
	glViewport( 0, 0, x, y );
}

// Determines direction to move, based on current angle.
void moveDir(string direction){
	std::vector<int> tempvec;
	int GameSize = GameMap.size();
	int zLocOld = zLoc;
	int xLocOld = xLoc;
	if( yRot > (PI/4)*7 || yRot <= (PI/4) ){ // q1
		if(direction == "forward") zLoc = zLoc + 1;
		else if(direction == "backward") zLoc = zLoc - 1;
		else if(direction == "strafe_left")xLoc = xLoc + 1;
		else if(direction == "strafe_right")xLoc = xLoc - 1;
	}
	else if(yRot > (PI/4) && yRot <= (PI/4)*3){ // q2
		if(direction == "forward") xLoc = xLoc - 1;
		else if(direction == "backward") xLoc = xLoc + 1;
		else if(direction == "strafe_left")zLoc = zLoc + 1;
		else if(direction == "strafe_right")zLoc = zLoc - 1;
	}
	else if(yRot > (PI/4)*3 && yRot <= (PI/4)*5){ // q3
		if(direction == "forward") zLoc = zLoc - 1;
		else if(direction == "backward") zLoc = zLoc + 1;
		else if(direction == "strafe_left")xLoc = xLoc - 1;
		else if(direction == "strafe_right")xLoc = xLoc + 1;
	}
	else if(yRot > (PI/4)*5 && yRot <= (PI/4)*7){ // q4
		if(direction == "forward") xLoc = xLoc + 1;
		else if(direction == "backward") xLoc = xLoc - 1;
		else if(direction == "strafe_left")zLoc = zLoc - 1;
		else if(direction == "strafe_right")zLoc = zLoc + 1;
	}
	// This code will stop the player going outside the bounds or through a wall.
	tempvec = GameMap[zLoc-(zLoc*2)];
	if( zLoc > 0 || xLoc > 0 || tempvec[xLoc-(xLoc*2)] == 1) {
		zLoc = zLocOld;
		xLoc = xLocOld;
		// playSound("crash");
	}
	else{
		// playSound("walking");
	}
}
// Main render function
void render()
{
	// Update the camera, and draw the scene.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	// glDepthMask(false);


	glUseProgram(programID);

	// We reset the camera for this frame
	glm::mat4 viewMatrix;
	viewMatrix = glm::rotate(viewMatrix, yRot, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::translate( viewMatrix, glm::vec3(xLoc - 1, -1.0f, zLoc - 1));

	// Load it to the shader program
	int viewHandle = glGetUniformLocation(programID, "modelview_matrix");
	if (viewHandle == -1) {
		std::cout << "Uniform: modelview_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

	if(programID == 3){
		programID = 9;
		glUseProgram(programID);
		// Load it to the shader program
		int viewHandle2 = glGetUniformLocation(programID, "modelview_matrix");
		if (viewHandle2 == -1) {
			std::cout << "Uniform: modelview_matrix is not an active uniform label\n";
		}
		glUniformMatrix4fv( viewHandle2, 1, false, glm::value_ptr(viewMatrix) );
		programID = 3;
		glUseProgram(programID);
	}
	// Lighting.
	int storeID = programID;
	programID = 12;
	glUseProgram(programID);

	glm::mat3 normMatrix;
	normMatrix = glm::mat3(viewMatrix);
	// Load it to the shader program
	int viewHandle3 = glGetUniformLocation(programID, "modelview_matrix");
	int normHandle = glGetUniformLocation(programID, "normal_matrix");
	int lightposHandle = glGetUniformLocation(programID, "light_pos");
	if (viewHandle3 == -1 || normHandle==-1 || lightposHandle == -1) {
		fprintf(stderr, "Error: can't find matrix uniforms\n");
		exit(1);
	}
	glUniformMatrix4fv( viewHandle3, 1, false, glm::value_ptr(viewMatrix) );
	glUniformMatrix4fv( normHandle, 1, false, glm::value_ptr(normMatrix) );

	// Update the light position
	float lightPos[4] = { (float)xLoc, 0.0f, (float)zLoc, 1.0f };
	glUniform4fv(lightposHandle, 1, lightPos);

	programID = storeID;
	glUseProgram(programID);


	GameObject->render(programID, GameMap, xLoc, zLoc);

}

// Error callback for GLFW. Simply prints error message to stderr.
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
// Sets the lighting uniform variables. Done once at initialisation.
// @return success on 0, failure otherwise
int setupShader(unsigned int id)
{
	glUseProgram(id);

	// Uniform lighting variables
	int lightambientHandle = glGetUniformLocation(id, "light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(id, "light_diffuse");
	int lightspecularHandle = glGetUniformLocation(id, "light_specular");
	if ( lightambientHandle == -1 ||
		 lightdiffuseHandle == -1 ||
		 lightspecularHandle == -1) {
		fprintf(stderr, "Error: can't find light uniform variables\n");
		return 1;
	}

	float lightambient[3] = { 0.12f, 0.1f, 0.1f };  // ambient light components
	float lightdiffuse[3] = { 0.0f, 1.0f, 0.0f };   // diffuse light components
	float lightspecular[3] = { 1.0f, 1.0f, 1.0f };  // specular light components

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);

	// Uniform variables defining material colours
	// These can be changed for each sphere, to compare effects
	int mtlambientHandle = glGetUniformLocation(id, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(id, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(id, "mtl_specular");
	if ( mtlambientHandle == -1 ||
		 mtldiffuseHandle == -1 ||
		 mtlspecularHandle == -1) {
		fprintf(stderr, "Error: can't find material uniform variables\n");
		return 1;
	}

	float mtlambient[3] = { 0.12f, 0.1f, 0.1f };    // ambient material
	float mtldiffuse[3] = { 0.0f, 1.0f, 0.0f }; // diffuse material
	float mtlspecular[3] = { 1.0f, 1.0f, 1.0f };    // specular material

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);

	return 0;   // return success
}

int main (int argc, char **argv)
{
	// Controls
	cout << "Esc - Escape/Exit" << endl;
	cout << "W - forward" << endl;
	cout << "S - backward" << endl;
	cout << "A - strafe right" << endl;
	cout << "D - strafe left" << endl;
	cout << "Q - turn counter clockwise" << endl;
	cout << "E - turn clockwise" << endl;

	int GameSize;
	parser input;
	// Parse input file.
	GameMap = input.parseInput(argc, argv);
	input.printMatrix(GameMap);

	GameSize = GameMap.size();

	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(1);
	}

	// Specify that we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window and OpenGL context
	window = glfwCreateWindow(winX, winY, "Modelling and viewing", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Set OpenGL state we need for this application.
	glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);

	// Set up the shaders we are to use. 0 indicates error.
	programID = LoadShaders("gameWalls.vert", "gameWalls.frag");
	if (programID == 0) {
		fprintf(stderr, "Error loading shaders\n");
		exit(1);
	}
	setProjection();

	programID = LoadShaders("texture.vert", "texture.frag");
	if (programID == 0) {
		fprintf(stderr, "Error loading shaders\n");
		exit(1);
	}
	setProjection();

	programID = LoadShaders("gameFloor.vert", "gameFloor.frag");
	if (programID == 0) {
		fprintf(stderr, "Error loading shaders\n");
		exit(1);
	}
	setProjection();


	programID = LoadShaders("pf-light.vert", "pf-light.frag");
	if (programID == 0) {
		fprintf(stderr, "Can't compile shaders!\n");
		exit(1);
	}
	if (setupShader(programID) != 0) {
	   fprintf(stderr, "Can't initialise shaders!\n");
		exit(1);
	}
	setProjection();

	// Set up the scene and the cameras
	programID = 6;
	GameObject = new Game( programID );

	// glEnable(GL_COLOR_MATERIAL);

	// Define callback functions and start main loop
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);

	while (!glfwWindowShouldClose(window))
	{
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	delete GameObject;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(0);

	return 0;
}
