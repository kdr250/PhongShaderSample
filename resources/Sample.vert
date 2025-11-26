#version 330

precision mediump float;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 fragWorldPos;
out vec3 fragNormal;

void main()
{
    vec4 worldPos = uModel * vec4(inPosition, 1.0);
    vec4 position = uProjection * uView * worldPos;
    gl_Position = position;
    
    fragWorldPos = worldPos.xyz;
    fragNormal = mat3(transpose(inverse(uModel))) * inNormal;
}
