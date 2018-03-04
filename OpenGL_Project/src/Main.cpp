#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "FrameTimer.h"

int main( void )
{
	GLFWwindow* window;

	/* Initialize the library */
	if( !glfwInit() )
		return -1;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	static constexpr int width = 800;
	static constexpr int height = 600;

	/* Create a windowed mode window and its OpenGL context */
	 window = glfwCreateWindow( width, height, "PlayGround : ) ", NULL, NULL );
	 //window = glfwCreateWindow( 1920, 1080,  "PlayGround : ) ", glfwGetPrimaryMonitor(), NULL );
	
	if( !window )
	{
		GLCall( glfwTerminate() );
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent( window );

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

	glfwSwapInterval( 1 );

	if( glewInit() != GLEW_OK )
		std::cout << "ERROR!! GLEW INIT FAILED" << std::endl;

	std::cout << glGetString( GL_VERSION ) << std::endl;
	{
		float positions[] =
		{
			-0.5f,  -0.5f,  0.0f,  0.0f,// 0
			 0.5f,  -0.5f,  1.0f,  0.0f,// 1
			 0.5f,   0.5f,  1.0f,  1.0f,// 2
			-0.5f,   0.5f,  0.0f,  1.0f // 3
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall( glEnable( GL_BLEND ) );
		GLCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );

		VertexArray va;
		VertexBuffer vb( positions, 4 * 4 * sizeof( float ) );

		VertexBufferLayout layout;
		layout.Push<float>( 2 );
		layout.Push<float>( 2 );
		va.AddBuffer( vb, layout );

		IndexBuffer ib( indices, 6 );

		//glm::mat4 proj = glm::ortho( -4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f ); // ratio 4:3
		//glm::mat4 proj = glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f ); // In world coordinates
		//Projection matrix : 45° Field of View, 4 : 3 ratio, display range : 0.1 unit < -> 100 units
		glm::mat4 proj = glm::perspective( glm::radians( 45.0f ), 4.0f / 3.0f, 1.0f, 100.0f );
		
		glm::mat4 View = glm::lookAt(
			glm::vec3( 4, 3, 3 ), // Camera is at (4,3,3), in World Space
			glm::vec3( 0, 0, 0 ), // and looks at the origin
			glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
		);

		glm::mat4 Model = glm::mat4( 1.0f );

		glm::mat4 mvp = proj * View * Model;


		Shader shader( "res/shaders/Basic.shader" );
		shader.Bind();
		shader.SetUniformMat4f( "u_MVP", mvp );

		//Texture texture( "res/textures/minion.png" );
		Texture texture( "res/textures/minion - Copy.png" );
		texture.Bind();
		shader.SetUniform1i( "u_Texture", 0 );

		Renderer renderer;
		FrameTimer ft;

		/* Loop until the user closes the window */
		while( !glfwWindowShouldClose( window ) && !glfwGetKey( window, GLFW_KEY_ESCAPE ) )
		{
			renderer.Clear();

			shader.Bind();
			renderer.Draw( va, ib, shader );


			/* Swap front and back buffers */
			glfwSwapBuffers( window );

			/* Poll for and process events */
			GLCall( glfwPollEvents() );
		}
	}
	glfwTerminate();
	return 0;
}
// working BOOOOOM