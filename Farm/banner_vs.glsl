#version 330 core

in vec3 position;
in vec2 texCoords;

out vec2 TexCoords;
out vec2 FogTexCoords;
out vec3 FragPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 texModelMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    vec3 ndcSpacePos;
    if (gl_Position.w != 0)
        ndcSpacePos = gl_Position.xyz / gl_Position.w;
    FogTexCoords = (ndcSpacePos.xy + 1.0f) / 2.0f;
    FragPos = vec3(modelMatrix * vec4(position, 1.0));

    TexCoords = vec2(texModelMatrix * vec4(texCoords, 1.0f, 1.0f));
};