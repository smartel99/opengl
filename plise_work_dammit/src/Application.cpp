// http://docs.gl/ 
// OpenGl Documentation.


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static void GLClearError();
static bool GLLogCall(const char* function, const char* file, int line);
static ShaderProgramSource ParseShader(const std::string&);
static unsigned int CompileShader(unsigned int type, const std::string& source);
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

int main( void ) {
    GLFWwindow* window;

    /* Initialize the library */
    if ( !glfwInit() )
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
    if ( !window ) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent( window );

    if ( glewInit() != GLEW_OK )
        std::cout << "Error at glewInit!" << std::endl;

    std::cout << "Using OpenGL Version " << glGetString( GL_VERSION ) << std::endl;

    // Create an openGL buffer.
    float positions[] = {
        -0.5f, -0.5f,	// 0
         0.5f, -0.5f,	// 1
         0.5f,  0.5f,	// 2
        -0.5f,  0.5f	// 3
    };

	// The values of this array represent the index of a vector in the position array.
	// No matter what type that is chosen for this array, it must be unsigned.
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0 
	};

    unsigned int buffer;
	GLCall(glGenBuffers( 1, &buffer ));
	GLCall(glBindBuffer( GL_ARRAY_BUFFER, buffer ));
	GLCall(glBufferData( GL_ARRAY_BUFFER, 6 * 2 *sizeof(float), positions, GL_STATIC_DRAW ));

	GLCall(glEnableVertexAttribArray( 0 ));
	GLCall(glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 2, 0 ));

	unsigned int ibo;	// ibo -> index buffer object.
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");	

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	GLCall(glBindBuffer( GL_ARRAY_BUFFER, 0 ));
    /* Loop until the user closes the window */
    while ( !glfwWindowShouldClose( window ) ) {
        /* Render here */
        glClear( GL_COLOR_BUFFER_BIT );

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    
        /* Swap front and back buffers */
        glfwSwapBuffers( window );

        /* Poll for and process events */
        glfwPollEvents();
    }

	glDeleteProgram(shader);
    
	glfwTerminate();
    return 0;
}

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error]: <" << error << ">: " << function << 
			" " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1 
	};

	std::string line;
	std::stringstream ss[2];

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // Equivalent to &source[0].
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length*sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fregment")
			<< " shader!" << std::endl;
		std::cout<<message<<std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}