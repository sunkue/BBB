#include "stdafx.h"
#include "Shader.h"

Shader::Shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS)
	: shader_id_{ compile_shader(filenameVS, filenameFS, includesFS) } {}

/// //////////////////////////////////////////////

bool read_file(string_view filename, std::string& target)
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

/// //////////////////////////////////////////////

void Shader::add_shader(GLuint shader_program, const char* raw_shader, GLenum shader_type)
{
	GLuint ShaderObj = glCreateShader(shader_type);
	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", shader_type);
	}

	const GLchar* p[1];
	p[0] = raw_shader;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(raw_shader);

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, InfoLog);
		printf("%s \n", raw_shader);
	}

	glAttachShader(shader_program, ShaderObj);
	glDeleteShader(ShaderObj);
}

GLuint Shader::compile_shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS)
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

	for (auto& include : includesFS)
	{
		std::string temp;
		if (!read_file(include, temp)) {
			printf("Error compiling includes\n");
			return -1;
		};
		fs += temp;
	}

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

	glUseProgram(0);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

/// //////////////////////////////////////////////

void Shader::use()const
{
	glUseProgram(shader_id_);
}

