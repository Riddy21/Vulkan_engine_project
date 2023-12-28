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
    gl_Position = vec4(position, 0.0, 1.0); // don't need to specify the index because specified in vertex buffer 

    // pass input color to output color
    fragColor = color;
}
