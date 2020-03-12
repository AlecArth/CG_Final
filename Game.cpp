
#include "Game.h"

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SQUARE_NUM_VERTICES 4
#define SQUARE_NUM_TRIS 2

#define CUBE_NUM_VERTICES 8
#define CUBE_NUM_TRIS 12
#define VALS_PER_VERT 3

#define NUM_TEXTURES 8
#define TEX_SIZE 64  // resolution of our constructed texture image
#define PI 3.14159265

#define VALS_PER_TEX 2


// Variables defining texture settings
static GLenum CurrentTexSample = GL_NEAREST;
static char   CurrentTexString[32] = "GL_NEAREST";

static GLenum CurrentTexMode = GL_REPLACE;
static char   CurrentTexModeString[32] = "GL_REPLACE";

GLuint TexID[NUM_TEXTURES];
static GLuint CurrentWallsTex = 0;
static GLuint CurrentFloorTex = 1;

// texture image data
GLubyte myimage[TEX_SIZE][TEX_SIZE][3];

unsigned int vaoGameEndMarkerHandle;
unsigned int vaoGameHandle;

using namespace std;


// Creates a new vertex array object for a cube
// and loads in data into a vertex attribute buffer
int createGameVAO(int programID)
{

	// Cube vertex data
	float Vertices[ CUBE_NUM_VERTICES * VALS_PER_VERT ] = {
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0  };

	// Texture coords for each vertex. 2 per vertex.
	float tex_coord[CUBE_NUM_VERTICES * VALS_PER_TEX] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	};

	// 12 triangles - 2 per face of the cube
	unsigned int Indices[CUBE_NUM_TRIS*3] = {
		0,1,2, 2,3,0,
		1,5,6, 6,2,1,
		5,4,7, 7,6,5,
		4,0,3, 3,7,4,
		3,2,6, 6,7,3,
		4,5,1, 1,0,4
	};

	// glUseProgram(programID);
	glGenVertexArrays(1, &vaoGameEndMarkerHandle);
	glBindVertexArray(vaoGameEndMarkerHandle);

	int vertLoc = glGetAttribLocation(programID, "a_vertex");
	int texLoc = glGetAttribLocation(programID, "a_tex_coord");

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return vaoGameEndMarkerHandle;
}
// VAO for floor and exterior walls
int createGameFloorVAO(int programID)
{

	// Cube vertex data
	float Vertices[ SQUARE_NUM_VERTICES * VALS_PER_VERT ] = {
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0, };

	// Texture coords for each vertex. 2 per vertex.
	float tex_coord[SQUARE_NUM_VERTICES * VALS_PER_TEX] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	};

	// 12 triangles - 2 per face of the cube
	unsigned int Indices[SQUARE_NUM_TRIS*3] = {
		0,1,2, 2,3,0,
	};

	// glUseProgram(programID);
	glGenVertexArrays(1, &vaoGameHandle);
	glBindVertexArray(vaoGameHandle);

	int vertLoc = glGetAttribLocation(programID, "a_vertex");
	int texLoc = glGetAttribLocation(programID, "a_tex_coord");

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return vaoGameHandle;
}

