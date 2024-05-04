#ifndef GFX_GPU_H
#define GFX_GPU_H

// TODO:
#undef GLAPI
#define GLAPI extern

#define SDL_GPU_DISABLE_GLEW
// #define GLEW_STATIC // needed for windows only(?)
// #include <GL/glew.h>






#if GFX_GL_MAJOR >= 2
    #define SDL_GPU_DISABLE_GLES
    #define SDL_GPU_DISABLE_OPENGL_1_BASE
    #define SDL_GPU_DISABLE_OPENGL_1    
    #if GFX_GL_MAJOR == 2
        #define _GPU_RENDERER_SRC_ "sdl_gpu/src/renderer_OpenGL_2.c"
        #define SDL_GPU_DISABLE_OPENGL_3
        #define SDL_GPU_DISABLE_OPENGL_4            
    #elif GFX_GL_MAJOR == 3    
        #define _GPU_RENDERER_SRC_ "sdl_gpu/src/renderer_OpenGL_3.c"
        #define SDL_GPU_DISABLE_OPENGL_2
        #define SDL_GPU_DISABLE_OPENGL_4
    #elif GFX_GL_MAJOR == 4
        #define _GPU_RENDERER_SRC_ "sdl_gpu/src/renderer_OpenGL_4.c"
        #define SDL_GPU_DISABLE_OPENGL_2
        #define SDL_GPU_DISABLE_OPENGL_4
    #endif         
#elif GFX_GLES_MAJOR >= 2
    #define SDL_GPU_DISABLE_OPENGL
    #if GFX_GLES_MAJOR == 2
        #define _GPU_RENDERER_SRC_ "sdl_gpu/src/renderer_GLES_2.c"
        #define SDL_GPU_DISABLE_GLES_1        
        #define SDL_GPU_DISABLE_GLES_3        
    #elif GFX_GLES_MAJOR == 3
        #define _GPU_RENDERER_SRC_ "sdl_gpu/src/renderer_GLES_3.c"
        #define SDL_GPU_DISABLE_GLES_1
        #define SDL_GPU_DISABLE_GLES_2        
    #endif   

    //  TODO: only for GLES ? need to profile...
    #define SDL_GPU_USE_BUFFER_RESET 
    // SDL_GPU_USE_BUFFER_RESET for GLES3? Android for sure...
    // When SDL_gpu does its rendering, the textures themselves stay as-is on the GPU. The only things that SDL_gpu may upload by itself each blit or each frame are the VBO that stores the vertex position, texcoord, and color data (usually 4 vertices per blit) and the IBO/EBO that stores indices into the VBO (usually 6 indices per blit). There is a performance difference when considering how this data goes to the GPU that depends on your device. You can #define SDL_GPU_USE_BUFFER_MAPPING or SDL_GPU_USE_BUFFER_RESET when building SDL_gpu to try different upload methods. Please do let me know your device model, OS version, and which of these modes works best for you.
    // https://github.com/grimfang4/sdl-gpu/issues/70
    // #define SDL_GPU_USE_BUFFER_MAPPING
    // #define SDL_GPU_USE_BUFFER_UPDATE
#else
    #error gfx/gpu requires either GL2+ or GLES2+
#endif



#include "sdl_gpu/src/SDL_gpu.c"
#include "sdl_gpu/src/SDL_gpu_matrix.c"
#include "sdl_gpu/src/SDL_gpu_shapes.c"
#include "sdl_gpu/src/SDL_gpu_renderer.c"
#include _GPU_RENDERER_SRC_

#include <math.h>
#include "sdl_gpu/common/common.c"

/*
typedef Uint32 GPU_RendererEnum;
static const GPU_RendererEnum GPU_RENDERER_UNKNOWN = 0;  // invalid value
static const GPU_RendererEnum GPU_RENDERER_OPENGL_1_BASE = 1;
static const GPU_RendererEnum GPU_RENDERER_OPENGL_1 = 2;
static const GPU_RendererEnum GPU_RENDERER_OPENGL_2 = 3;
static const GPU_RendererEnum GPU_RENDERER_OPENGL_3 = 4;
static const GPU_RendererEnum GPU_RENDERER_OPENGL_4 = 5;
static const GPU_RendererEnum GPU_RENDERER_GLES_1 = 11;
static const GPU_RendererEnum GPU_RENDERER_GLES_2 = 12;
static const GPU_RendererEnum GPU_RENDERER_GLES_3 = 13;
static const GPU_RendererEnum GPU_RENDERER_D3D9 = 21;
static const GPU_RendererEnum GPU_RENDERER_D3D10 = 22;
static const GPU_RendererEnum GPU_RENDERER_D3D11 = 23;
#define GPU_RENDERER_CUSTOM_0 1000
*/

