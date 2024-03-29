#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

layout (push_constant) uniform constants {
    mat4 model;
} ObjectData;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ObjectData.model * vec4(vertexPosition, 1.0);
    fragColor = vertexColor;
}
