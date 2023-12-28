#version 450

// Runs on a per fragment basis

// Pass values from fragment in 
layout (location = 0) in vec3 fragColor; // location and datatype matches vertex shader

// Fragment shaders do not have a built in output variable
// Need to declare ourselfves
layout (location = 0) out vec4 outColor;

// Layout is a qualifier with input argument location
// Capable of outputing to differe locations
// out is qualifier to make in an output variable
// vec4 type
// Name is outColor

void main() {
    // Colour is 4 output, R G B Alpha channnels
    // Colour is only run on the per fragment basis, which is determined later
    outColor = vec4(fragColor, 1.0);
}