#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "PropertyInfo.h"

#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while( glGetError() != GL_NO_ERROR );
}

static bool GLLogCall( const char* function, const char* file, int line )
{
	while( GLenum error = glGetError() )
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function <<
			" " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader( const std::string& filepath )
{
	std::ifstream stream( filepath );

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[ 2 ];
	ShaderType type = ShaderType::NONE;
	while( getline( stream, line ) )
	{
		if( line.find( "#shader" ) != std::string::npos )
		{
			if( line.find( "vertex" ) != std::string::npos )
				type = ShaderType::VERTEX;
			else if( line.find( "fragment" ) != std::string::npos )
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[ (int) type ] << line << '\n';
		}
	}
	return { ss[ 0 ].str(), ss[ 1 ].str() };
}

static unsigned int CompileShader( unsigned int type, const std::string& source )
{
	unsigned int id = glCreateShader( type );
	const char* src = source.c_str();
	glShaderSource( id, 1, &src, nullptr );
	glCompileShader( id );

	int result;
	glGetShaderiv( id, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{
		int length;
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
		char* message = (char*) alloca( length * sizeof( char ) );
		glGetShaderInfoLog( id, length, &length, message );
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		glDeleteShader( id );
		return 0;
	}
	return id;
}

static unsigned int CreateShader( const std::string& vertexShader, const std::string& fragmentShader )
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader( GL_VERTEX_SHADER, vertexShader );
	unsigned int fs = CompileShader( GL_FRAGMENT_SHADER, fragmentShader );

	glAttachShader( program, vs );
	glAttachShader( program, fs );
	glLinkProgram( program );
	glValidateProgram( program );

	glDeleteShader( vs );
	glDeleteShader( fs );
	return program;
}

int main( void )
{
	GLFWwindow* window;

	/* Initialize the library */
	if( !glfwInit() )
		return -1;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
	if( !window )
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );

	glfwSwapInterval( 1 );

	if( glewInit() != GLEW_OK )
		std::cout << "ERROR!! GLEW INIT FAILED" << std::endl;

	_INFO_COMPILER();
	_INFO_OPENGL();


	typedef struct Vertex {
		glm::vec4 XYZW;
		glm::vec4 RGBA;
	};

	Vertex Vertices[] =	{
		//------------------------------------
		//		     4----------7
		//		    /|         /|
		//		   0----------3 |
		//		   | |        | |
		//		   | 5--------|-6
		//		   |/         |/
		//		   1----------2
		//------------------------------------
		glm::vec4( -0.5f, 0.5f, 0.5f, 1.0f ), glm::vec4( 1.0f, 1.0f, 1.0f, 0.0f ), // 0
		glm::vec4( -0.5f,-0.5f, 0.5f, 1.0f ), glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f ), // 1
		glm::vec4(  0.5f,-0.5f, 0.5f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ), // 2
		glm::vec4(  0.5f, 0.5f, 0.5f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ), // 3
		glm::vec4( -0.5f, 0.5f,-0.5f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ), // 4
		glm::vec4( -0.5f,-0.5f,-0.5f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ), // 5
		glm::vec4(  0.5f,-0.5f,-0.5f, 1.0f ), glm::vec4( 1.0f, 1.0f, 1.0f, 0.0f ), // 6
		glm::vec4(  0.5f, 0.5f,-0.5f, 1.0f ), glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f )  // 7
	};

	unsigned int Indices[] = {
		// front & back
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,

		// top & bot
		4, 0, 3,
		4, 3, 7,
		5, 1, 2,
		5, 2, 6,

		// lef & right
		0, 4, 5,
		0, 5, 1,
		3, 7, 6,
		3, 6, 2
	};

	unsigned int vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	unsigned int vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( Vertices ), Vertices, GL_STATIC_DRAW );

	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (const void*) sizeof( Vertices->XYZW ) );

	unsigned int ibo;
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( Indices ), Indices, GL_STATIC_DRAW );


	ShaderProgramSource source = ParseShader( "res/shaders/Basic.shader" );

	unsigned int shader = CreateShader( source.VertexSource, source.FragmentSource );
	glUseProgram( shader );

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective( glm::radians( 45.0f ), 4.0f / 3.0f, 0.1f, 100.0f );

	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3( 0, 1, 3 ), // Camera is at (4,3,3), in World Space
		glm::vec3( 0, 0, 0 ), // and looks at the origin
		glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4( 1.0f );
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	glUniformMatrix4fv( glGetUniformLocation( shader, "mvp" ), 1, 0, &mvp[ 0 ][ 0 ] );

	glEnable( GL_DEPTH_TEST );

	while( !glfwWindowShouldClose( window ) && !glfwGetKey( window, GLFW_KEY_ESCAPE ) )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//glDrawElements( GL_TRIANGLES, sizeof(Indices)/sizeof( unsigned int ), GL_UNSIGNED_INT, nullptr );

			glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL );

		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glBindVertexArray( 0 );

	glDeleteProgram( shader );

	glfwTerminate();
	return 0;
}


// working BOOOOOM