#version 140

uniform samplerCube skyboxSampler;
in vec3 texCoord_v;
out vec4 color_f;
uniform bool isFog;
uniform float night_control_val;

vec4 applyFog(vec4 color) {
    float distanceFromCamera = abs(gl_FragCoord.z / gl_FragCoord.w);
    float fogDensity = 100.0f;
    vec4 fogColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);

    float fogBlendingFactor = exp(-pow(fogDensity * distanceFromCamera, 2));
    fogBlendingFactor = 1.0f - clamp(fogBlendingFactor, 0.0f, 1.0f);
    color = mix(color, fogColor, fogBlendingFactor);
    return mix(color, vec4(0.3, 0.3, 0.3, 1.0), night_control_val);
}

void main() {
    color_f = mix(texture(skyboxSampler, texCoord_v), vec4(0.1, 0.1, 0.1, 1.0), night_control_val);
    if (isFog) 
        color_f = applyFog(color_f);
}