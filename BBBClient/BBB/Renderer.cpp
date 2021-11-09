#include "stdafx.h"
#include "Renderer.h"
#include "TextRenderer.h"

////////////////////////////

SCREEN screen;



void Renderer::init()
{
	load_texture();
	init_shader();
	init_resources();
}

void Renderer::init_shader()
{
	vector<string> VS;
	vector<string> FS;
	vector<string> GS;
	VS.clear(); VS.emplace_back("./Shader/test_vertex.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/IN_light.glsl"sv); FS.emplace_back("./Shader/test_fragment.glsl"sv);
	GS.clear(); GS.emplace_back("./Shader/test_geometry.glsl"sv);
	testing_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/grass_vertex.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/png_fragment.glsl"sv);
	GS.clear();
	billboard_shader_ = Shader::create(VS, FS, GS);

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

	screen_renderer = make_unique<ScreenRenderer>();
	screen_renderer->init();
}

void Renderer::init_resources()
{
	load_model();

	testing_directional_light_ = DirectionalLight::create();
	testing_point_light_ = PointLight::create();
	testing_spot_light_ = SpotLight::create();

	ubo_vp_mat.bind(testing_shader_, "VP_MAT");
	ubo_vp_mat.bind(billboard_shader_, "VP_MAT");

}

void Renderer::load_model()
{
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

	vector<Vertex> grassvertices;
	for (auto& v : cross_billboard_3)
	{
		grassvertices.push_back(v);
	}

	const auto grass_count = 10;
	const auto grass_range = 80;

	grasses_.set_num_inst(grass_count);
	grasses_.setup_mesh(grassvertices);

	vector<float> scales; scales.reserve(grass_count);
	vector<float> yaw; yaw.reserve(grass_count);
	vector<float> shearseed; shearseed.reserve(grass_count);
	vector<glm::vec3> translate; translate.reserve(grass_count);

	for (int i = 0; i < grass_count; i++)
	{
		glm::vec3 pos = { rand() % (2 * grass_range) - grass_range, 0.f, rand() % (2 * grass_range) - grass_range };
		const glm::vec3 default_pos = { -HALF_ROOT3, -1.f, -1.f };
		translate.push_back(pos + default_pos);
		shearseed.push_back(translate[i].x / 10);
		scales.push_back(1.f + i % 3 * 0.3f);
		yaw.push_back(glm::radians(static_cast<float>(rand() % 360)));
	}

	auto texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/grass0.png");grasses_.add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/grass1.png");		grasses_.add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/blueflower.png");	grasses_.add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/redflower.png");	grasses_.add_texture(texture);

	grasses_.setup_instance_attribute(billboard_shader_, "a_scale", scales.data());
	grasses_.setup_instance_attribute(billboard_shader_, "a_yaw", yaw.data());
	grasses_.setup_instance_attribute(billboard_shader_, "a_translate", translate.data());
	grasses_.setup_instance_attribute(billboard_shader_, "a_shearseed", shearseed.data());
}


void Renderer::load_texture()
{

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
	ready_draw();

	auto gametime = static_cast<float>(GAME_SYSTEM::get().game_time()) / 1000.f;

	screen_renderer->bind_predraw_fbo();
	
	{
		testing_shader_->use();
		auto shaket = main_camera_->get_shaking_time();
		testing_shader_->set("time", abs(1.f - shaket * 5.f));
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

		// billoards
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);

		billboard_shader_->use();
		billboard_shader_->set("u_time", gametime);
		auto tt = grasses_.get_textures();
		billboard_shader_->set("u_tex_sampler", tt);
		grasses_.draw();

	}

	screen_renderer->blit_fbo();

	screen_renderer->draw_screen();

	glUseProgram(0);
	
	

	return;
}

void ScreenRenderer::init()
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

	/// /

	glGenFramebuffers(1, &predraw_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, predraw_fbo);

	predraw_tbo = Texture::create();
	glGenTextures(1, &predraw_tbo->id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, predraw_tbo->id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB,
		screen.width, screen.height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0
		, GL_TEXTURE_2D_MULTISAMPLE, predraw_tbo->id, 0);

	glGenRenderbuffers(1, &predraw_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, predraw_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screen.width, screen.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, predraw_rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glGenFramebuffers(1, &screen_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

	screen_tbo = Texture::create();
	glGenTextures(1, &screen_tbo->id);
	glBindTexture(GL_TEXTURE_2D, screen_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_tbo->id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	vector<string> VS; VS.emplace_back("./Shader/screen_vertex.glsl"sv);
	vector<string> FS; FS.emplace_back("./Shader/screen_fragment.glsl"sv);
	vector<string> GS;
	screen_shader = Shader::create(VS, FS, GS);
}