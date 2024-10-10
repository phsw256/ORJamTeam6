#include "ORImage.h"
#include "ORException.h"
#include "ORFile.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ImGui
{
    void ImageEx(ImTextureID user_texture_id, const ImVec2& delta, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)return;
        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + delta);
        ItemSize(bb);
        if (!ItemAdd(bb, 0))return;
        window->DrawList->AddImage(user_texture_id, bb.Min + delta, bb.Max, uv0, uv1, GetColorU32(tint_col));
    }
}

void ORImage::DrawChecked()
{
    if (!Available())throw ORException(u8"ORImage::DrawChecked ：绘制失败");
    ImGui::ImageEx(GetID(), DrawDelta, GetSize());
}
bool ORImage::Draw() noexcept
{
    if (!Available())return false;
    ImGui::ImageEx(GetID(), DrawDelta, GetSize());
    return true;
}
bool ORImage::Load(const char* pFileName)
{
    auto st = ORExtFileStraw(pFileName);
    return Load(st);
}
bool ORImage::Load(ORReadStraw& Source)
{
    gl_texture_t* png_tex = NULL;
    TexID = 0;
    GLint alignment;
    png_tex = ReadPNGFromFile_Custom((ORReadStraw*)(&Source));
    if (png_tex && png_tex->texels)
    {
        /* Generate texture */
        glGenTextures(1, &png_tex->id);
        glBindTexture(GL_TEXTURE_2D, png_tex->id);
        /* Setup some parameters for texture filters and mipmapping */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, png_tex->internalFormat, png_tex->width, png_tex->height, 0, png_tex->format, GL_UNSIGNED_BYTE, png_tex->texels);
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        TexID = png_tex->id;
        /* OpenGL has its own copy of texture data */
        free(png_tex->texels);
        TextureData = *png_tex;
        TextureData.texels = nullptr;
        free(png_tex);
    }
    OK = (TexID != 0);
    return OK;
}
ORImage::ORImage(ORImage&& rhs) noexcept:TextureData(rhs.TextureData), TexID(rhs.TexID), OK(rhs.OK)
{
    rhs.TexID = 0;
    rhs.OK = false;
}
ORImage::ORImage(const char* pFileName, ImVec2 Delta) : ORImage() 
{
    DrawDelta = Delta;
    Load(pFileName);
}
ORImage::ORImage(ORReadStraw& Source, ImVec2 Delta) : ORImage()
{
    DrawDelta = Delta;
    Load(Source);
}
ORImage::ORImage(const char* pFileName, ImVec2 Delta, UseRelativePosition) : ORImage()
{
    Load(pFileName);
    DrawDelta = GetSize() * Delta;
}
ORImage::ORImage(ORReadStraw& Source, ImVec2 Delta, UseRelativePosition) : ORImage()
{
    Load(Source);
    DrawDelta = GetSize() * Delta;
}
ORImage::ORImage(const char* pFileName, OriginAtCenter) : ORImage()
{
    Load(pFileName);
    DrawDelta = GetSize() * ImVec2 { -0.5F, -0.5F };
}
ORImage::ORImage(ORReadStraw& Source, OriginAtCenter) : ORImage()
{
    Load(Source);
    DrawDelta = GetSize() * ImVec2 { -0.5F, -0.5F };
}

void GetPNGtextureInfo(int color_type,  gl_texture_t *texinfo)
{
	switch (color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		texinfo->format = GL_LUMINANCE;
		texinfo->internalFormat = 1;
		break;
 
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		texinfo->format = GL_LUMINANCE_ALPHA;
		texinfo->internalFormat = 2;
		break;
 
	case PNG_COLOR_TYPE_RGB:
		texinfo->format = GL_RGB;
		texinfo->internalFormat = 3;
		break;
 
	case PNG_COLOR_TYPE_RGB_ALPHA:
		texinfo->format = GL_RGBA;
		texinfo->internalFormat = 4;
		break;
 
	default:
		/* Badness */
		break;
	}
}

void ReadStraw_Read(png_structp png_ptr,
    png_bytep data, size_t length)
{
    size_t check;
    if (png_ptr == NULL)
        return;
    check = ((ORReadStraw*)png_get_io_ptr(png_ptr))->Get(data, length);
    if (check != length)
        png_error(png_ptr, "Read Error");
}

gl_texture_t* ReadPNGFromFile_Custom(ORReadStraw* Source)
{
    gl_texture_t* texinfo;
    png_byte magic[8];
    png_structp png_ptr;
    png_infop info_ptr;
    int bit_depth, color_type;
    png_bytep* row_pointers = NULL;
    png_uint_32 w, h;
    int i;
    /* Open image file */
    if (!Source || !Source->Available())
    {
        throw ORException(u8"ReadPNGFromFile_Custom ：加载PNG文件失败");
    }
    /* Read magic number */
    Source->Get(magic, sizeof(magic));
    /* Check for valid magic number */
    if (!png_check_sig(magic, sizeof(magic)))
    {
        throw ORException(u8"ReadPNGFromFile_Custom ：PNG文件非法");
    }
    /* Create a png read struct */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        throw ORException(u8"ReadPNGFromFile_Custom ：PNG结构创建失败");
    }
    /* Create a png info struct */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw ORException(u8"ReadPNGFromFile_Custom ：PNG信息结构创建失败");
    }
    /* Create our OpenGL texture object */
    texinfo = (gl_texture_t*)malloc(sizeof(gl_texture_t));
    /* Initialize the setjmp for returning properly after a libpng error occured */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if (row_pointers) free(row_pointers);
        if (texinfo) {
            if (texinfo->texels)
                free(texinfo->texels);
            free(texinfo);
        }
        throw ORException(u8"ReadPNGFromFile_Custom ：LibPNG 发生异常");
    }
    /* Setup libpng for using standard C fread() function with our FILE pointer */
    png_init_io(png_ptr, (png_FILE_p)Source);
    png_set_read_fn(png_ptr, Source, ReadStraw_Read);
    /* Tell libpng that we have already read the magic number */
    png_set_sig_bytes(png_ptr, sizeof(magic));
    /* Read png info */
    png_read_info(png_ptr, info_ptr);
    /* Get some usefull information from header */
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    texinfo->channel = png_get_channels(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    /* Convert index color images to RGB images */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    /* Convert 1-2-4 bits grayscale images to 8 bits grayscale. */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    else if (bit_depth < 8) png_set_packing(png_ptr);
    /* Update info structure to apply transformations */
    png_read_update_info(png_ptr, info_ptr);
    /* Retrieve updated information */
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);
    texinfo->width = w;
    texinfo->height = h;
    /* Get image format and components per pixel */
    GetPNGtextureInfo(color_type, texinfo);
    /* We can now allocate memory for storing pixel data */
    texinfo->texels = (GLubyte*)malloc(sizeof(GLubyte) * texinfo->width * texinfo->height * texinfo->internalFormat);
    /* Setup a pointer array. Each one points at the begening of a row. */
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * texinfo->height);
    for (i = 0; i < texinfo->height; ++i)
    {
        row_pointers[i] = (png_bytep)(texinfo->texels + ((i)*texinfo->width * texinfo->internalFormat));
        //row_pointers[i] = (png_bytep)(texinfo->texels + ((texinfo->height - (i + 1)) * texinfo->width * texinfo->internalFormat));
    }
    /* Read pixel data using row pointers */
    png_read_image(png_ptr, row_pointers);
    /* Finish decompression and release memory */
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    /* We don't need row pointers anymore */
    free(row_pointers);
    return texinfo;
}
