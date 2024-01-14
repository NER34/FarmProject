#include <iostream>

#include "ModelContainer.h"

ModelContainer::~ModelContainer()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VBO != 0) glDeleteBuffers(1, &EBO);
}

bool ModelContainer::CreateModel(const char* path, const GLuint& shader_program, const GLbyte& _stencil_id, const bool& _transform_model, const bool& _glass_mode)
{
    std::cout << "loading model: " << path << std::endl;

    stencil_id = _stencil_id;
    transform_model = _transform_model;
    glass_mode = _glass_mode;
    time = 0;

	Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0
        | aiProcess_Triangulate             // Triangulate polygons (if any).
        | aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
        | aiProcess_GenSmoothNormals        // Calculate normals per vertex.
        | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes != 1) {
        std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
        return false;
    }

    const aiMesh* mesh = scene->mMeshes[0];

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertex.tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    EBO_size = indices.size();
    shader.SetProgram(shader_program);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    GLuint attrib_location;

    attrib_location = shader.GetAttribValue("position");
    glVertexAttribPointer(attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(attrib_location);

    attrib_location = shader.GetAttribValue("normal");
    glVertexAttribPointer(attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(attrib_location);

    attrib_location = shader.GetAttribValue("texCoords");
    glVertexAttribPointer(attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
    glEnableVertexAttribArray(attrib_location);

    glBindVertexArray(0);

	return true;
}

bool ModelContainer::SetMaterial(const char* diffuse_texture_path, const char* specular_texture_path, float shininess) {
    material.diffuse_texture = pgr::createTexture(diffuse_texture_path);
    if (material.diffuse_texture == 0)
    {
        std::cout << "load failed: " << diffuse_texture_path << std::endl;
        return false;
    }
    material.specular_texture = pgr::createTexture(specular_texture_path);
    if (material.specular_texture == 0)
    {
        std::cout << "load failed: " << specular_texture_path << std::endl;
        return false;
    }
    material.shininess = shininess;
    return true;
}

void ModelContainer::SetMaterial(const GLuint& diffuse_texture, const GLuint& specular_texture, float shininess)
{
    material.diffuse_texture = diffuse_texture;
    material.specular_texture = specular_texture;
    material.shininess = shininess;
}

void ModelContainer::Draw(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, DirectLight direct, PointLight point, SpotLight spot, Camera camera, float dt, bool fog_enabled) {

    shader.UseProgram();
    glUniformMatrix4fv(shader.GetUniformValue("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader.GetUniformValue("viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader.GetUniformValue("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniform1i(shader.GetUniformValue("material.diffuse"), 0);
    glUniform1i(shader.GetUniformValue("material.specular"), 1);
    glUniform1f(shader.GetUniformValue("material.shininess"), 32);

    glUniform1i(shader.GetUniformValue("fog_texture"), 2);
    glUniform1i(shader.GetUniformValue("fog"), fog_enabled);

    glUniform3fv(shader.GetUniformValue("point_light.ambient"), 1, glm::value_ptr(point.ambient));
    glUniform3fv(shader.GetUniformValue("point_light.diffuse"), 1, glm::value_ptr(point.diffuse));
    glUniform3fv(shader.GetUniformValue("point_light.specular"), 1, glm::value_ptr(point.specular));
    glUniform1f(shader.GetUniformValue("point_light.linear"), point.linear);
    glUniform1f(shader.GetUniformValue("point_light.quadratic"), point.quadratic);
    glUniform3fv(shader.GetUniformValue("point_light.position"), 1, glm::value_ptr(point.position));
    glUniform1f(shader.GetUniformValue("point_light.intensity"), point.intensity);

    glUniform3fv(shader.GetUniformValue("direct_light.ambient"), 1, glm::value_ptr(direct.ambient));
    glUniform3fv(shader.GetUniformValue("direct_light.diffuse"), 1, glm::value_ptr(direct.diffuse));
    glUniform3fv(shader.GetUniformValue("direct_light.specular"), 1, glm::value_ptr(direct.specular));
    glUniform3fv(shader.GetUniformValue("direct_light.direction"), 1, glm::value_ptr(direct.direction));
    glUniform1f(shader.GetUniformValue("direct_light.intensity"), direct.intensity);

    glUniform3fv(shader.GetUniformValue("spot_light.point.ambient"), 1, glm::value_ptr(spot.point.ambient));
    glUniform3fv(shader.GetUniformValue("spot_light.point.diffuse"), 1, glm::value_ptr(spot.point.diffuse));
    glUniform3fv(shader.GetUniformValue("spot_light.point.specular"), 1, glm::value_ptr(spot.point.specular));
    glUniform1f(shader.GetUniformValue("spot_light.point.linear"), spot.point.linear);
    glUniform1f(shader.GetUniformValue("spot_light.point.quadratic"), spot.point.quadratic);
    glUniform3fv(shader.GetUniformValue("spot_light.point.position"), 1, glm::value_ptr(spot.point.position));
    glUniform3fv(shader.GetUniformValue("spot_light.direction"), 1, glm::value_ptr(spot.direction));
    glUniform1f(shader.GetUniformValue("spot_light.cut_off"), glm::cos(glm::radians(spot.cut_off)));
    glUniform1f(shader.GetUniformValue("spot_light.point.intensity"), spot.point.intensity);

    glUniform3fv(shader.GetUniformValue("viewPos"), 1, glm::value_ptr(camera.position));

    glUniform1i(shader.GetUniformValue("transform_model"), transform_model);
    if (transform_model) {
        time += dt;
        float change_value = cos(time) / 2 + 1.0f;
        glUniform1f(shader.GetUniformValue("change_val"), change_value);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuse_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.specular_texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fog_texture);

    if (stencil_id != 0) {
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }

    if (glass_mode) glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
    else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    if (stencil_id != 0) glStencilFunc(GL_ALWAYS, stencil_id, -1);
    glDrawElements(GL_TRIANGLES, EBO_size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    if (stencil_id != 0) glDisable(GL_STENCIL_TEST);
}

void ModelContainer::SetStencilId(const GLbyte& _stencil_id) {
    stencil_id = _stencil_id;
}

void ModelContainer::SetShaderProgram(GLuint shader_program) {
    shader.SetProgram(shader_program);
}


ShaderContainer& ModelContainer::GetShader() {
    return shader;
}

void ModelContainer::SetVAO(GLuint vao) {
    VAO = vao;
}

void ModelContainer::SetVBO(GLuint vbo) {
    VBO = vbo;
}

void ModelContainer::SetEBO(GLuint ebo, unsigned int ebo_size) {
    EBO = ebo;
    EBO_size = ebo_size;
}

void ModelContainer::SetFogTexture(GLuint texture) {
    fog_texture = texture;
}