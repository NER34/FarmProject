#include "render.h"
#include "campfire.h"

std::vector<GLuint> shader_programs;
std::vector<ModelContainer*> models;
std::vector<GLuint> diffuse_textures;
std::vector<GLuint> specular_textures;
std::vector<std::pair<Transform, GLuint>> objects;

const char* fog_texture_path = "Resources/Textures/fog.png";
GLuint fog_texture;
bool fog_enabled = false;

/// <summary>
/// Model on which animated textures are drawing
/// </summary>
Geometry anim_texture_plane;
std::vector<GLuint> anim_textures;
std::vector<std::string> anim_texture_paths{
	"Resources/Textures/ufo_light_diffuse.png",
	"Resources/Textures/fire_animation_diffuse.png",
	"Resources/Textures/abc_characters.png"
};
/// <summary>
/// Defines the basic parameters required for rendering a fire animated texture
/// </summary>
struct FireInfo {
	int fire_index = 0;
	float fire_timer = 0;
	float fire_timeout = 0.05f;
	bool fire_enabled = true;
	GLuint campfire_id;
}fire_info;

/// <summary>
/// Model on which banner texture is drawing
/// </summary>
Geometry banner_texture_plane;
GLuint banner_texture;
std::string banner_texture_path = "Resources/Textures/cow_diffuse.png";

/// <summary>
/// Defines the basic parameters required for rendering a banner 
/// </summary>
struct BannerInfo {
	float timer = 0.0f;
	float timeout = 3.0f;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 target_scale = glm::vec3(3.0f, 1.0f, 1.0f);
	glm::vec3 target_position_offset = glm::vec3(-1.0f, -4.0f, 0.0f);
}banner_info;

/// <summary>
/// Defines the basic parameters required for rendering a skybox
/// </summary>
struct Skybox {
	GLuint VAO;
	GLuint VBO;
	ShaderContainer shader;
	GLuint texture;
	int numTriangles;
	float night_control_val;
}skybox;
const char* SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "Resources/Textures/Skybox/skybox";

/// <summary>
/// Defines the basic parameters required for rendering an animated object
/// </summary>
struct AnimatedObjectInfo {
	ModelContainer * model;
	std::string diffuse_path = "Resources/Textures/duck_diffuse.png";
	std::string specular_path = "Resources/Textures/no_specular.png";
	GLuint diffuse_texture, specular_texture;
	glm::vec3 way_center = glm::vec3(-4.48f, 0.5f, - 13.0f);
	glm::vec3 last_position = glm::vec3(-4.48f, 0.5f, -13.0f);
	glm::vec3 last_direction;
	float time = 0;
	float speed = 0.03f;
	bool enabled = true;
}anim_obj_info;

std::string message = "Hello there";

bool data_loaded = false;

void LoadData(const std::string& config_file_path) 
{
	// Loading shaders
	if (!LoadShaders()) {
		LoadFail("failed load shaders.");
		return;
	}

	// Loading data for skybox
	initSkyboxGeometry();

	// Loading fog texture
	LoadFogTexture();

	// Loading data for animated textures
	LoadAnimTextures();

	// Loading data for banner
	LoadBanner();

	// loading data from file
	std::cout << "reading data from file" << std::endl;
	std::vector<std::string> data;
	ReadDataFromFile(config_file_path, data);

	// loading diffuse textures
	std::cout << "loading diffuse textures" << std::endl;
	GLuint textures_size = ReadHeader(data);
	if (textures_size < 0) {
		LoadFail("failed load diffuse textures.");
		return;
	}
	std::vector<std::string> textures_data;
	if (!ReplaceData(textures_size, data, textures_data)) {
		LoadFail("failed load diffuse textures.");
		return;
	}
	if (!LoadTextures(textures_data, true)) {
		LoadFail("failed load diffuse textures.");
		return;
	}
	// loading specular textures
	std::cout << "loading specular textures" << std::endl;
	textures_size = ReadHeader(data);
	if (textures_size < 0) {
		LoadFail("failed load specular textures.");
		return;
	}
	textures_data.clear();
	if (!ReplaceData(textures_size, data, textures_data)) {
		LoadFail("failed load specular textures.");
		return;
	}
	if (!LoadTextures(textures_data, false)) {
		LoadFail("failed load specular textures.");
		return;
	}
	// Loading models
	std::cout << "loading models" << std::endl;
	GLuint models_size = ReadHeader(data);
	if (models_size < 0) {
		LoadFail("failed load models.");
		return;
	}
	std::vector<std::string> models_data;
	if (!ReplaceData(models_size, data, models_data)) {
		LoadFail("failed load models.");
		return;
	}
	if (!LoadModels(models_data)) {
		LoadFail("failed load models.");
		return;
	}
	// loading objects
	std::cout << "loading objects" << std::endl;
	GLuint objects_size = ReadHeader(data);
	if (objects_size < 0) {
		LoadFail("failed load objects.");
		return;
	}
	std::vector<std::string> objects_data;
	if (!ReplaceData(objects_size, data, objects_data)) {
		LoadFail("failed load objects.");
		return;
	}
	if (!LoadObjects(objects_data)) {
		LoadFail("failed load objects.");
		return;
	}

	LoadAnimatedObject();

	data_loaded = true;
}

