#pragma once

using Texture = GLuint;

unsigned char* loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight);
GLuint CreatePngTexture(const char* filePath);
GLuint CreateBmpTexture(const char* filePath);
