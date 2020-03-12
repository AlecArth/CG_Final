
#version 330

// Position and colour variables. 1 per vertex.
layout (location = 0) in vec3 a_vertex; 
//layout (location = 1) in vec4 a_colour;	

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 game;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 pos;

void main(void)
{
	pos = vec4(a_vertex, 1.0);	// We simply pass the colour along to the next stage

    // a_vertex is vertex position in object coordinates
    // gl_position is vertex position in camera coordinates
	gl_Position = projection_matrix * modelview_matrix * game * vec4(a_vertex, 1.0);
}