bool LoadShaders()
{
	GLuint program;

	if (!LoadSingleShaderProgram("object_vs.glsl", "object_fs.glsl", program)) return false;
	shader_programs.push_back(program);
	if (!LoadSingleShaderProgram("skybox_vs.glsl", "skybox_fs.glsl", program)) return false;
	shader_programs.push_back(program);
	if (!LoadSingleShaderProgram("anim_texture_vs.glsl", "anim_texture_fs.glsl", program)) return false;
	shader_programs.push_back(program);
	if (!LoadSingleShaderProgram("banner_vs.glsl", "banner_fs.glsl", program)) return false;
	shader_programs.push_back(program);

	return true;
}

bool LoadSingleShaderProgram(const char* vs_path, const char* fs_path, GLuint& program)
{
	GLuint shaders[] = {
	pgr::createShaderFromFile(GL_VERTEX_SHADER, vs_path),
	pgr::createShaderFromFile(GL_FRAGMENT_SHADER, fs_path),
	0
	};
	if (shaders[0] == 0 || shaders[1] == 0) {
		std::cout << "failed to create object shader from file. VS: " << shaders[0] << "; FS: " << shaders[1] << std::endl;
		if (shaders[0] != 0) glDeleteShader(shaders[0]);
		if (shaders[1] != 0) glDeleteShader(shaders[1]);
		return false;
	}

	program = pgr::createProgram(shaders);
	if (program == 0) {
		std::cout << "failed to create object program from shaders" << std::endl;
		for (int i = 0; i < 2; i++) glDeleteShader(shaders[i]);
		return false;
	}

	return true;
}

void initSkyboxGeometry() {

	// 2D coordinates of 2 triangles covering the whole screen (NDC), draw using triangle strip
	static const float screenCoords[] = {
	  -1.0f, -1.0f,
	   1.0f, -1.0f,
	  -1.0f,  1.0f,
	   1.0f,  1.0f
	};

	skybox.shader.SetProgram(shader_programs[1]);

	glGenVertexArrays(1, &skybox.VAO);
	glBindVertexArray(skybox.VAO);

	// buffer for far plane rendering
	glGenBuffers(1, &skybox.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	//glUseProgram(farplaneShaderProgram);

	GLuint screenCoordLoc = skybox.shader.GetAttribValue("screenCoord");
	glEnableVertexAttribArray(screenCoordLoc);
	glVertexAttribPointer(screenCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();

	skybox.numTriangles = 2;

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &skybox.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);

	const char* suffixes[] = { "right", "left", "top", "bottom", "front", "back" };
	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_CUBE_TEXTURE_FILE_PREFIX) + "_" + suffixes[i] + ".png";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
		CHECK_GL_ERROR();
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	CHECK_GL_ERROR();
	// unbind the texture (just in case someone will mess up with texture calls later)
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}

void LoadFogTexture() {
	fog_texture = pgr::createTexture(fog_texture_path);
	CHECK_GL_ERROR();
}

void LoadAnimTextures()
{
	float texture_verts[] =
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	};

	GLuint texture_indexes[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	LoadGeometry(anim_texture_plane, texture_verts, texture_indexes, shader_programs[2]);

	for (GLuint i = 0; i < anim_texture_paths.size(); i++) {
		anim_textures.push_back(pgr::createTexture(anim_texture_paths[i]));
	}
}

