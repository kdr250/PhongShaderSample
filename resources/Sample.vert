#version 330

precision mediump float;

layout (location = 0) in vec3 inPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 position = uProjection * uView * uModel * vec4(inPosition, 1.0);
    gl_Position = position;
}
