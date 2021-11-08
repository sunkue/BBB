#include "stdafx.h"
#include "Renderer.h"
#include "TextRenderer.h"

////////////////////////////

SCREEN screen;

////////////////////////////

void render_chatrecord(glm::vec3 color = {}, void* font = GLUT_BITMAP_HELVETICA_18)
{
	const auto& record = ChatManager::get().get_record();
	TextOpt opt;
	opt._align = TEXT_ALIGNMENT::left;
	opt._space = { 0.0f, 0.0f };
	Pointf p = { -0.985f, -0.8f };
	auto rv_it_record = views::reverse(record).begin();
	for (int i = 0; i < min(10ull, record.size()); i++, rv_it_record++)
	{
		const auto& record = rv_it_record;
		glColor3f(color.r, color.g, color.b);
		auto chat_header = "player" + to_string(record->id) + ":: ";
		TextRenderer::rend_text(chat_header, p, opt, true, glm::vec4(0), font);

		glColor3f(color.r, color.g, color.b);
		auto header_w = TextRenderer::get_width(chat_header.data(), font) + 0.025f;
		p._x += header_w;
		TextRenderer::rend_text(record->str, p, opt, true, glm::vec4(0), font);
		p._x -= header_w;

		p._y += TextRenderer::get_height(font);
	}
}

////////////////////////////



