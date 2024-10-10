#pragma once
#include "Include.h"
#include "Minimal.h"
#define PNG_DEBUG 3
#include <png.h>

namespace ImGui
{
    void ImageEx(ImTextureID user_texture_id, const ImVec2& delta, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
}

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
    ImVec2 DrawDelta;
public:
    inline void SetDelta(ImVec2 Delta) { DrawDelta = Delta; }
    inline ImVec2 GetDelta() const { return DrawDelta; }
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
    ORImage(ORImage&&) noexcept;
    ORImage(const char* pFileName, ImVec2 Delta = { 0.0F,0.0F });
    ORImage(ORReadStraw& Source, ImVec2 Delta = { 0.0F,0.0F });
    ORImage(const char* pFileName, ImVec2 Delta, UseRelativePosition);
    ORImage(ORReadStraw& Source, ImVec2 Delta, UseRelativePosition);
    ORImage(const char* pFileName, OriginAtCenter);
    ORImage(ORReadStraw& Source, OriginAtCenter);
    ~ORImage() = default;
};

void GetPNGtextureInfo(int color_type, gl_texture_t* texinfo);
gl_texture_t* ReadPNGFromFile_Custom(ORReadStraw* Source);
