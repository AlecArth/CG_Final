/*********************************************************
Version 1.1

Code provided by Michael Hemsley and Anthony Dick
for the course 
COMP SCI 3014/7090 Computer Graphics
School of Computer Science
University of Adelaide

Permission is granted for anyone to copy, use, modify, or distribute this
program and accompanying programs and documents for any purpose, provided
this copyright notice is retained and prominently displayed, along with
a note saying that the original programs are available from the aforementioned 
course web page. 

The programs and documents are distributed without any warranty, express or
implied.  As the programs were written for research purposes only, they have
not been tested to the degree that would be advisable in any important
application.  All use of these programs is entirely at the user's own risk.
*********************************************************/

#version 330

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 maze;

in vec3 a_vertex;
in vec2 a_tex_coord;

out vec2 st;

void main(void)
{
    // Pass texture coords on to the fragment shader
    st = a_tex_coord;

	gl_Position = projection_matrix * modelview_matrix * maze * vec4(a_vertex, 1.0);
}
