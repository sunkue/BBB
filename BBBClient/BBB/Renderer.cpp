#include "stdafx.h"
#include "Renderer.h"
#include "Sun.h"
#include "UI.h"
////////////////////////////

SCREEN screen;



void Renderer::swap_player_ghost()
{
	player_.swap(ghost_);
	main_camera_->set_ownner(player_.get());
}

void Renderer::init()
{
	load_texture();
	init_shader();
	init_resources();


	KEY_BOARD_EVENT_MANAGER::get().BindMainKeyFunc(
		[this](const KEY_BOARD_EVENT_MANAGER::key_event& key)->bool
		{ return get_player()->process_input(key); });

	MOUSE_EVENT_MANAGER::get().BindMainScrollFunc(
		[this](const MOUSE_EVENT_MANAGER::scroll_event& scroll)->bool
		{ return get_player()->process_input(scroll); });
	MOUSE_EVENT_MANAGER::get().BindMainButtonFunc(
		[this](const MOUSE_EVENT_MANAGER::button_event& button)->bool
		{ return get_player()->process_input(button); });
	MOUSE_EVENT_MANAGER::get().BindMainPosFunc(
		[this](const MOUSE_EVENT_MANAGER::pos_event& pos)->bool
		{ return get_player()->process_input(pos); });
}

void Renderer::init_shader()
{
	vector<string> VS;
	vector<string> FS;
	vector<string> GS;

	VS.clear(); VS.emplace_back("./Shader/H_grass_vert.glsl"sv); VS.emplace_back("./Shader/grass_vert.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/png.frag"sv);
	GS.clear();
	grass_g_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/directional_depthmap.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/directional_depthmap.frag"sv);
	GS.clear();
	directional_depthmap_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/H_grass_vert.glsl"sv); VS.emplace_back("./Shader/directional_grass_depthmap_vert.glsl"sv);
	FS.clear(); FS.emplace_back("./Shader/directional_grass_depthmap.frag"sv);
	GS.clear();
	directional_grass_depthmap_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/point_depthmap.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/point_depthmap.frag"sv);
	GS.clear(); GS.emplace_back("./Shader/point_depthmap.geom"sv);
	point_depthmap_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/gbuffer.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/gbuffer.frag"sv);
	GS.clear(); //GS.emplace_back("./Shader/gbuffer_geomatry.glsl"sv);
	default_g_shader_ = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Shader/quad.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/sun_w_fragment.glsl"sv);
	GS.clear(); //GS.emplace_back("./Shader/gbuffer_geomatry.glsl"sv);
	sun_w_shader_ = Shader::create(VS, FS, GS);

	//	sun_b_shader_ = Shader::create(VS, FS, GS);//

	VS.clear(); VS.emplace_back("./Shader/quad.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/sky.frag"sv);
	GS.clear(); //GS.emplace_back("./Shader/gbuffer_geomatry.glsl"sv);
	skypass_shader_ = Shader::create(VS, FS, GS);

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
		VS.clear(); VS.emplace_back("./Shader/cubemap.vert"sv);
		FS.clear(); FS.emplace_back("./Shader/cubemap.frag"sv);
		GS.clear();
		auto cubeshader = Shader::create(VS, FS, GS);
		skybox = CubeMap::create(cubeshader, textures, dir);
	}

	screen_renderer_ = make_unique<ScreenRenderer>();
	screen_renderer_->init();
	depth_renderer_ = make_unique<DepthRenderer>();
	depth_renderer_->init();
	gbuffer_renderer_ = make_unique<gBufferRenderer>();
	gbuffer_renderer_->init();
	sun_renderer_ = make_unique<SunRenderer>();
	sun_renderer_->init();
}

