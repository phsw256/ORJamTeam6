#pragma once
#include "Include.h"

#define PNG_DEBUG 3
#include <png.h>

class gl_texture_t{
public:
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLint internalFormat;
	GLuint id;
    GLuint channel;
	GLubyte *texels;
};

class ORReadStraw;
class ORStaticStraw;

class ORImage
{
private:
    gl_texture_t TextureData;
    GLuint TexID{ 0 };
    bool OK{ false };
public:
    inline ImVec2 GetSize() const { return { (float)TextureData.width,(float)TextureData.height }; }
    inline ImTextureID GetID() const { return ((ImTextureID)TexID); }
    inline int GetWidth() const { return TextureData.width; }
    inline int GetHeight() const { return TextureData.height; }
    void DrawChecked();
    bool Draw() noexcept;
    inline bool Available() const { return OK; }
    bool Load(const char* pFileName);
    bool Load(ORReadStraw& Source);
    ORImage() = default;
    ORImage(const ORImage&) = default;
    ORImage(ORImage&&);
    ORImage(const char* pFileName);
    ORImage(ORReadStraw& Source);
    ~ORImage() = default;
};

void GetPNGtextureInfo(int color_type, gl_texture_t* texinfo);
gl_texture_t* ReadPNGFromFile_Custom(ORReadStraw* Source);
