#version 450

// Runs on a per fragment basis

// No association in input locations and output locations
layout(location = 0) in vec3 fragColor;

// Fragment shaders do not have a built in output variable
// Need to declare ourselfves
layout(location = 0) out vec4 outColor;
// Layout is a qualifier with input argument location
// Capable of outputing to differe locations
// out is qualifier to make in an output variable
// vec4 type
// Name is outColor

// Shader expects to receive push constant data
// NOTE: Only one push constant can be used per shader block
layout(push_constant) uniform Push {
    vec2 offset; // must be same order as co
    vec3 color;
} push;

void main() {
    // Colour is 4 output, R G B Alpha channnels
    // Colour is only run on the per fragment basis, which is determined later
    outColor = vec4(push.color + fragColor, 1.0);
}