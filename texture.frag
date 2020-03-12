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

in vec2 st;

uniform sampler2D texMap;

out vec4 fragColour;

void main(void)
{
    // Try changing this colour...
    vec4 colour = vec4(1.0, 1.0, 1.0, 1.0);
    
    // Modulate (multiply) a preset colour with the texture colour
    // at this location.
	fragColour = colour * texture(texMap, st);	
}