void Renderer::init_resources()
{
	load_model();

	testing_directional_light_ = DirectionalLight::create();
	testing_point_light_ = PointLight::create();
	testing_spot_light_ = SpotLight::create();

	ubo_vp_mat.bind(grass_g_shader_, "VP_MAT");
	ubo_vp_mat.bind(default_g_shader_, "VP_MAT");

	ubo_inv_v_mat.bind(skypass_shader_, "INV_V_MAT");
	ubo_inv_p_mat.bind(skypass_shader_, "INV_P_MAT");

	ubo_default_buffer_resolution.bind(skypass_shader_, "RESOLUTION");

	ubo_lightspace_mat.bind(gbuffer_renderer_->lightpass_shader, "LIGHTSPACE_MAT");

//	int index = depth_renderer_->add_lightspace_mat(testing_directional_light_);
//	ubo_lightspace_mat.update(depth_renderer_->directional_lightspace_mat.data());

// index = depth_renderer_->add_lightspace_mat(testing_point_light_);

//	ubo_sun.update(testing_directional_light_.get());

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
	default_map = make_shared<Obj>(map);
	default_map->scaling(glm::vec3{ 50.f });


	size_t id = -1;
	cars_.emplace_back(make_shared<VehicleObj>(++id, bluecar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, pinkcar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, greencar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, pinkcar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, greencar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, greencar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, bluecar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, bluecar));
	cars_.emplace_back(make_shared<VehicleObj>(++id, bluecar));
	
	player_ = cars_.at(0);

	main_camera_ = make_shared<Camera>();
	main_camera_->set_ownner(player_.get());
	
	main_camera_->load("main_camera");
	//main_camera_->set_diff(glm::vec3{ -12.f, 5.f, 0.f });


	/// /
	grasses_ = make_shared<InstancingObj>();

	vector<Vertex> grassvertices;
	for (auto& v : cross_billboard_3)
	{
		grassvertices.push_back(v);
	}

	const auto grass_count = 62000;
	const auto grass_range = 200;

	grasses_->set_num_inst(grass_count);
	grasses_->setup_mesh(grassvertices);

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
		scales.push_back(1.f + i % 3 * 0.6f);
		yaw.push_back(glm::radians(static_cast<float>(rand() % 360)));
	}

	auto texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/grass0.png"); grasses_->add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/grass1.png");		grasses_->add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/blueflower.png"); grasses_->add_texture(texture);
	texture = Texture::create(); texture->id = CreatePngTexture("./Resource/Texture/grass/redflower.png");	grasses_->add_texture(texture);

	grasses_->add_instancing_attribute("a_scale", 10);
	grasses_->add_instancing_attribute("a_yaw", 11);
	grasses_->add_instancing_attribute("a_shearseed", 12);
	grasses_->add_instancing_attribute("a_translate", 13);

	grasses_->setup_instance_attribute("a_scale", scales.data());
	grasses_->setup_instance_attribute("a_yaw", yaw.data());
	grasses_->setup_instance_attribute("a_translate", translate.data());
	grasses_->setup_instance_attribute("a_shearseed", shearseed.data());

	/// ///////

	ghost_ = make_unique<GhostObj>(Model::no_model());
}

void Renderer::load_texture()
{

}

/////////////////////////////////////////////////////////////////////

void Renderer::ready_draw()
{
	//main_camera_->set_target(main_camera_->get_position() - glm::vec3(-0.14, -0.2, 1.f));


	auto p = proj_mat();
	auto v = main_camera_->view_mat();
	auto inv_p = glm::inverse(p);
	auto inv_v = glm::inverse(v);
	auto vp = p * v;

	ubo_vp_mat.update(glm::value_ptr(vp));

	ubo_inv_p_mat.update(glm::value_ptr(inv_p));
	ubo_inv_v_mat.update(glm::value_ptr(inv_v));

	glm::vec2 default_buffer_resolution = { screen.width, screen.height };
	
	ubo_default_buffer_resolution.update(glm::value_ptr(default_buffer_resolution));

	float n = screen.n; float f = screen.f;
	glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, n, f);
	glm::mat4 lightView = glm::lookAt(-Sky::get().get_sun_light()->direction * 1000, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	ubo_lightspace_mat.update(glm::value_ptr(lightSpaceMatrix));
}

/////////////////////////////////////////////////////////////////////

