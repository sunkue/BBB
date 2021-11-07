#pragma once

using TexturePtr = shared_ptr<struct Texture>;
struct Texture
{
	GLuint id;
	string type;
	string path;

	CREATE_SHARED(TexturePtr, Texture);
private:
	explicit Texture() noexcept = default;
};


unsigned char* loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight);
GLuint CreatePngTexture(const char* filePath);
GLuint CreateBmpTexture(const char* filePath);

GLuint load_texture_file(const char* path, const string& directory, bool gamma = false);