void Renderer::init_shader()
{
	vector<string> VS;
	vector<string> FS;
	vector<string> GS;
	VS.clear(); VS.emplace_back("./Shader/test_vertex.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/IN_light.glsl"sv); FS.emplace_back("./Shader/test_fragment.glsl"sv);
	GS.clear(); GS.emplace_back("./Shader/test_geometry.glsl"sv);
	testing_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/screen_vertex.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/screen_fragment.glsl"sv);
	GS.clear();
	screen_shader_ = Shader::create(VS, FS, GS);

	{
		vector<string_view> textures
		{
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
		string_view dir{ "./Resource/Model/skybox" };
		VS.clear(); VS.emplace_back("./Shader/cubemap_vertex.glsl"sv);
		FS.clear(); FS.emplace_back("./Shader/cubemap_fragment.glsl"sv);
		GS.clear();
		auto cubeshader = Shader::create(VS, FS, GS);
		skybox = CubeMap::create(cubeshader, textures, dir);
	}

	ubo_vp_mat.bind(testing_shader_, "VP_MAT");
}

void Renderer::init_resources()
{
	load_model();
	testing_directional_light_ = DirectionalLight::create();
	testing_point_light_ = PointLight::create();
	testing_spot_light_ = SpotLight::create();
}

void Renderer::load_model()
{

	glGenVertexArrays(1, &quad_vao);
	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	struct QUAD
	{
		glm::vec2 pos;
		glm::vec2 tex;
	};
	QUAD quadv[] =
	{
		{{-1,-1},{0,0}},
		{{ 1, 1},{1,1}},
		{{-1, 1},{0,1}},
		{{-1,-1},{0,0}},
		{{ 1,-1},{1,0}},
		{{ 1, 1},{1,1}}
	};

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

	glBindVertexArray(0);


	auto model = Model::create("./Resource/Model/backpack/backpack.obj");
	auto greencar = Model::create("./Resource/Model/rocket/green/green_car.obj");
	auto bluecar = Model::create("./Resource/Model/rocket/blue/blue_car.obj");
	auto pinkcar = Model::create("./Resource/Model/rocket/pink/pink_car.obj");
	auto map = Model::create("./Resource/Model/default_map/default_map.obj");

	// create objs, give vaos for models

	cout << "model_load_done" << endl;
	default_map = make_shared<OBJ>(map);
	default_map->scaling(glm::vec3{ 50.f });

	cars_.emplace_back(make_shared<OBJ>(model));
	cars_[0]->move({ 10.f,5.f,2.f });

	cars_.emplace_back(make_shared<OBJ>(bluecar));
	cars_[1]->move({ 2.f,0.f,6.f });
	cars_[1]->scaling(glm::vec3{ 4.0f });

	cars_.emplace_back(make_shared<OBJ>(pinkcar));
	cars_[2]->move({ 5.f,0.f,10.f });
	cars_[2]->scaling(glm::vec3{ 4.0f });

	cars_.emplace_back(make_shared<OBJ>(greencar));
	cars_[3]->move({ 8.f,0.f,14.f });
	cars_[3]->scaling(glm::vec3{ 4.0f });

	player_ = make_shared<ControllObj>(size_t{ 0 }, bluecar);
	player_->scaling(glm::vec3{ 4.0f });

	main_camera_ = make_shared<Camera>();
	main_camera_->set_ownner(player_.get());
	main_camera_->set_diff({ -5.f, 3.f, 0.f });

	//	box_vao = terrain_shader_->create_vao(box, 36);
	//	terrain_ = make_shared<OBJ>(box_data, default_material);
		//glm::vec3 scale_ = { 100.f, 0.25f, 100.f };
		//terrain_->scaling(scale_);
		//glm::vec3 move_ = { 0.f,(scale_.y * -1.f) - 1.f,0.f };
		//terrain_->move(move_);

		//auto grass_vao = billboard_shader_->create_vao(cross_billboard_3, 36);
	//	ObjDataPtr grass_data = make_shared<OBJ_DATA>(grass_vao);
	const auto grass_count = 10;
	const auto grass_range = 50;
	grasses_.reserve(grass_count);
	for (int i = 0; i < grass_count; i++)
	{
		//grasses_.emplace_back(grass_data, default_material);
	}
	for (auto& g : grasses_)
	{
		glm::vec3 pos = { rand() % (2 * grass_range) - grass_range, 0.f, rand() % (2 * grass_range) - grass_range };
		g.scaling(glm::vec3(1.0f));
		g.move(pos);
		g.move({ -HALF_ROOT3,-1.f,-1.f });
		g.rotate(glm::vec3{ 0.f, rand() % 360, 0.f });
	}

}

void Renderer::load_texture()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	tbo = Texture::create();
	glGenTextures(1, &tbo->id);
	glBindTexture(GL_TEXTURE_2D, tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_.width, screen_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo->id, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_.width, screen_.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

}

/////////////////////////////////////////////////////////////////////

void Renderer::ready_draw()
{
	auto vp_mat = proj_mat() * main_camera_->view_mat();
	ubo_vp_mat.update(glm::value_ptr(vp_mat));
}

/////////////////////////////////////////////////////////////////////

void Renderer::draw()
{
	//timer::TIMER::instance().start();
	ready_draw();

	/* logic */
	Sleep(1);
	float milli_tick = static_cast<float>(GAME_SYSTEM::get().tick_time().count());
	float tick = milli_tick / 1000.f;
	player_->update(tick);
	main_camera_->update(tick);
	GAME_SYSTEM::get().tick();
	// 로직분리, 카메라 회전 , 스크롤로 거리조절 >>  1tick == 최소 1ms.

	auto gametime = static_cast<float>(GAME_SYSTEM::get().game_time())/1000.f;


	/*
	while (true) // render loop
	{
		// 1. geometry pass: render all geometric/color data to g-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gBufferShader.use();
		for (Object obj : Objects)
		{
			ConfigureShaderTransformsAndUniforms();
			obj.Draw();
		}
		// 2. lighting pass: use g-buffer to \
		//calculate the scene's lighting
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		lightingPassShader.use();
		BindAllGBufferTextures();
		SetLightingUniforms();
		RenderQuad();
	}

	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gColorSpec;
	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen.width, screen.height
		, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D
		, gPosition, 0);
	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen.width, screen.height
		, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
		gNormal, 0);
	// - color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen.width, screen.height
		, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
		gColorSpec, 0);
	// - tell OpenGL which color attachments we'll use
	// (of this framebuffer) for rendering
	unsigned int attachments[3]
		= { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// then also add render buffer object as depth buffer and
	// check for completeness. [...]
	*/
	glViewport(0, 0, screen_.width, screen_.height);
	glBindFramebuffer(GL_FRAMEBUFFER, tbo->id);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	testing_shader_->use();
	auto shaket = main_camera_->get_shaking_time();
	testing_shader_->set("time", abs(1.f - 5 * shaket));
	testing_shader_->set("explosion", main_camera_->get_shaking());
	auto view_pos = main_camera_->get_position();
	testing_shader_->set("u_view_pos", view_pos);
	testing_spot_light_->direction = main_camera_->get_look_dir();
	testing_spot_light_->position = main_camera_->get_position();
	testing_shader_->set("u_point_light", testing_point_light_);
	testing_shader_->set("u_directinal_light", testing_directional_light_);
	testing_shader_->set("u_spot_light", testing_spot_light_);

	glDisable(GL_CULL_FACE);
	default_map->update_uniform_vars(testing_shader_);
	default_map->draw(testing_shader_);
	glEnable(GL_CULL_FACE);

	for (auto& car : cars_)
	{
		car->update_uniform_vars(testing_shader_);
		car->draw(testing_shader_);
	}
	player_->update_uniform_vars(testing_shader_);
	player_->draw(testing_shader_);


	skybox->draw();

	// draw fbo
	glViewport(screen_.viewport_.x, screen_.viewport_.y, screen_.viewport_.z, screen_.viewport_.w);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.4f, 0.2f, 0.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	screen_shader_->use();
	screen_shader_->set("screen_texture", tbo);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	
	// text
	player_->render_chat({ 1,0,1 });
	render_chatrecord();

	auto fps = 1000 / (GAME_SYSTEM::get().tick_time().count() + 1);
	string title = "("s + to_string(fps) + " fps)"s;
	glutSetWindowTitle(title.c_str());
	
	return;






















	//////

	//terrain_shader_->use();
	//terrain_->bind_vao();
	//terrain_->update_uniform_vars(testing_shader_);
	//testing_shader_->set_texture("u_tex_sampler", terrain_tex_);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	billboard_shader_->use();
	glPolygonMode(GL_FRONT_AND_BACK, GLU_FILL);
	glDisable(GL_CULL_FACE);
	auto time = GAME_SYSTEM::get().game_time();
	//	grasses_[0].bind_vao();
	for (int i = 0; auto & grass : grasses_)
	{
		glm::mat4 shear = glm::mat4(1);
		auto t = float(time) / 500 + grass.get_position().x / 10;
		auto ww = glm::cos(t) / 8;
		shear[1][0] = ww;
		shear[1][2] = ww;
		grass.update_uniform_vars(billboard_shader_.get());
		//	billboard_shader_->set("u_shear_mat", shear);
	//		billboard_shader_->set_texture("u_tex_sampler", billboard_tex0_ + (i++ % 4));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}
	glPolygonMode(GL_FRONT, GLU_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);



	//timer::TIMER::instance().end("T::");
	// 16.6	-> 60fps
	// 33	-> 30fps
	//auto fps = 1000 / (GAME_SYSTEM::get().tick_time().count() + 1);
	//string title = "("s + to_string(fps) + " fps)"s;
	//glutSetWindowTitle(title.c_str());
}