/*! \ingroup Initialization
 * Initialization flags for changing default init parameters.  Can be bitwise OR'ed together.
 * Default (0) is to use late swap vsync and double buffering.
 * \see GPU_SetPreInitFlags()
 * \see GPU_GetPreInitFlags()
 */
// typedef Uint32 GPU_InitFlagEnum;
// static const GPU_InitFlagEnum GPU_INIT_ENABLE_VSYNC = 0x1;
// static const GPU_InitFlagEnum GPU_INIT_DISABLE_VSYNC = 0x2;
// static const GPU_InitFlagEnum GPU_INIT_DISABLE_DOUBLE_BUFFER = 0x4;
// static const GPU_InitFlagEnum GPU_INIT_DISABLE_AUTO_VIRTUAL_RESOLUTION = 0x8;
// static const GPU_InitFlagEnum GPU_INIT_REQUEST_COMPATIBILITY_PROFILE = 0x10;
// static const GPU_InitFlagEnum GPU_INIT_USE_ROW_BY_ROW_TEXTURE_UPLOAD_FALLBACK = 0x20;
// static const GPU_InitFlagEnum GPU_INIT_USE_COPY_TEXTURE_UPLOAD_FALLBACK = 0x40;
// #define GPU_DEFAULT_INIT_FLAGS 0
/*! Set special flags to use for initialization. Set these before calling GPU_Init().
 * \param GPU_flags An OR'ed combination of GPU_InitFlagEnum flags.  Default flags (0) enable late swap vsync and double buffering. */
// DECLSPEC void SDLCALL GPU_SetPreInitFlags(GPU_InitFlagEnum GPU_flags);






GPU_Image* GPU_CreateImageUsingTexture3(GPU_TextureHandle handle, GPU_bool take_ownership, GLint w, GLint h, GLint gl_format);






