#version 330 core

out vec4 color;

in vec2 TexCoords;
in vec2 FogTexCoords;
in vec3 FragPos;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform sampler2D fog_tex;
uniform bool fog;

void main() {
    vec4 output_color = texture(tex, TexCoords);
    if (output_color.a == 0.0f) discard;
    if (fog) 
        output_color = vec4(mix(vec3(output_color), vec3(texture(fog_tex, FogTexCoords)), min(length(FragPos - viewPos), 10) / 10), output_color.a);
    color = output_color;
};