void LoadBanner()
{
	float texture_verts[] =
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 4.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 4.0f,
	};

	GLuint texture_indexes[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	LoadGeometry(banner_texture_plane, texture_verts, texture_indexes, shader_programs[3]);

	banner_texture = pgr::createTexture(banner_texture_path);
	glBindTexture(GL_TEXTURE_2D ,banner_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadGeometry(Geometry& model_geometry, float* verts, GLuint* indexes, GLuint shader_program)
{
	model_geometry.shader.SetProgram(shader_program);

	glGenVertexArrays(1, &model_geometry.VAO);
	glGenBuffers(1, &model_geometry.VBO);
	glGenBuffers(1, &model_geometry.EBO);

	glBindVertexArray(model_geometry.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, model_geometry.VBO);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_geometry.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indexes, GL_STATIC_DRAW);

	GLuint attrib_location;
	attrib_location = model_geometry.shader.GetAttribValue("position");
	glVertexAttribPointer(attrib_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(attrib_location);

	attrib_location = model_geometry.shader.GetAttribValue("texCoords");
	glVertexAttribPointer(attrib_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(attrib_location);

	glBindVertexArray(0);

	CHECK_GL_ERROR();
}

bool LoadTextures(const std::vector<std::string>& textures_data, bool diffuse)
{
	for (int i = 0; i < textures_data.size(); i++) 
	{
		GLuint texture = pgr::createTexture(textures_data[i]);
		if (texture == 0) return false;
		if (diffuse) diffuse_textures.push_back(texture);
		else specular_textures.push_back(texture);
	}
	return true;
}

bool LoadModels(const std::vector<std::string>& models_data) 
{
	for (GLuint i = 0; i < models_data.size() / 3; i++) {
		ModelContainer * model = new ModelContainer();
		GLbyte stencil_id = 0;
		if (models_data[i * 3] == "Resources/Models/tractor.obj") stencil_id = 2;
		else if (models_data[i * 3] == "Resources/Models/trough.obj") stencil_id = 3;
		if (models_data[i * 3] == "Resources/Models/campfire.obj") {
			LoadCampfire(&model);
			fire_info.campfire_id = i;
		}
		else if (!model->CreateModel(models_data[i * 3].c_str(), shader_programs[0], stencil_id)) {
			std::cout << "failed to create model. model id: " << i * 3 << std::endl;
			delete model;
			return false;
		}
		GLuint diffuse_texture = diffuse_textures[std::stoi(models_data[i * 3 + 1])];
		GLuint specular_texture = specular_textures[std::stoi(models_data[i * 3 + 2])];
		model->SetMaterial(diffuse_texture, specular_texture, 32);
		model->SetFogTexture(fog_texture);
		models.push_back(model);
	}

	return true;
}

void LoadCampfire(ModelContainer** model) {

	(*model)->SetShaderProgram(shader_programs[0]);
	(*model)->SetStencilId((GLbyte)1);

	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, planeNVertices * planeNAttribsPerVertex * sizeof(float), planeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeNTriangles * 3 * sizeof(GLuint), planeTriangles, GL_STATIC_DRAW);

	GLuint attrib_location;

	attrib_location = (*model)->GetShader().GetAttribValue("position");
	glVertexAttribPointer(attrib_location, 3, GL_FLOAT, GL_FALSE, planeNAttribsPerVertex * sizeof(float), 0);
	glEnableVertexAttribArray(attrib_location);

	attrib_location = (*model)->GetShader().GetAttribValue("normal");
	glVertexAttribPointer(attrib_location, 3, GL_FLOAT, GL_FALSE, planeNAttribsPerVertex * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(attrib_location);

	attrib_location = (*model)->GetShader().GetAttribValue("texCoords");
	glVertexAttribPointer(attrib_location, 2, GL_FLOAT, GL_FALSE, planeNAttribsPerVertex * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(attrib_location);

	glBindVertexArray(0);

	CHECK_GL_ERROR();

	(*model)->SetVAO(VAO);
	(*model)->SetVBO(VBO);
	(*model)->SetEBO(EBO, planeNTriangles * 3);
}

bool LoadObjects(const std::vector<std::string>& objects_data) 
{
	for (GLuint i = 0; i < objects_data.size() / 4; i++) 
	{
		GLuint model_id;
		try
		{
			model_id = (GLuint)std::stoi(objects_data[i * 4]);
		}
		catch (const std::exception&)
		{
			return false;
		}
		if (model_id < 0 || model_id >= models.size())
		{
			std::cout << "model id out of range. object id: " << i * 4 << std::endl;
			return false;
		}
		Transform transform;
		if(!GetVector3f(objects_data[i * 4 + 1], transform.position)) return false;
		if(!GetVector4f(objects_data[i * 4 + 2], transform.rotation)) return false;
		if(!GetVector3f(objects_data[i * 4 + 3], transform.scale)) return false;

		objects.push_back(std::make_pair(transform, model_id));
	}

	return true;
}

void LoadAnimatedObject() 
{
	anim_obj_info.model = new ModelContainer();
	anim_obj_info.diffuse_texture = pgr::createTexture(anim_obj_info.diffuse_path);
	anim_obj_info.specular_texture = pgr::createTexture(anim_obj_info.specular_path);
	anim_obj_info.model->CreateModel("Resources/Models/duck.obj", shader_programs[0], 0, true, true);
	anim_obj_info.model->SetMaterial(anim_obj_info.diffuse_texture, anim_obj_info.specular_texture, 32);
	anim_obj_info.model->SetFogTexture(fog_texture);
}

void Draw(const DirectLight& direct_light, const PointLight& point_light, const SpotLight& spot_light, const Camera& camera, GLfloat win_width, GLfloat win_height, float dt)
{
	if (!data_loaded) return;

	glm::mat4 viewMatrix;
	viewMatrix = glm::lookAt(camera.position, camera.position + camera.direction, camera.camera_up);

	glm::mat4 projectionMatrix = glm::perspective(45.0f, win_width / win_height, 0.1f, 100.0f);

	
	drawSkybox(viewMatrix, projectionMatrix);

	for (GLuint i = 0; i < objects.size(); i++)
	{
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, objects[i].first.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i].first.rotation[3]), glm::vec3(objects[i].first.rotation));
		modelMatrix = glm::scale(modelMatrix, objects[i].first.scale);

		GLuint model_id = objects[i].second;
		models[model_id]->Draw(modelMatrix, viewMatrix, projectionMatrix, direct_light, point_light, spot_light, camera, dt, fog_enabled);
		//CHECK_GL_ERROR();
	}

	DrawAnimatedObject(direct_light, point_light, spot_light, camera, projectionMatrix, viewMatrix, dt);

	DrawBanner(projectionMatrix, viewMatrix, camera, dt, glm::vec3(16.6f, 8.3f, 34.85f), glm::vec3(2.0f, 8.0f, 1.0f));

	DrawAnimTexture(projectionMatrix, viewMatrix, camera, UFO, 1, 1, 0, glm::vec3(16.3f, 8.3f, 34.25f), glm::vec3(4.0f, 8.0f, 4.0f));

	if (fire_info.fire_enabled) {
		fire_info.fire_timer += dt;
		if (fire_info.fire_timer >= fire_info.fire_timeout) {
			fire_info.fire_index++;
			fire_info.fire_index %= 12;
			fire_info.fire_timer = 0.0f;
		}
		glm::vec3 campfire_pos;
		GetCampfireData(campfire_pos);
		campfire_pos.y += 1.0f;
		DrawAnimTexture(projectionMatrix, viewMatrix, camera, FIRE, 4, 4, fire_info.fire_index, campfire_pos, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	DrawMessage(projectionMatrix, viewMatrix, camera, message);
	return;
}

void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(skybox.shader.GetProgram());

	// compose transformations
	glm::mat4 matrix = projectionMatrix * viewMatrix;

	// create view rotation matrix by using view matrix with cleared translation
	glm::mat4 viewRotation = viewMatrix;
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// vertex shader will translate screen space coordinates (NDC) using inverse PV matrix
	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

	glUniformMatrix4fv(skybox.shader.GetUniformValue("inversePVmatrix"), 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skybox.shader.GetUniformValue("skyboxSampler"), 0);
	glUniform1i(skybox.shader.GetUniformValue("isFog"), fog_enabled);
	glUniform1f(skybox.shader.GetUniformValue("night_control_val"), skybox.night_control_val);
	

	// draw "skybox" rendering 2 triangles covering the plane
	glBindVertexArray(skybox.VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, skybox.numTriangles + 2);

	glBindVertexArray(0);
	glUseProgram(0);
}

void DrawAnimatedObject(const DirectLight& direct_light, const PointLight& point_light, const SpotLight& spot_light, const Camera& camera, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float dt)
{
	float changed_time = anim_obj_info.time + (anim_obj_info.enabled ? dt : anim_obj_info.speed);
	anim_obj_info.time = anim_obj_info.enabled ? changed_time : anim_obj_info.time;

	glm::vec3 new_anim_position = glm::vec3(glm::cos(changed_time) * 0.75f, 0.0f, glm::sin(changed_time) * glm::cos(changed_time) / 2) + anim_obj_info.way_center;
	glm::vec3 new_anim_direction = glm::normalize(new_anim_position - anim_obj_info.last_position);
	glm::mat4 modelMatrix = GetRotatedModelMatrix(-new_anim_direction, anim_obj_info.last_position, glm::vec3(0.125f));

	if (anim_obj_info.enabled)
	{
		anim_obj_info.last_position = new_anim_position;
		anim_obj_info.last_direction = new_anim_direction;
	}

	anim_obj_info.model->Draw(modelMatrix, viewMatrix, projectionMatrix, direct_light, point_light, spot_light, camera, dt, fog_enabled);
}

void DrawBanner(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, float dt, glm::vec3 position, glm::vec3 scale)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	banner_info.timer += dt;
	if (banner_info.timer >= banner_info.timeout) {
		banner_info.timer = 0.0f;
	}

	glm::mat4 modelMatrix = GetRotatedModelMatrix(glm::vec3(camera.direction.x, 0.0f, camera.direction.z), position, scale);

	glm::vec3 banner_position_offset = glm::mix(glm::vec3(0.0f), banner_info.target_position_offset, banner_info.timer / banner_info.timeout);
	glm::vec3 banner_current_scale = glm::mix(banner_info.scale, banner_info.target_scale, banner_info.timer / banner_info.timeout);

	glm::mat4 texModelMatrix;
	texModelMatrix = glm::translate(texModelMatrix, banner_position_offset);
	texModelMatrix = glm::scale(texModelMatrix, banner_current_scale);

	banner_texture_plane.shader.UseProgram();
	glUniformMatrix4fv(banner_texture_plane.shader.GetUniformValue("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(banner_texture_plane.shader.GetUniformValue("viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(banner_texture_plane.shader.GetUniformValue("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(banner_texture_plane.shader.GetUniformValue("texModelMatrix"), 1, GL_FALSE, glm::value_ptr(texModelMatrix));
	glUniform3fv(banner_texture_plane.shader.GetUniformValue("viewPos"), 1, glm::value_ptr(camera.position));
	glUniform1i(banner_texture_plane.shader.GetUniformValue("fog"), fog_enabled);
	glUniform1i(banner_texture_plane.shader.GetUniformValue("tex"), 0);
	glUniform1i(banner_texture_plane.shader.GetUniformValue("fog_tex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, banner_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fog_texture);

	glBindVertexArray(banner_texture_plane.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DrawAnimTexture(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, GLuint type, int size_x, int size_y, int index, glm::vec3 tex_coord, glm::vec3 tex_scale, bool enable_rotation)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 modelMatrix;
	if (enable_rotation)
	{
		modelMatrix = GetRotatedModelMatrix(glm::vec3(camera.direction.x, 0.0f, camera.direction.z), tex_coord, tex_scale);
	}
	else
	{
		modelMatrix = glm::translate(modelMatrix, tex_coord);
		modelMatrix = glm::scale(modelMatrix, tex_scale);
	}

	anim_texture_plane.shader.UseProgram();
	glUniformMatrix4fv(anim_texture_plane.shader.GetUniformValue("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(anim_texture_plane.shader.GetUniformValue("viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(anim_texture_plane.shader.GetUniformValue("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform3fv(anim_texture_plane.shader.GetUniformValue("viewPos"), 1, glm::value_ptr(camera.position));
	glUniform1i(anim_texture_plane.shader.GetUniformValue("fog"), fog_enabled);
	glUniform1i(anim_texture_plane.shader.GetUniformValue("size_x"), size_x);
	glUniform1i(anim_texture_plane.shader.GetUniformValue("size_y"), size_y);
	glUniform1i(anim_texture_plane.shader.GetUniformValue("index"), index);
	glUniform1f(anim_texture_plane.shader.GetUniformValue("offset_x"), 1.0f / size_x);
	glUniform1f(anim_texture_plane.shader.GetUniformValue("offset_y"), 1.0f / size_y);
	glUniform1i(anim_texture_plane.shader.GetUniformValue("tex"), 0);
	glUniform1i(anim_texture_plane.shader.GetUniformValue("fog_tex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, anim_textures[type]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fog_texture);

	glBindVertexArray(anim_texture_plane.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DrawMessage(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, std::string mes)
{
	glm::vec3 position = glm::vec3(-2.0f, 1.0f, -2.0f);
	glm::vec3 pos_offset = glm::vec3(0.5f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f);
	for (GLuint i = 0; i < mes.size(); i++) 
	{
		mes[i] = std::toupper(mes[i]);
		if (mes[i] == ' ') {
			position += pos_offset;
			continue;
		}
		if (mes[i] < 'A' || mes[i] > 'Z') continue;
		int index = mes[i] - 'A';

		DrawAnimTexture(projectionMatrix, viewMatrix, camera, CHARACTER, 13, 2, index, position, scale, false);
		position += pos_offset;
	}
}

void LoadFail(const std::string& message) 
{
	std::cout << "loading failed: " << message << std::endl;
	ClearData();
}

void GetAnimObjectData(glm::vec3& position, glm::vec3& direction)
{
	position = anim_obj_info.last_position;
	direction = anim_obj_info.last_direction;
}

void GetCampfireData(glm::vec3& position)
{
	position = objects[fire_info.campfire_id].first.position;
}

glm::mat4& GetRotatedModelMatrix(const glm::vec3& direction_from_target, const glm::vec3& position, const glm::vec3& scale) {
	glm::mat4 texViewMatrix;
	texViewMatrix = glm::lookAt(glm::vec3(0.0f), direction_from_target, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 billboardRotationMatrix = glm::mat4(
		texViewMatrix[0],
		texViewMatrix[1],
		texViewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
	modelMatrix = glm::scale(modelMatrix, scale);
	return modelMatrix * billboardRotationMatrix;
}

void SetNightValue(float val) {
	skybox.night_control_val = val;
}

void SwitchFog() {
	fog_enabled = !fog_enabled;
}

bool SwitchCampfire()
{
	fire_info.fire_enabled = !fire_info.fire_enabled;
	return fire_info.fire_enabled;
}

void SwitchAnimObject()
{
	anim_obj_info.enabled = !anim_obj_info.enabled;
}

void ClearData() 
{
	data_loaded = false;

	// Clear models
	for (GLuint i = 0; i < models.size(); i++) {
		delete models[i];
	}
	models.clear();
	
	// Clear shaders
	for (GLuint i = 0; i < shader_programs.size(); i++) {
		pgr::deleteProgramAndShaders(shader_programs[i]);
	}
	shader_programs.clear();

	// Clear diffuse and specular textures
	for (GLuint i = 0; i < diffuse_textures.size(); i++) {
		glDeleteTextures(1, &diffuse_textures[i]);
	}
	diffuse_textures.clear();
	for (GLuint i = 0; i < specular_textures.size(); i++) {
		glDeleteTextures(1, &specular_textures[i]);
	}
	specular_textures.clear();

	objects.clear();
	
	glDeleteTextures(1, &fog_texture);

	// Delete animated textures data
	glDeleteVertexArrays(1, &anim_texture_plane.VAO);
	glDeleteBuffers(1, &anim_texture_plane.VBO);
	glDeleteBuffers(1, &anim_texture_plane.EBO);

	for (GLuint i = 0; i < anim_textures.size(); i++) {
		glDeleteTextures(1, &anim_textures[i]);
	}
	anim_textures.clear();

	// Delete banner data
	glDeleteVertexArrays(1, &banner_texture_plane.VAO);
	glDeleteBuffers(1, &banner_texture_plane.VBO);
	glDeleteBuffers(1, &banner_texture_plane.EBO);
	glDeleteTextures(1, &banner_texture);

	// Delete skybox data
	glDeleteVertexArrays(1, &skybox.VAO);
	glDeleteBuffers(1, &skybox.VBO);
	glDeleteTextures(1, &skybox.texture);

	//Delete data of animated object
	delete anim_obj_info.model;
	glDeleteTextures(1, &anim_obj_info.diffuse_texture);
	glDeleteTextures(1, &anim_obj_info.specular_texture);

	return;
}