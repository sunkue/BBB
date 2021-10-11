#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer()
{
	glClearDepth(1.f);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GLU_FILL);
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init();
}


Renderer::~Renderer()
{
}

void Renderer::init()
{
	init_shader();
	init_resources();
}

void Renderer::init_shader()
{
	_default_shader = compile_shader("./Shader/vertex.glsl"sv, "./Shader/fragment.glsl"sv);
	_uloc_mvp_mat_ds = glGetUniformLocation(_default_shader, "u_mvp_mat");

	_terrain_shader = compile_shader("./Shader/vertex.glsl"sv, "./Shader/terrain_fragment.glsl"sv);
	_uloc_mvp_mat_ts = glGetUniformLocation(_terrain_shader, "u_mvp_mat");
	_uloc_texture_ts = glGetUniformLocation(_terrain_shader, "u_tex_sampler");

	_billboard_shader = compile_shader("./Shader/grass_vertex.glsl"sv, "./Shader/png_fragment.glsl"sv);
	_uloc_mvp_mat_bs = glGetUniformLocation(_billboard_shader, "u_mvp_mat");
	_uloc_texture_bs = glGetUniformLocation(_billboard_shader, "u_tex_sampler");
	_uloc_time_bs = glGetUniformLocation(_billboard_shader, "u_time");
}

void Renderer::init_resources()
{
	load_model();
	load_texture();
}

void Renderer::load_model()
{
	// create objs, give vaos for models
	auto box_vao = create_vao(_default_shader, box, 36);
	ObjDataPtr box_data = make_shared<OBJ_DATA>(box_vao);

	_cars.emplace_back(make_shared<OBJ>(box_data, _default_shader));
	_cars[0]->move({ 10.f,0.f,2.f });
	_cars.emplace_back(make_shared<OBJ>(box_data, _default_shader));
	_cars[1]->move({ 2.f,0.f,14.f });

	_player = make_shared<ControllObj>(box_data, _default_shader);

	_main_camera = make_shared<Camera>();
	_main_camera->set_ownner(_player->get_obj());
	_main_camera->set_diff({ -5.f, 3.f, 0.f });

	box_vao = create_vao(_terrain_shader, box, 36);
	_terrain = make_shared<OBJ>(box_data, _terrain_shader);
	glm::vec3 scale_ = { 100.f, 0.25f, 100.f };
	_terrain->scaling(scale_);
	glm::vec3 move_ = { 0.f,(scale_.y * -1.f) - 1.f,0.f };
	_terrain->move(move_);

	auto grass_vao = create_vao(_billboard_shader, cross_billboard_3, 18);
	ObjDataPtr grass_data = make_shared<OBJ_DATA>(grass_vao);
	const auto grass_count = 8000;
	const auto grass_range = 50;
	_grasses.reserve(grass_count);
	for (int i = 0; i < grass_count; i++)
	{
		_grasses.emplace_back(make_shared<OBJ>(grass_data, _billboard_shader));
	}
	for (auto& g : _grasses)
	{
		glm::vec3 pos = { rand() % (2 * grass_range) - grass_range, 0.f, rand() % (2 * grass_range) - grass_range };
		g->scaling(glm::vec3(1.0f));
		g->move(pos);
		g->rotate(glm::vec3{ 0.f,rand() % 360,0.f });
	}
}

void Renderer::load_texture()
{
	std::string Dir{ "./Resource/Texture" };
	_terrain_tex = CreatePngTexture((Dir + "./greenwool.png").c_str());

	_billboard_tex0 = CreatePngTexture((Dir + "./grass0.png").c_str());
	_billboard_tex1 = CreatePngTexture((Dir + "./grass1.png").c_str());
	_billboard_tex2 = CreatePngTexture((Dir + "./redflower.png").c_str());
	_billboard_tex3 = CreatePngTexture((Dir + "./blueflower.png").c_str());
}


