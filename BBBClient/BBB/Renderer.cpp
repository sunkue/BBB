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
		TextRenderer::rend_text(chat_header, p, opt, true, font);

		glColor3f(color.r, color.g, color.b);
		auto header_w = TextRenderer::get_width(chat_header.data(), font);;
		p._x += header_w;
		TextRenderer::rend_text(record->str, p, opt, true, font);
		p._x -= header_w;

		p._y += TextRenderer::get_height(font);
	}
}

////////////////////////////



void Renderer::init_shader()
{
	vector<string_view> includesFS;
	includesFS.emplace_back("./Shader/IN_light.glsl"sv);
	vector<string_view> etmpty;

	default_shader_ = make_unique<Shader>("./Shader/vertex.glsl"sv, "./Shader/fragment.glsl"sv, etmpty);
	testing_shader_ = make_unique<Shader>("./Shader/test_vertex.glsl"sv, "./Shader/test_fragment.glsl"sv, includesFS);
	terrain_shader_ = make_unique<Shader>("./Shader/vertex.glsl"sv, "./Shader/terrain_fragment.glsl"sv, etmpty);
	billboard_shader_ = make_unique<Shader>("./Shader/grass_vertex.glsl"sv, "./Shader/png_fragment.glsl"sv, etmpty);
}

void Renderer::init_resources()
{
	load_texture();
	load_model();
	testing_directional_light_ = DirectionalLight::create();
	testing_point_light_ = PointLight::create();
	testing_spot_light_ = SpotLight::create();
}

void Renderer::load_model()
{	
	auto default_material = Material::create(player_tex_, player_spec_tex_);
	// create objs, give vaos for models
	auto box_vao = default_shader_->create_vao(box, 36);
	ObjDataPtr box_data = make_shared<OBJ_DATA>(box_vao);

	sky_box_ = make_shared<OBJ>(box_data, default_material);
	sky_box_->scaling(glm::vec3{ 500.f });
	sky_box_->move({ 0.f, -250.f, 0.f });

	cars_.emplace_back(make_shared<OBJ>(box_data, default_material));
	cars_[0]->move({ 10.f,5.f,2.f });
	cars_.emplace_back(make_shared<OBJ>(box_data, default_material));
	cars_[1]->move({ 2.f,0.f,14.f });

	player_ = make_shared<ControllObj>(size_t{ 0 }, box_data, default_material);

	main_camera_ = make_shared<Camera>();
	main_camera_->set_ownner(player_.get());
	main_camera_->set_diff({ -5.f, 3.f, 0.f });

	box_vao = terrain_shader_->create_vao(box, 36);
	terrain_ = make_shared<OBJ>(box_data, default_material);
	glm::vec3 scale_ = { 100.f, 0.25f, 100.f };
	terrain_->scaling(scale_);
	glm::vec3 move_ = { 0.f,(scale_.y * -1.f) - 1.f,0.f };
	terrain_->move(move_);

	auto grass_vao = billboard_shader_->create_vao(cross_billboard_3, 36);
	ObjDataPtr grass_data = make_shared<OBJ_DATA>(grass_vao);
	const auto grass_count = 10;
	const auto grass_range = 50;
	grasses_.reserve(grass_count);
	for (int i = 0; i < grass_count; i++)
	{
		grasses_.emplace_back(grass_data, default_material);
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
	std::string Dir{ "./Resource/Texture" };
	//sky_box_tex_ = CreatePngTexture((Dir + "/skydome.png").c_str());
	player_tex_ = CreatePngTexture((Dir + "/RGB.png").c_str());
	player_spec_tex_ = CreatePngTexture((Dir + "/RGB_spec.png").c_str());
	black_tex_ = CreatePngTexture((Dir + "/black.png").c_str());

	terrain_tex_ = CreatePngTexture((Dir + "/greenwool.png").c_str());

	billboard_tex0_ = CreatePngTexture((Dir + "/grass0.png").c_str());
	billboard_tex1_ = CreatePngTexture((Dir + "/grass1.png").c_str());
	billboard_tex2_ = CreatePngTexture((Dir + "/redflower.png").c_str());
	billboard_tex3_ = CreatePngTexture((Dir + "/blueflower.png").c_str());

}

/////////////////////////////////////////////////////////////////////

void Renderer::ready_draw()
{
	vp_mat_ = proj_mat() * main_camera_->view_mat();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.2f, 0.2f, 1.0f);

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




	/* draw */
	default_shader_->use();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	sky_box_->bind_vao();
	default_shader_->set_texture("u_tex_sampler", sky_box_tex_);
	sky_box_->update_uniform_vars(default_shader_.get());
	glDrawArrays(GL_TRIANGLES, 0, 36);



	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	/// <summary>
	/// // light testing....
	/// </summary>
	testing_shader_->use();
	auto view_pos = main_camera_->get_position();
	testing_shader_->set("u_view_pos", view_pos);
	testing_spot_light_->direction = main_camera_->get_look_dir();
	testing_spot_light_->position = main_camera_->get_position();

	testing_shader_->set("u_directinal_light", testing_directional_light_);
	testing_shader_->set("u_point_light", testing_point_light_);
	testing_shader_->set("u_spot_light", testing_spot_light_);

	testing_shader_->set_texture("u_tex_sampler", player_tex_);
	for (auto& car : cars_)
	{
		car->update_uniform_vars(testing_shader_.get());
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	player_->update_uniform_vars(testing_shader_.get());
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//////

	//terrain_shader_->use();
	terrain_->bind_vao();
	terrain_->update_uniform_vars(testing_shader_.get());
	testing_shader_->set_texture("u_tex_sampler", terrain_tex_);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	billboard_shader_->use();
	glPolygonMode(GL_FRONT_AND_BACK, GLU_FILL);
	glDisable(GL_CULL_FACE);
	auto time = GAME_SYSTEM::get().game_time();
	grasses_[0].bind_vao();
	for (int i = 0; auto & grass : grasses_)
	{
		glm::mat4 shear = glm::mat4(1);
		auto t = float(time) / 500 + grass.get_position().x / 10;
		auto ww = glm::cos(t) / 8;
		shear[1][0] = ww;
		shear[1][2] = ww;
		grass.update_uniform_vars(billboard_shader_.get());
		billboard_shader_->set("u_shear_mat", shear);
		billboard_shader_->set_texture("u_tex_sampler", billboard_tex0_ + (i++ % 4));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}
	glPolygonMode(GL_FRONT, GLU_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// text
	player_->render_chat({ 1,0,1 });
	render_chatrecord();

	//timer::TIMER::instance().end("T::");
	// 16.6	-> 60fps
	// 33	-> 30fps
	auto fps = 1000 / (GAME_SYSTEM::get().tick_time().count() + 1);
	string title = "("s + to_string(fps) + " fps)"s;
	glutSetWindowTitle(title.c_str());
}

