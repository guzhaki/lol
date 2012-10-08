//
// Lol Engine
//
// Copyright: (c) 2010-2012 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include "core.h"
#include "lolgl.h"

#if defined _WIN32 && defined USE_D3D9
#   define FAR
#   define NEAR
#   include <d3d9.h>
#endif

using namespace std;

#if defined USE_D3D9
extern IDirect3DDevice9 *g_d3ddevice;
#elif defined _XBOX
extern D3DDevice *g_d3ddevice;
#endif

namespace lol
{

//
// The TextureData class
// ---------------------
//

class TextureData
{
    friend class Texture;

    ivec2 m_size;
    PixelFormat m_format;

#if defined USE_D3D9
    IDirect3DTexture9 *m_tex;
#elif defined _XBOX
    D3DTexture *m_tex;
#else
    GLuint m_texid;
    GLint m_internal_format;
    GLenum m_gl_format, m_gl_type;
#endif
};

//
// The Texture class
// -----------------
//

#define GET_CLAMPED(array, index) \
    array[std::max(0, std::min((int)(index), \
                   (int)sizeof(array) / (int)sizeof(*array)))]

Texture::Texture(ivec2 size, PixelFormat format)
  : m_data(new TextureData)
{
    m_data->m_size = size;
    m_data->m_format = format;

#if defined USE_D3D9 || defined _XBOX
    static int const d3d_formats[] =
    {
        /* Unknown */
        D3DFMT_UNKNOWN,

        /* R8G8B8 */
        D3DFMT_R8G8B8,

        /* A8R8G8B8 */
#   if defined USE_D3D9
        D3DFMT_A8R8G8B8,
#   else
        /* By default the X360 will swizzle the texture. Ask for linear. */
        D3DFMT_LIN_A8R8G8B8,
#   endif
    };

    int d3d_format = GET_CLAMPED(d3d_formats, format);
#   if defined USE_D3D9
    int d3d_usage = D3DUSAGE_DYNAMIC;
#   else
    int d3d_usage = D3DUSAGE_WRITEONLY;
#   endif

    g_d3ddevice->CreateTexture(m_data->m_size.x, m_data->m_size.y, 1,
                               d3d_usage, d3d_format,
                               D3DPOOL_DEFAULT, &m_data->m_tex, NULL);
#else
    static struct
    {
        GLint internal_format;
        GLenum format, type;
    }
    const gl_formats[] =
    {
        /* Unknown */
        { 0, 0, 0 },

        /* R8G8B8 */
        { GL_RGB, GL_RGB, GL_UNSIGNED_BYTE },

        /* A8R8G8B8 */
#if __CELLOS_LV2__
        { GL_ARGB_SCE, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV },
#elif defined __native_client__ || defined HAVE_GLES_2X
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
#else
        /* Seems efficient for little endian textures */
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV },
#endif

        /* A8B8G8R8 */
#if __CELLOS_LV2__
        { GL_ARGB_SCE, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV },
#elif defined __native_client__ || defined HAVE_GLES_2X
        /* FIXME: if GL_RGBA is not available, we should advertise
         * this format as "not available" on this platform. */
        { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
#else
        /* Seems efficient for little endian textures */
        { GL_RGBA, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV },
#endif
    };

    m_data->m_internal_format = GET_CLAMPED(gl_formats, format).internal_format;
    m_data->m_gl_format = GET_CLAMPED(gl_formats, format).format;
    m_data->m_gl_type = GET_CLAMPED(gl_formats, format).type;

    glGenTextures(1, &m_data->m_texid);
    glBindTexture(GL_TEXTURE_2D, m_data->m_texid);

#   if defined __CELLOS_LV2__
    /* We need this hint because by default the storage type is
     * GL_TEXTURE_SWIZZLED_GPU_SCE. */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_ALLOCATION_HINT_SCE,
                    GL_TEXTURE_TILED_GPU_SCE);
#   endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
}

void Texture::Bind()
{
#if defined _XBOX || defined USE_D3D9
    g_d3ddevice->SetTexture(0, m_data->m_tex);
#else
#   if !defined HAVE_GLES_2X
    glEnable(GL_TEXTURE_2D);
#   endif
    glBindTexture(GL_TEXTURE_2D, m_data->m_texid);
#endif
}

void Texture::SetData(void *data)
{
#if defined _XBOX || defined USE_D3D9
    D3DLOCKED_RECT rect;
#   if defined USE_D3D9
    m_data->m_tex->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
#   else
    m_data->m_tex->LockRect(0, &rect, NULL, 0);
#   endif

    memcpy(rect.pBits, data, rect.Pitch * m_data->m_size.y);

    m_data->m_tex->UnlockRect(0);

#else
    glTexImage2D(GL_TEXTURE_2D, 0, m_data->m_internal_format,
                 m_data->m_size.x, m_data->m_size.y, 0,
                 m_data->m_gl_format, m_data->m_gl_type, data);
#endif
}

void Texture::SetSubData(ivec2 origin, ivec2 size, void *data)
{
#if defined _XBOX || defined USE_D3D9
    D3DLOCKED_RECT rect;
    m_data->m_tex->LockRect(0, &rect, NULL, 0);

    for (int j = 0; j < size.y; j++)
    {
        uint8_t *dst = (uint8_t *)rect.pBits + (origin.y + j) * rect.Pitch;
        /* FIXME: the source or destination pitch isn't necessarily 4! */
        uint8_t *src = (uint8_t *)data + j * size.x * 4;
        memcpy(dst, src, size.x * 4);
    }

    m_data->m_tex->UnlockRect(0);

#else
    glTexSubImage2D(GL_TEXTURE_2D, 0, origin.x, origin.y, size.x, size.y,
                    m_data->m_gl_format, m_data->m_gl_type, data);
#endif
}

Texture::~Texture()
{
#if defined USE_D3D9 || defined _XBOX
    m_data->m_tex->Release();
#else
    glDeleteTextures(1, &m_data->m_texid);
#endif

    delete m_data;
}

} /* namespace lol */
