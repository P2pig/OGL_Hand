#include <windows.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "PropertyInfo.h"
#include "Primitives\Vertex.h"
#include "Primitives\FrameTimer.h"
#include "Camera.h"

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

Camera camera;

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
	window = glfwCreateWindow( 800, 600, "Sup Cubes :D", NULL, NULL );

	if( !window )
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );

	/* Initialize GLEW */
	if( glewInit() != GLEW_OK )
		std::cout << "ERROR!! GLEW INIT FAILED" << std::endl;

	glfwSwapInterval( 0 );
	glEnable( GL_DEPTH_TEST );
	_INFO_COMPILER();
	_INFO_OPENGL();

	Vertex Vertices[] = {
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
		glm::vec4( 0.5f,-0.5f, 0.5f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ), // 2
		glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ), // 3
		glm::vec4( -0.5f, 0.5f,-0.5f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ), // 4
		glm::vec4( -0.5f,-0.5f,-0.5f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ), // 5
		glm::vec4( 0.5f,-0.5f,-0.5f, 1.0f ), glm::vec4( 1.0f, 1.0f, 1.0f, 0.0f ), // 6
		glm::vec4( 0.5f, 0.5f,-0.5f, 1.0f ), glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f )  // 7
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

	float degree = 0;
	using namespace glm;

	int nbFrames = 0;
	FrameTimer ft;
	float z1 = 0.0f;
	float z2 = 0.0f;
	float z3 = 0.0f;

	float speed1 = 1.1f;
	float speed2 = 1.0f;
	float speed3 = 1.3f;

	int t = 0;
	double xpos, ypos;

	mat4 TMatrix;
	mat4 RMatrix;
	mat4 Projection;

	float time;
	double lastTime;
	int nbframes = 0;
	double lasttime = glfwGetTime();


	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	while( !glfwWindowShouldClose( window ) && !glfwGetKey( window, GLFW_KEY_ESCAPE ) )
	{

		// Measure speed
		double currentTime = glfwGetTime();
		nbframes++;
		time = currentTime - lasttime;
		if( time >= 1.0 ){
			printf( "%f ms/frame\n", double( nbframes ) );
			nbframes = 0;
			lasttime += 1.0;
		}

		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glfwGetCursorPos( window, &xpos, &ypos );
			camera.MouseUpdate( vec2( xpos, ypos ) );

			Projection = perspective( radians( 60.0f ), 4.0f / 3.0f, 0.1f, 100.0f );
			glUniformMatrix4fv( glGetUniformLocation( shader, "Projection" ), 1, 0, &Projection[ 0 ][ 0 ] );
			glUniformMatrix4fv( glGetUniformLocation( shader, "View" ), 1, 0, &camera.View()[ 0 ][ 0 ] );

			// cube:1
			TMatrix = translate( mat4( 1.0f ), vec3( 0.0f, 0.0f, z1 ) );
			RMatrix = rotate( mat4( 1.0f ), 45.0f + degree, vec3( 0.1f, 0.1f, 0.0f ) );
			glUniformMatrix4fv( glGetUniformLocation( shader, "TMatrix" ), 1, 0, &TMatrix[ 0 ][ 0 ] );
			glUniformMatrix4fv( glGetUniformLocation( shader, "RMatrix" ), 1, 0, &RMatrix[ 0 ][ 0 ] );
			glDrawElements( GL_TRIANGLES, sizeof( Indices ) / sizeof( unsigned int ), GL_UNSIGNED_INT, nullptr );

			// cube:2
			TMatrix = translate( mat4( 1.0f ), vec3( 3.0f, 0.0f, z2 ) );
			RMatrix = rotate( mat4( 1.0f ), 45.0f - degree, vec3( 0.1f, 0.0f, 0.0f ) );
			glUniformMatrix4fv( glGetUniformLocation( shader, "TMatrix" ), 1, 0, &TMatrix[ 0 ][ 0 ] );
			glUniformMatrix4fv( glGetUniformLocation( shader, "RMatrix" ), 1, 0, &RMatrix[ 0 ][ 0 ] );
			glDrawElements( GL_TRIANGLES, sizeof( Indices ) / sizeof( unsigned int ), GL_UNSIGNED_INT, nullptr );

			// cube:3
			TMatrix = translate( mat4( 1.0f ), vec3( 1.5f, 1.5f, z3 ) );
			RMatrix = rotate( mat4( 1.0f ), (45.0f - degree), vec3( 0.5f, -0.075f, 0.0f ) );
			glUniformMatrix4fv( glGetUniformLocation( shader, "TMatrix" ), 1, 0, &TMatrix[ 0 ][ 0 ] );
			glUniformMatrix4fv( glGetUniformLocation( shader, "RMatrix" ), 1, 0, &RMatrix[ 0 ][ 0 ] );
			glDrawElements( GL_TRIANGLES, sizeof( Indices ) / sizeof( unsigned int ), GL_UNSIGNED_INT, nullptr );

			degree += 0.01;
			if( degree > 360 )
				degree = 0;

			//z1 += speed1;
			//if( z1 > 10 || z1 < -50 )
			//	speed1 = -speed1;

			//z2 += speed2;
			//if( z2 > 40 || z2 < -40 )
			//	speed2 = -speed2;

			//z3 += speed3;
			//if( z3 > 50 || z3 < -50 )
			//	speed3 = -speed3;

			glfwSwapBuffers( window );
			glfwPollEvents();
		}
	}

	glBindVertexArray( 0 );
	glDeleteProgram( shader );

	glfwTerminate();
	return 0;
}


// working BOOOOOM