int createTextures()
{
	// A shader program has many texture units,
	// slots in which a texture can be bound, available to
	// it and this function defines which unit we are working with currently
	// We will only use unit 0 here. This is the default.
	glActiveTexture(GL_TEXTURE0);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	glGenTextures( NUM_TEXTURES, TexID );
	
	// create checkerboard image
	// We don't really need RGB channels for this image, but use them anyway.
	for(int i=0; i < TEX_SIZE; i++){
		for(int j=0; j < TEX_SIZE; j++)
		{
			GLubyte c;
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			myimage[i][j][0]  = c;
			myimage[i][j][1]  = c;
			myimage[i][j][2]  = c;
		}                
	}

	int x, y, n;
	unsigned char *data;
	const char* conv_my_str;
	string tex_num;

	for(int i = 0; i<8; i++){

		tex_num = std::to_string(i);
		tex_num = "textures/" + tex_num + ".jpg";
		conv_my_str = tex_num.c_str();

		// load an image from file as texture sky
		data = stbi_load(
						conv_my_str, // char* filepath
						&x, // The address to store the width of the image
						&y, // The address to store the height of the image
						&n  // Number of channels in the image,
						0   // Force number of channels if > 0
		);

		glBindTexture( GL_TEXTURE_2D, TexID[i] );
		if (n == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.");
		}
		stbi_image_free(data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	return 0;
}

// calculates the table's dimensions based on the desired height and width.
Game::Game(int id)
{
	vaoGameHandle = createGameFloorVAO(id);
	vaoGameEndMarkerHandle = createGameVAO(id);
	createTextures();
}

void Game::toggleTex(){

	// CurrentWallsTex = ( (CurrentWallsTex==NUM_TEXTURES-1) ? 0 : CurrentWallsTex+1 );
	// CurrentFloorTex = ( (CurrentFloorTex==NUM_TEXTURES-1) ? 0 : CurrentFloorTex+1 );
}
void Game::toggleTexMode(){
	switch (CurrentTexSample)
	{
	case GL_NEAREST:
		CurrentTexSample = GL_LINEAR;
		strcpy( CurrentTexString, "GL_LINEAR" );
		break;
	case GL_LINEAR:
		CurrentTexSample = GL_NEAREST_MIPMAP_NEAREST;
		strcpy( CurrentTexString, "GL_NEAREST_MIPMAP_NEAREST" );
		break;
	case GL_NEAREST_MIPMAP_NEAREST:
		CurrentTexSample = GL_NEAREST_MIPMAP_LINEAR;
		strcpy( CurrentTexString, "GL_NEAREST_MIPMAP_LINEAR" );
		break;
	case GL_NEAREST_MIPMAP_LINEAR:
		CurrentTexSample = GL_LINEAR_MIPMAP_NEAREST;
		strcpy( CurrentTexString, "GL_LINEAR_MIPMAP_NEAREST" );
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		CurrentTexSample = GL_LINEAR_MIPMAP_LINEAR;
		strcpy( CurrentTexString, "GL_LINEAR_MIPMAP_LINEAR" );
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		CurrentTexSample = GL_NEAREST;
		strcpy( CurrentTexString, "GL_NEAREST" );
		break;
	}
}

// Draw the table as a set of scaled blocks.
void Game::render(int programID, std::vector< std::vector<int> > gameMap, int xLoc, int zLoc)
{

	int gameUniformHandle = glGetUniformLocation(programID, "maze");
	if (gameUniformHandle == -1){
		std::cout << "Uniform: maze is not an active uniform label\n";
		exit(1);
	}
	// Bind wall texture if required.
	if(programID == 6){
		glBindTexture(GL_TEXTURE_2D, TexID[CurrentWallsTex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	}
	// Bind floor and walls VOA.
	glBindVertexArray(vaoGameHandle);
	// Declare variables.
	glm::mat4 game;
	std::vector<int> tempvec;
	int gameSize = gameMap.size();
	int i, k;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	// glDepthMask(false);

	// z+
	glBindTexture(GL_TEXTURE_2D, TexID[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	// game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 0.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	// x-
	glBindTexture(GL_TEXTURE_2D, TexID[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, -1.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	// z-
	glBindTexture(GL_TEXTURE_2D, TexID[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	game = glm::rotate(game, (float)PI, glm::vec3(0.0f, -1.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	// x+
	glBindTexture(GL_TEXTURE_2D, TexID[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	// y-  bottom
	glBindTexture(GL_TEXTURE_2D, TexID[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	game = glm::rotate(game, (float)PI/2, glm::vec3(1.0f, 0.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	// y+  top
	glBindTexture(GL_TEXTURE_2D, TexID[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	glUseProgram(programID);
	// Render one big piece.
	game = glm::mat4(1.0f);
	game = glm::translate( game, glm::vec3(500, 0, 500) );
	game = glm::rotate(game, (float)PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
	game = glm::scale( game, glm::vec3( 1000, 1000, 1000) );
	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);



	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_CLAMP);
	// glDepthMask(false);


	glBindTexture(GL_TEXTURE_2D, TexID[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);  

	// Render the walls.
	for(i = 0; i < gameSize; i++){
		tempvec = gameMap[i];
		for(k = 0; k < gameSize; k++){
			if(tempvec[k] == 1){
				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
				game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, -1.0f, 0.0f));
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
				game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
				game = glm::rotate(game, (float)PI, glm::vec3(0.0f, -1.0f, 0.0f));
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			}
		}
	}
	// Render exterior walls.
	for(i = 0; i < gameSize; i++){
		tempvec = gameMap[i];
		for(k = 0; k < gameSize; k++){
			if(i == 0 && tempvec[k] != 1){
				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1-1) );
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			}
			// if(i == gameSize-1 && tempvec[k] != 1){
			// 	game = glm::mat4(1.0f);
			// 	game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
			// 	game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
			// 	glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
			// 	glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			// }
			if(k == 0 && tempvec[k] != 1){
				game = glm::mat4(1.0f);
				game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
				game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, -1.0f, 0.0f));
				game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
				glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
				glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			}
			// if(k == gameSize-1 && tempvec[k] != 1){
			//     game = glm::mat4(1.0f);
			//     game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
			//     game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
			//     game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
			//     glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
			//     glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			// }
		}
	}
	if(programID == 3) {
		programID = 9;    
		glUseProgram(programID);    
		gameUniformHandle = glGetUniformLocation(programID, "maze");
		if (gameUniformHandle == -1){
			std::cout << "Uniform: maze is not an active uniform label\n";
			exit(1);
		}
	}
	// Bind floor texture if required.
	if(programID == 6){
		glBindTexture(GL_TEXTURE_2D, TexID[CurrentFloorTex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);
	}

	// // Lighting.
	// int storeID = programID;
	// programID = 12;    
	// glUseProgram(programID);    
	// // gameUniformHandle = glGetUniformLocation(programID, "game");
	// // glm::mat3 normMatrix;
	
	// // normMatrix = glm::mat3(game);
	// // glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(game) );  // Middle
	// // glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	
	// // game = glm::mat4(1.0f);
	// // game = glm::translate( game, glm::vec3(1.0f, 0.5f, 1.0f) );
	// // normMatrix = glm::mat3(game);
	// // game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
	// // glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	// glDrawElements(GL_TRIANGLES, 5000, GL_UNSIGNED_INT, 0);

	
		// game = glm::mat4(1.0f);
		// game = glm::translate( game, glm::vec3(1, 0.5, 1) );
		// game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 0.0f, 0.0f));
		// game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
		// glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
		// glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
	
	glBindTexture(GL_TEXTURE_2D, TexID[CurrentFloorTex]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);

	// // Keep going.
	// programID = storeID;    
	// glUseProgram(programID); 
	// int halfMap = (gameSize/2)-gameSize;   
	// // Render floor.
	// for(i = 0; i < gameSize; i++){
	// 	tempvec = gameMap[i];
	// 	for(k = 0; k < gameSize; k++){
	// 		if(tempvec[k] != 1){
	// 			game = glm::mat4(1.0f);
	// 			game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
	// 			game = glm::translate( game, glm::vec3(halfMap, 0, halfMap) );
	// 			game = glm::rotate(game, (float)PI/2, glm::vec3(1.0f, 0.0f, 0.0f));
	// 			game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
	// 			glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	// 			glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
	// 		}
	// 	}
	// }
	// Bind finish marker VOA.
	// glBindVertexArray(vaoGameEndMarkerHandle);
	// // Render finished marker.
	// for(i = 0; i < gameSize; i++){
	//     tempvec = gameMap[i];
	//     for(k = 0; k < gameSize; k++){
	//         if(tempvec[k] == 2){
	//             game = glm::mat4(1.0f);
	//             game = glm::translate( game, glm::vec3(k+1, 1.5, i+1) );
	//             game = glm::scale( game, glm::vec3( 15, 15, 15) );
	//             glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	//             glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
	//         }
	//     }
	// }

	// glDepthMask(false);



	// if(k == gameSize-1 && tempvec[k] != 1){
	//     game = glm::mat4(1.0f);
	//     game = glm::translate( game, glm::vec3(k+1, 0.5, i+1) );
	//     game = glm::rotate(game, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
	//     game = glm::scale( game, glm::vec3( 0.5, 0.5, 0.5) );
	//     glUniformMatrix4fv( gameUniformHandle, 1, false, glm::value_ptr(game) );
	//     glDrawElements(GL_TRIANGLES, SQUARE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
	// }

	// We are using texture unit 0 (the default)
	glUniform1i(gameUniformHandle, 0);              // What does this do???
	
	glBindVertexArray(0);

	glFlush();
	
}
