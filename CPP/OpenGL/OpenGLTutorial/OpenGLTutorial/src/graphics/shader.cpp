#include "Shader.h"

// contructor
Shader::Shader() {}
Shader::Shader(const char* vertexShaderPath, const char* fragShaderPath) {
	generate(vertexShaderPath, fragShaderPath);
}

void Shader::generate(const char* vertexShaderPath, const char* fragShaderPath) {
	int success;
	char infoLog[512];

	// compile shaders
	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragShaderPath, GL_FRAGMENT_SHADER);

	// create program
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	glLinkProgram(id);

	// linking errors
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Linking error:" << std::endl << infoLog << std::endl;
	}

	// delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

// activate shader
void Shader::activate() {
	glUseProgram(id);
}

// utility functions
std::string Shader::loadShaderSrc(const char* filePath) {
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filePath);

	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
	}
	else {
		std::cout << "Could not open " << filePath << std::endl;
	}

	file.close();

	return ret;
}

GLuint Shader::compileShader(const char* filePath, GLuint type) {
	int success;
	char infoLog[512];

	GLuint ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filePath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);

	// catch error
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "Error with shader comp.:" << std::endl << infoLog << std::endl;
	}

	return ret;
}

// uniform functions
//glUniform4f(vertexColorLoc, 0.0f, 0.0f, blueValue, 1.0f);
void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::setMat4(const std::string& name, glm::mat4 val) {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}