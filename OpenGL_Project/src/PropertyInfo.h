#pragma once

#define _INFO_COMPILER()\
	printf( "Compiler info:" );\
	printf( "\n\t_MSC_FULL_VER :\t%d", _MSC_FULL_VER );\
	printf( "\n\t_MSC_BUILD :   \t%d", _MSC_BUILD    );\
	printf( "\n\t_MSVC_LANG :   \tC++%d", (_MSVC_LANG / 100) % 2000 )

#define _INFO_OPENGL()\
	printf( "\nGraphics Info:");\
	printf( "\n\t OpenGL_ver: \t%s"  , glGetString( GL_VERSION ) );\
	printf( "\n\t GLSL_ver:   \t%s"  , glGetString( GL_SHADING_LANGUAGE_VERSION ) );\
	printf( "\n\t Vendor:     \t%s"  , glGetString( GL_VENDOR ) );\
	printf( "\n\t Renderer:   \t%s\n", glGetString( GL_RENDERER ) )