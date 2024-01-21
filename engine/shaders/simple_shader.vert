//VERTEX SHADER
#version 450

// Specifies a vertex attribute
//     - Datatype is vec2
//     - The in key word specifies that this takes the value from the vertex buffer
//     - location specifies the storage location of where the variable value will come from
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color; // loation is the state machine location of the input

// No association in input locations and output locations
layout(location = 0) out vec3 fragColor;

// Shader expects to receive push constant data
// NOTE: Only one push constant can be used per shader block
layout(push_constant) uniform Push {
    float rotate;
    vec2 scale;
    vec2 offset; // must be same order as code
    vec3 color;
} push;

// Will be executed once for each vertex we have
// Input will get input vertex from input assembler stage
// Output wil be the output a position
void main() {
    // Instead of return, we assign a value to act as our output
    // gl_Positioin is a 4D vector that maps to our output frame buffer image
    // vec4 args:
    //      x, y: gl_Vertexindex contains the index of the current vertex for each time out main function is run
    //      z: 0 is front most layer stacks of layers 1 is the back
    //      normalization coef: normalizes vector, all the vectors are divided by this component to normalize
    mat2 scale = mat2(push.scale.x, 0.0, 0.0, push.scale.y); // mat2 is a 2x2 matrix
    //// Calculate the rotation matrix
    float c = cos(push.rotate);
    float s = sin(push.rotate);
    mat2 rotate = mat2(c, s, -s, c);

    // NOTE: matrix mult applies right to left
    // position -> scale -> rotate
    gl_Position = vec4(rotate * scale * position + push.offset, 0.0, 1.0);
    fragColor = color;
}
