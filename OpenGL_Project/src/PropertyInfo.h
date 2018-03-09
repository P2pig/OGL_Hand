#pragma once

#define _INFO_COMPILER()\
	printf( "_MSC_VER : %d \n", _MSC_VER );\
	printf( "_MSC_FULL_VER : %d \n", _MSC_FULL_VER );\
	printf( "_MSC_BUILD : %d \n", _MSC_BUILD );\
	printf( "_MSVC_LANG : C++%d \n\n", (_MSVC_LANG / 100) % 2000 )

#define _INFO_OPENGL()\
	printf( "OpenGL version: %s \n", glGetString( GL_VERSION ) );\
	printf( "GLSL version: %s \n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );\
	printf( "Vendor: %s \n", glGetString( GL_VENDOR ) );\
	printf( "Renderer: %s \n\n", glGetString( GL_RENDERER ) )