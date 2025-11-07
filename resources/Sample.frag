#version 330

precision mediump float;

in vec3 fragWorldPos;
in vec3 fragNormal;

uniform vec3 uCameraPos;

out vec4 outColor;

void main()
{
    vec3 baseColor = vec3(0.0235, 0.2392, 0.0235);

    vec3 ambientLight = vec3(1.0, 0.9843, 0.0196);

    vec3 lightDir = vec3(0.0, 0.0, -0.707);
    vec3 diffuseColor = vec3(0.78, 0.88, 1.0);
    vec3 specularColor = vec3(0.8, 0.8, 0.8);
    float specularPower = 100.0;

    // Surface normal
    vec3 N = normalize(fragNormal);

    // Vector from surface to light
    vec3 L = normalize(-lightDir);

    // Vector from surface to camera
    vec3 V = normalize(uCameraPos - fragWorldPos);

    // Reflection of -L about N
    vec3 R = normalize(reflect(-L, N));

    // Compute phong reflection
    vec3 phong = ambientLight;
    float NDotL = dot(N, L);
    if (NDotL > 0.0)
    {
        vec3 diffuse = diffuseColor * NDotL;
        vec3 specular = specularColor * pow(max(0.0, dot(R, V)), specularPower);
        phong += diffuse + specular;
    }

    outColor = vec4(baseColor, 1.0) * vec4(phong, 1.0);
}
