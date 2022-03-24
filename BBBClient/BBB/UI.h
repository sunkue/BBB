#pragma once

class UI
{
	SINGLE_TON(UI) = default;

public:
	void draw();
	void update()
	{
		ranking_.update();
		// hp_;
		 //warning_;
	}

private:
	class RankingUI
	{
		static constexpr float width = 0.125;
		static constexpr float heigth = 0.0625;
		const glm::vec2 start_pos = { -0.975, 0.525 };
		const glm::vec2 diff = { 0 , heigth + 0.02 };
	public:
		RankingUI();

		void draw();

		void update();

	private:
		GLuint frame_vao_;
		vector<TexturePtr> players_; // colors for each player car. 
		vector<glm::vec2> dynamic_sps_; // ��հ� �����̴� ui. ���� ui ��ġ. �÷��̾� ��.
		vector<glm::vec2> static_sps_; // ������. ui���� ���� ������. ������.
		ShaderPtr shader_;
	};

	class HpUI
	{
	public:

	private:
		TexturePtr frame_;
		TexturePtr color_;
		ShaderPtr shader_;
	};

	class WarningUI
	{
	public:

	private:
		TexturePtr frame_;
		TexturePtr color_;
		ShaderPtr shader_;
	};

private:
	RankingUI ranking_;
	HpUI hp_;
	WarningUI warning_;
};



