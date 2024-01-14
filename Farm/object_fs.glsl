#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec2 FogTexCoords;

out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
    vec3 direction;
};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;
    vec3 position;
    float linear;
    float quadratic;
};

struct SpotLight {
    PointLight point;

    vec3 direction;
    float cut_off;
};

uniform Material material;
uniform DirectLight direct_light;
uniform PointLight point_light;
uniform SpotLight spot_light;
uniform vec3 viewPos;
uniform sampler2D fog_texture;
uniform bool fog;

vec3 CalculateDiffuse(vec3 material_diffuse, vec3 light_diffuse, vec3 light_direction, vec3 normal){
    float diffuse_value = max(dot(normal, light_direction), 0.0);
    return diffuse_value * material_diffuse * light_diffuse;
}

vec3 CalculateAmbient(vec3 material_ambient, vec3 light_ambient){
    return material_ambient * light_ambient;
}

vec3 CalculateSpecular(vec3 material_specular, vec3 light_specular, float material_shininess, vec3 view_position, vec3 light_direction, vec3 normal){
    vec3 view_direction = normalize(view_position - FragPos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_value = pow(max(dot(view_direction, reflect_direction), 0.0), material_shininess);
    return material_specular * specular_value * light_specular;
}

vec3 CalculateDirectLight(vec3 material_diffuse, vec3 material_specular, vec3 normal){
    vec3 light_direction = normalize(-direct_light.direction);
    
    vec3 ambient = CalculateAmbient(material_diffuse, direct_light.ambient);

    vec3 diffuse = CalculateDiffuse(material_diffuse, direct_light.diffuse, light_direction, normal);

    vec3 specular = CalculateSpecular(material_specular, direct_light.specular, material.shininess, viewPos, light_direction, normal);

    return direct_light.intensity * (ambient + diffuse + specular);
}

vec3 CalculatePointLight(vec3 material_diffuse, vec3 material_specular, vec3 normal, PointLight point){
    float distance = length(point.position - FragPos);
    float attenuation = 1.0f / (1.0f + point.linear * distance + point.quadratic * (distance * distance));

    vec3 light_direction = normalize(point.position - FragPos);

    vec3 ambient = CalculateAmbient(material_diffuse, point.ambient);

    vec3 diffuse = CalculateDiffuse(material_diffuse, point.diffuse, light_direction, normal);

    vec3 specular = CalculateSpecular(material_specular, point.specular, material.shininess, viewPos, light_direction, normal);

    return point.intensity * attenuation * (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(vec3 material_diffuse, vec3 material_specular, vec3 normal){
    vec3 light_direction = normalize(spot_light.point.position - FragPos);
    float theta = dot(light_direction, normalize(-spot_light.direction));
    vec3 ambient = CalculateAmbient(material_diffuse, spot_light.point.ambient);
    if (theta > spot_light.cut_off){
        return max(CalculatePointLight(material_diffuse, material_specular, normal, spot_light.point), ambient);
    }
    else{
        return ambient;
    }
}

void main() {

    vec3 material_diffuse = vec3(texture(material.diffuse, TexCoords));
    vec3 material_specular = vec3(texture(material.specular, TexCoords));
    vec3 normal = normalize(Normal);

    vec3 direct_color = CalculateDirectLight(material_diffuse, material_specular, normal);

    vec3 point_color = CalculatePointLight(material_diffuse, material_specular, normal, point_light);

    vec3 spot_color = CalculateSpotLight(material_diffuse, material_specular, normal);
    
    vec4 output_color = vec4(direct_color + point_color + spot_color, 1.0f);



    if (fog) 
        output_color = mix(output_color, texture(fog_texture, FogTexCoords), min(length(FragPos - viewPos), 10) / 10);
    color = output_color;
}