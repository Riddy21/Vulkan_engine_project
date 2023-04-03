#version 450

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
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}