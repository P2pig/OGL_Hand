#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

	std::cout << glGetString( GL_VERSION ) << std::endl;

	// ==================================================================================================
	float positions[] =
	{
		// my cube lalala :D
		//------------------------------------
		//			  4----------7
		//			 /|         /|
		//			0----------3 |
		//			| |        | |
		//			| 5--------|-6
		//			|/         |/
		//			1----------2
		//------------------------------------

		// X       Y       Z
		// 0 ~ 7
		  -0.5f,   0.5f,   0.5f,
		  -0.5f,  -0.5f,   0.5f,
		   0.5f,  -0.5f,   0.5f,
		   0.5f,   0.5f,   0.5f,

		  -0.5f,   0.5f,  -0.5f,
		  -0.5f,  -0.5f,  -0.5f,
		   0.5f,  -0.5f,  -0.5f,
		   0.5f,   0.5f,  -0.5f
	};

	unsigned int indices[] = {
		// front & back
		0, 1, 2,
		0, 3, 2,
		4, 5, 6,
		4, 7, 6,

		// top & bot
		4, 0, 3,
		4, 7, 3,
		5, 1, 2,
		5, 6, 2,

		// lef & right
		0, 1, 5,
		0, 4, 5,
		3, 2, 6,
		3, 7, 6
	};


	unsigned int vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	unsigned int vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof( float ), positions, GL_STATIC_DRAW ); // 8 vertices

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), 0 );

	unsigned int ibo;
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof( unsigned int ), indices, GL_STATIC_DRAW );

	ShaderProgramSource source = ParseShader( "res/shaders/Basic.shader" );

	unsigned int shader = CreateShader( source.VertexSource, source.FragmentSource );
	glUseProgram( shader );

	while( !glfwWindowShouldClose( window ) && !glfwGetKey( window, GLFW_KEY_ESCAPE ) )
	{
		glClear( GL_COLOR_BUFFER_BIT );


		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr );


		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glDeleteProgram( shader );

	glfwTerminate();
	return 0;
}

// working BOOOOOM