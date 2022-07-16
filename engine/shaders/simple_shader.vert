//VERTEX SHADER
#version 450

// hardcode vertices for triangle
// vec2 is a built-in type in the glsl lang that contains two floating point values
// 3 points for the corners of the triangle
vec2 positions[3] = vec2[] (
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

// Will be executed once for each vertex we have
// Input will get input vertex from input assembler stage
// Output wil be the output a position
void main() {
    // Instead of return, we assign a value to act as our output
    // gl_Positioin is a 4D vector that maps to our output fram buffer image
    // vec4 args:
    //      x, y: gl_Vertexindex contains the index of the current vertex for each time out main function is run
    //      z: 0 is front most layer stacks of layers 1 is the back
    //      normalization coef: normalizes vector, all the vectors are divided by this component to normalize
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
