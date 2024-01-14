#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec2 FogTexCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform bool transform_model;
uniform float change_val;

void main() {
    vec3 new_pos = position;
    if (transform_model){
        new_pos.y = new_pos.y * change_val;
    }
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(new_pos, 1.0f);
    vec3 ndcSpacePos;
    if (gl_Position.w != 0)
        ndcSpacePos = gl_Position.xyz / gl_Position.w;
    FogTexCoords = (ndcSpacePos.xy + 1.0f) / 2.0f;
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    TexCoords = texCoords; 
};