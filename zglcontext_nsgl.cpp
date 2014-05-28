/*
 * Copyright 2011-2014 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "zbgfx_p.h"

#if BX_PLATFORM_OSX && (BGFX_CONFIG_RENDERER_OPENGLES2|BGFX_CONFIG_RENDERER_OPENGLES3|BGFX_CONFIG_RENDERER_OPENGL)
#include "zrenderer_gl.h"
#	include <bx/os.h>

namespace bgfx
{

#	define GL_IMPORT(_optional, _proto, _func, _import) _proto _func
#include "zglimports.h"
	
	static void* s_opengl = NULL;

extern "C"
{
struct GlCtx_s
{
	void* view;
	void* m_context;
} typedef GlCtx;

void bgfx_GlContext_create(GlCtx *ctx, void* nswnd, uint32_t _width, uint32_t _height);
void bgfx_GlContext_destroy(GlCtx *ctx);
void bgfx_GlContext_resize(GlCtx *ctx, uint32_t _width, uint32_t _height, bool _vsync);
void bgfx_GlContext_swap(GlCtx *ctx);
}


	void GlContext::create(uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_width, _height);

		s_opengl = bx::dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL");
		BX_CHECK(NULL != s_opengl, "OpenGL dynamic library is not found!");

		bgfx_GlContext_create((GlCtx*)this, g_bgfxNSWindow, _width, _height);
		import();
	}

	void GlContext::destroy()
	{
		bgfx_GlContext_destroy((GlCtx*)this);
		bx::dlclose(s_opengl);
	}

	void GlContext::resize(uint32_t _width, uint32_t _height, bool _vsync)
	{
		bgfx_GlContext_resize((GlCtx*)this, _width, _height, _vsync);
	}

	void GlContext::swap()
	{
		bgfx_GlContext_swap((GlCtx*)this);
	}

	void GlContext::import()
	{
		BX_TRACE("Import:");
#	define GL_EXTENSION(_optional, _proto, _func, _import) \
				{ \
					if (_func == NULL) \
					{ \
						_func = (_proto)bx::dlsym(s_opengl, #_import); \
						BX_TRACE("%p " #_func " (" #_import ")", _func); \
					} \
					BGFX_FATAL(_optional || NULL != _func, Fatal::UnableToInitialize, "Failed to create OpenGL context. NSGLGetProcAddress(\"%s\")", #_import); \
				}
#include "zglimports.h"
	}

} // namespace bgfx

#endif // BX_PLATFORM_OSX && (BGFX_CONFIG_RENDERER_OPENGLES2|BGFX_CONFIG_RENDERER_OPENGLES3|BGFX_CONFIG_RENDERER_OPENGL)