void Renderer::draw()
{
	//=> scene 별로 함수 맹글어서 교체하는 방식..
	// 매칭씬, 
	// 게임씬,

	ready_draw();

	auto gametime = static_cast<float>(GAME_SYSTEM::get().game_time()) / 1000.f;

	// 1. first render to depth map 

	{
		depth_renderer_->bind_directional_depthmap_fbo();
		glCullFace(GL_FRONT); // 깊이맵에서 피터패닝을 해결하는 법중하나. 속이 보이지 않는 물체는 이것으로 해결가능.

		depth_renderer_->set_directional_depthmap_shader(directional_depthmap_shader_, 0);

		glDisable(GL_CULL_FACE);
		default_map->update_uniform_vars(directional_depthmap_shader_);
		default_map->draw(directional_depthmap_shader_);
		glEnable(GL_CULL_FACE);

		for (auto& car : cars_)
		{
			car->update_uniform_vars(directional_depthmap_shader_);
			car->draw(directional_depthmap_shader_);
		}
		/**/

		// grass
		depth_renderer_->set_directional_depthmap_shader(directional_grass_depthmap_shader_, 0);


		//glDisable(GL_CULL_FACE);
		grasses_->update_uniform_vars(directional_grass_depthmap_shader_);
		grasses_->draw(directional_grass_depthmap_shader_);
		//	glEnable(GL_CULL_FACE);
	}

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	/// // 2. draw_gbuffers

	{
		gbuffer_renderer_->bind_gbuffer_fbo();

		glDisable(GL_CULL_FACE);
		default_map->update_uniform_vars(default_g_shader_);
		default_map->draw(default_g_shader_);
		glEnable(GL_CULL_FACE);

		for (auto& car : cars_)
		{
			car->update_uniform_vars(default_g_shader_);
			car->draw(default_g_shader_);
		}

		// grass
		glDisable(GL_CULL_FACE);
		grasses_->update_uniform_vars(grass_g_shader_);
		grasses_->draw(grass_g_shader_);
		glEnable(GL_CULL_FACE);


		// track
		track_.get().draw(default_g_shader_);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	//

	/// // sky_god_ray_____
	{
		sun_renderer_->bind_sky_fbo();


		Sky::get().update_uniform_vars(skypass_shader_);

		skypass_shader_->use();
		skypass_shader_->set("obj_texture", gbuffer_renderer_->normal_tbo); // normal_tbo

		sun_renderer_->draw_quad();
		/**/

		sun_renderer_->draw_godray();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	/// // 3. do_lights_pass

	gbuffer_renderer_->bind_lightpass_fbo();
	auto& light_shader = gbuffer_renderer_->lightpass_shader;

	auto view_pos = main_camera_->get_position();
	light_shader->set("u_view_pos", view_pos);
	testing_spot_light_->direction = main_camera_->get_look_dir();
	testing_spot_light_->position = main_camera_->get_position();
	light_shader->set("u_point_light", testing_point_light_);
	light_shader->set("u_directinal_light", testing_directional_light_);
	light_shader->set("u_spot_light", testing_spot_light_);
	light_shader->set("u_shadowmap", depth_renderer_->directional_depthmap_tbo);

	gbuffer_renderer_->draw_quad();



	//skybox->draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	// fianl. render screen pass
	screen_renderer_->blit_fbo(gbuffer_renderer_->lightpass_fbo);

	screen_renderer_->draw_screen(sun_renderer_->skypass_tbo, sun_renderer_->godraypass_tbo);

	// Render UI
	UI::get().update();
	UI::get().draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void ScreenRenderer::init()
{
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

	vector<string> VS; VS.emplace_back("./Shader/quad.vert"sv);
	vector<string> FS; FS.emplace_back("./Shader/screen.frag"sv);
	vector<string> GS;
	screen_shader = Shader::create(VS, FS, GS);
}

/////////////////////////////////////////////////////////////////////

void gBufferRenderer::init()
{
	glGenFramebuffers(1, &gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);

	//position
	worldpos_tbo = Texture::create();
	glGenTextures(1, &worldpos_tbo->id);
	glBindTexture(GL_TEXTURE_2D, worldpos_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen.width, screen.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldpos_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//normal
	normal_tbo = Texture::create();
	glGenTextures(1, &normal_tbo->id);
	glBindTexture(GL_TEXTURE_2D, normal_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen.width, screen.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// color spec
	albedospec_tbo = Texture::create();
	glGenTextures(1, &albedospec_tbo->id);
	glBindTexture(GL_TEXTURE_2D, albedospec_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen.width, screen.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedospec_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// add depth bufer,, etc..
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen.width, screen.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	////
	glGenFramebuffers(1, &lightpass_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, lightpass_fbo);

	lightpass_tbo = Texture::create();
	glGenTextures(1, &lightpass_tbo->id);
	glBindTexture(GL_TEXTURE_2D, lightpass_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightpass_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	/*
	rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen.width, screen.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	*/


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	vector<string> VS; VS.emplace_back("./Shader/quad.vert"sv);
	vector<string> FS; FS.emplace_back("./Shader/H_light.glsl"sv); FS.emplace_back("./Shader/lightpass_frag.glsl"sv);
	vector<string> GS;
	lightpass_shader = Shader::create(VS, FS, GS);
}

///////////////////////////////////////////////

ScreenQuad::ScreenQuad()
{
	glGenVertexArrays(1, &quad_vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	struct QUAD
	{
		glm::vec2 pos;
		//		glm::vec2 tex;  // gl_Position.y / 2 + 0.5f
	};

	QUAD quadv[] =
	{
		{ {-1,-1}}//,{0,0} }
		,{{ 1, 1}}//,{1,1} }
		,{{-1, 1}}//,{0,1} }
		,{{-1,-1}}//,{0,0} }
		,{{ 1,-1}}//,{1,0} }
		,{{ 1, 1}}//,{1,1} }
	};

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

	glBindVertexArray(0);
}

void ScreenQuad::draw_quad()
{
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

///////////////////////////////////////////////

void SunRenderer::init()
{
	godray_param.load("godray");

	glGenFramebuffers(1, &sky_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, sky_fbo);

	skypass_tbo = Texture::create();
	glGenTextures(1, &skypass_tbo->id);
	glBindTexture(GL_TEXTURE_2D, skypass_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skypass_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen.width, screen.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	*/

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/// /
	glGenFramebuffers(1, &godray_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, godray_fbo);

	godraypass_tbo = Texture::create();
	glGenTextures(1, &godraypass_tbo->id);
	glBindTexture(GL_TEXTURE_2D, godraypass_tbo->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, godraypass_tbo->id, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/// / 
	vector<string> VS;
	vector<string> FS;
	vector<string> GS;

	VS.clear(); VS.emplace_back("./Shader/quad.vert"sv);
	FS.clear(); FS.emplace_back("./Shader/godray.frag"sv);
	GS.clear();
	godraypass_shader = Shader::create(VS, FS, GS);
}

/////////////////////////

void GodRayParam::update_uniform_vars(const ShaderPtr& shader)
{
	auto vp = Renderer::get().vp_mat();
	glm::vec3 lightdir = Sky::get().get_sun_light()->direction;
	glm::vec3 camerapos = Renderer::get().get_main_camera()->get_position();
	glm::vec3 cameralook = Renderer::get().get_main_camera()->get_look_dir();
	glm::vec3 lightpos = camerapos - lightdir * 1e6f;

	glm::vec4 pos = vp * glm::translate(lightpos) * glm::vec4(0.0, 60.0, 0.0, 1.0);
	pos = pos / pos.w;
	pos = pos * 0.5f + 0.5f;

	glm::vec2 texcoord_lightpos = pos;

	shader->use();
	
	shader->set("u_texcoord_lightpos", texcoord_lightpos);
	shader->set("u_light_dot_camera", glm::dot( -lightdir, cameralook));

	shader->set("u_samples", u_samples);
	shader->set("u_density", u_density);
	shader->set("u_decay", u_decay);
	shader->set("u_weight", u_weight);
	shader->set("u_exposure", u_exposure);
}

void GodRayParam::load_file_impl(ifstream& file)
{
	LOAD_FILE(file, enable_godray);
	LOAD_FILE(file, u_samples);
	LOAD_FILE(file, u_decay);
	LOAD_FILE(file, u_density);
	LOAD_FILE(file, u_weight);
	LOAD_FILE(file, u_exposure);
}

void GodRayParam::save_file_impl(ofstream& file)
{
	SAVE_FILE(file, enable_godray);
	SAVE_FILE(file, u_samples);
	SAVE_FILE(file, u_decay);
	SAVE_FILE(file, u_density);
	SAVE_FILE(file, u_weight);
	SAVE_FILE(file, u_exposure);
}

/////////////////////////////////////