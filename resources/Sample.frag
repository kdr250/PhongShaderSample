#version 330

precision mediump float;

in vec3 fragWorldPos;
in vec3 fragNormal;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;

out vec4 outColor;

void main()
{
    vec3 objectColor = vec3(0.0235, 0.2392, 0.0235);

    vec3 lightColor = vec3(1.0, 0.9843, 0.0196);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(uLightPos - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uCameraPos - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    outColor = vec4(result, 1.0);
}
