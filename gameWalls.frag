
#version 330

in vec4 pos; // Interpolated vertex position, in object coords

// The final colour we will see at this location on screen
out vec4 fragColour;

void main(void)
{
	vec4 sideClr = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 topClr = vec4(0.2, 0.7, 0.7, 1.0);

	// We will ignore vertex colour this time
	// Draw the top of the cube a different colour to the rest
	
	if (pos.y > 0.99) {
		fragColour = sideClr;
	}	
	else if (pos.y < -0.99) {
		fragColour = sideClr;
	}	
	else if (pos.x > 0.99) {
		fragColour = sideClr;
	}	
	else if (pos.x < -0.99) {
		fragColour = sideClr;
	}
	else {
		fragColour = sideClr;
	}

}