GLuint Renderer::create_vao(GLuint shader, const VERTEX* vertices, GLsizei vertices_num)
{
	GLuint retvao;
	GLuint abo;
	GLuint ebo;

	glGenVertexArrays(1, &retvao);
	glBindVertexArray(retvao);

	glGenBuffers(1, &abo);
	glBindBuffer(GL_ARRAY_BUFFER, abo);
	glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(VERTEX), vertices, GL_STATIC_DRAW);


	GLint positionAttribute = glGetAttribLocation(shader, "a_position");
	if (positionAttribute == -1) {
		std::cerr << "position 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, pos));

	GLint normalAttribute = glGetAttribLocation(shader, "a_normal");
	if (normalAttribute == -1) {
		std::cerr << "normal 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(normalAttribute);
	glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, nor));

	GLint texcoordAttribute = glGetAttribLocation(shader, "a_texcoord");
	if (texcoordAttribute == -1) {
		std::cerr << "texcoord 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(texcoordAttribute);
	glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, tex));


	glBindVertexArray(0);
	glDeleteBuffers(1, &abo);
	glDeleteBuffers(1, &ebo);

	return retvao;
}

void Renderer::add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);

	glShaderSource(ShaderObj, 1, p, Lengths);


	glCompileShader(ShaderObj);

	GLint success;

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::read_file(string_view filename, std::string& target)
{
	std::ifstream file(filename.data());
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target.append(line.c_str());
		target.append("\n");
	}
	return true;
}

GLuint Renderer::compile_shader(string_view filenameVS, string_view filenameFS)
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	if (!read_file(filenameVS, vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	if (!read_file(filenameFS, fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	add_shader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	add_shader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

/////////////////////////////////////////////////////////////////////


void Renderer::reshape(int w, int h)
{
	if (w < h)
	{
		glViewport(0, (h - w) / 2, w, w);
	}
	else 
	{
		glViewport((w - h) / 2, 0, h, h);
	}
	draw();
}


/////////////////////////////////////////////////////////////////////

void Renderer::ready_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.2f, 0.2f, 1.0f);

	/*
		uniforms
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(screen.proj_M()));
	*/
}

void Renderer::draw()
{
	//timer::TIMER::instance().start();
	ready_draw();

	auto update_texture =
		[&](GLuint shader, GLuint uloc, GLuint texture_id)
	{
		glUniform1i(uloc, texture_id);
		glActiveTexture(GL_TEXTURE0 + texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	};

	Sleep(1);
	float tick = static_cast<float>(GAME_SYSTEM::instance().tick_time().count());
	_player->update(tick / 1000.f);
	_main_camera->update();
	GAME_SYSTEM::instance().tick();
	// 로직분리, 카메라 회전 , 스크롤로 거리조절 >>  1tick == 최소 1ms.

	/* draw */
	
	glUseProgram(_default_shader);
	_cars[0]->bind_vao();
	for (auto& car : _cars)
	{
		car->update_uniform_vars();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	_player->get_obj()->update_uniform_vars();
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glUseProgram(_terrain_shader);
	_terrain->bind_vao();
	_terrain->update_uniform_vars();
	update_texture(_terrain_shader, _uloc_texture_ts, _terrain_tex);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glUseProgram(_billboard_shader);
	glPolygonMode(GL_FRONT_AND_BACK, GLU_FILL);
	glDisable(GL_CULL_FACE);

	auto time = GAME_SYSTEM::instance().game_time();
	_grasses[0]->bind_vao();
	for (int i = 0; auto& grass : _grasses)
	{
		grass->update_uniform_vars();
		update_texture(_billboard_shader, _uloc_texture_bs, _billboard_tex0 + (i++ % 4));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glUniform1ui(_uloc_time_bs, time);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}

	glPolygonMode(GL_FRONT, GLU_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//timer::TIMER::instance().end("T::");
	// 16.6	-> 60fps
	// 33	-> 30fps
	auto fps = 1000 / (GAME_SYSTEM::instance().tick_time().count() + 1);
	string title = "("s + to_string(fps) + " fps)"s;
	glutSetWindowTitle(title.c_str());
	
}