static GPU_Image* _CreateImageUsingTexture3(GPU_Renderer* renderer, GPU_TextureHandle handle, GPU_bool take_ownership,
    GLint w, GLint h, GLint gl_format)
{
    GLuint num_layers, bytes_per_pixel;
    GLint wrap_s, wrap_t;
    GLint min_filter;

    GPU_FormatEnum format;
    GPU_WrapEnum wrap_x, wrap_y;
    GPU_FilterEnum filter_mode;
	SDL_Color white = { 255, 255, 255, 255 };

	GPU_Image* result;
	GPU_IMAGE_DATA* data;
	
    flushAndBindTexture(renderer, handle);	

    switch(gl_format)
    {
        case GL_LUMINANCE:
            format = GPU_FORMAT_LUMINANCE;
            num_layers = 1;
            bytes_per_pixel = 1;
            break;
        case GL_LUMINANCE_ALPHA:
            format = GPU_FORMAT_LUMINANCE_ALPHA;
            num_layers = 1;
            bytes_per_pixel = 2;
            break;
        case GL_RGB:
            format = GPU_FORMAT_RGB;
            num_layers = 1;
            bytes_per_pixel = 3;
            break;
        case GL_RGBA:
        case 0x8058: //RGBA8_EXT: // D4
            format = GPU_FORMAT_RGBA;
            num_layers = 1;
            bytes_per_pixel = 4;
            break;
        #ifdef GL_BGR
        case GL_BGR:
            format = GPU_FORMAT_BGR;
            num_layers = 1;
            bytes_per_pixel = 3;
            break;
        #endif
        #ifdef GL_BGRA
        case GL_BGRA:
            format = GPU_FORMAT_BGRA;
            num_layers = 1;
            bytes_per_pixel = 4;
            break;
        #endif
        #ifdef GL_ABGR
        case GL_ABGR:
            format = GPU_FORMAT_ABGR;
            num_layers = 1;
            bytes_per_pixel = 4;
            break;
        #endif
        case GL_ALPHA:
            format = GPU_FORMAT_ALPHA;
            num_layers = 1;
            bytes_per_pixel = 1;
            break;
        #ifndef SDL_GPU_USE_GLES
        case GL_RG:
            format = GPU_FORMAT_RG;
            num_layers = 1;
            bytes_per_pixel = 2;
            break;
        #endif
        default:
            // RGBA8_EXT 0x8058
            // GL_RGBA 0x1908
            GPU_PushErrorCode("GPU_CreateImageUsingTexture", GPU_ERROR_DATA_ERROR, "Unsupported GL image format (0x%x)", gl_format);
            return NULL;
    }

	// glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	// glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);


	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
	// Ignore mag filter...  Maybe the wrong thing to do?

	// Let the user use one that we don't support and pretend that we're okay with that.
    switch(min_filter)
    {
        case GL_NEAREST:
            filter_mode = GPU_FILTER_NEAREST;
            break;
        case GL_LINEAR:
        case GL_LINEAR_MIPMAP_NEAREST:
            filter_mode = GPU_FILTER_LINEAR;
            break;
        case GL_LINEAR_MIPMAP_LINEAR:
            filter_mode = GPU_FILTER_LINEAR_MIPMAP;
            break;
        default:
            GPU_PushErrorCode("GPU_CreateImageUsingTexture", GPU_ERROR_USER_ERROR, "Unsupported value for GL_TEXTURE_MIN_FILTER (0x%x)", min_filter);
            filter_mode = GPU_FILTER_LINEAR;
            break;
    }


	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t);

	// Let the user use one that we don't support and pretend that we're okay with that.
	switch(wrap_s)
	{
    case GL_CLAMP_TO_EDGE:
        wrap_x = GPU_WRAP_NONE;
        break;
    case GL_REPEAT:
        wrap_x = GPU_WRAP_REPEAT;
        break;
    case GL_MIRRORED_REPEAT:
        wrap_x = GPU_WRAP_MIRRORED;
        break;
    default:
        GPU_PushErrorCode("GPU_CreateImageUsingTexture", GPU_ERROR_USER_ERROR, "Unsupported value for GL_TEXTURE_WRAP_S (0x%x)", wrap_s);
        wrap_x = GPU_WRAP_NONE;
        break;
	}

	switch(wrap_t)
	{
    case GL_CLAMP_TO_EDGE:
        wrap_y = GPU_WRAP_NONE;
        break;
    case GL_REPEAT:
        wrap_y = GPU_WRAP_REPEAT;
        break;
    case GL_MIRRORED_REPEAT:
        wrap_y = GPU_WRAP_MIRRORED;
        break;
    default:
        GPU_PushErrorCode("GPU_CreateImageUsingTexture", GPU_ERROR_USER_ERROR, "Unsupported value for GL_TEXTURE_WRAP_T (0x%x)", wrap_t);
        wrap_y = GPU_WRAP_NONE;
        break;
	}

	// Finally create the image

    data = (GPU_IMAGE_DATA*)SDL_malloc(sizeof(GPU_IMAGE_DATA));
    data->refcount = 1;
    data->handle = (GLuint)handle;
    data->owns_handle = take_ownership;
    data->format = gl_format;


    result = (GPU_Image*)SDL_malloc(sizeof(GPU_Image));
    result->refcount = 1;
    result->target = NULL;
    result->renderer = renderer;
    result->context_target = renderer->current_context_target;
    result->format = format;
    result->num_layers = num_layers;
    result->bytes_per_pixel = bytes_per_pixel;
    result->has_mipmaps = GPU_FALSE;

    result->anchor_x = renderer->default_image_anchor_x;
    result->anchor_y = renderer->default_image_anchor_y;

    result->color = white;
    result->use_blending = GPU_TRUE;
    result->blend_mode = GPU_GetBlendModeFromPreset(GPU_BLEND_NORMAL);
    result->snap_mode = GPU_SNAP_POSITION_AND_DIMENSIONS;
    result->filter_mode = filter_mode;
    result->wrap_mode_x = wrap_x;
    result->wrap_mode_y = wrap_y;

    result->data = data;
    result->is_alias = GPU_FALSE;

    result->using_virtual_resolution = GPU_FALSE;
    result->w = (Uint16)w;
    result->h = (Uint16)h;

    result->base_w = (Uint16)w;
    result->base_h = (Uint16)h;
    result->texture_w = (Uint16)w;
    result->texture_h = (Uint16)h;

    return result;
}


GPU_Image* GPU_CreateImageUsingTexture3(GPU_TextureHandle handle, GPU_bool take_ownership, GLint w, GLint h, GLint gl_format)
{
    if(_gpu_current_renderer == NULL || _gpu_current_renderer->current_context_target == NULL)
        return NULL;

    return _CreateImageUsingTexture3(_gpu_current_renderer, handle, take_ownership, w, h, gl_format);
}

#endif