#pragma once

unsigned char* loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight);
GLuint CreatePngTexture(char* filePath);
GLuint CreateBmpTexture(char* filePath);
