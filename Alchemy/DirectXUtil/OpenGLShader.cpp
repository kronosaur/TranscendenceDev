#include "PreComp.h"

#include "OpenGLShader.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

string textFileRead(const char *fileName) {
	ifstream in(fileName);
	string contents((istreambuf_iterator<char>(in)),
		istreambuf_iterator<char>());
	return contents;
}

Shader::Shader() {

}

Shader::Shader(const char *vsFile, const char *fsFile) {
	init(vsFile, fsFile);
}

void Shader::init(const char *vsFile, const char *fsFile) {
	char err_log[512];
	shader_vp = glCreateShader(GL_VERTEX_SHADER);
	shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

	// We must get the strings into separate local variables first,
	// otherwise we'll get garbage characters in our const char*s
	// See https://stackoverflow.com/questions/27627413/why-does-calling-c-str-on-a-function-that-returns-a-string-not-work
	string vsString = textFileRead(vsFile);
	string fsString = textFileRead(fsFile);
	const char* vsText = vsString.c_str();
	const char* fsText = fsString.c_str();

	if (vsText == NULL || fsText == NULL) {
		::kernelDebugLogPattern("Either vertex shader or fragment shader file not found.");
		return;
	}

	glShaderSource(shader_vp, 1, &vsText, 0);
	glShaderSource(shader_fp, 1, &fsText, 0);

	int success;
	glCompileShader(shader_vp);
	glGetShaderiv(shader_vp, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader_vp, 512, NULL, err_log);
		::kernelDebugLogPattern("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", err_log);
	};
	glCompileShader(shader_fp);
	glGetShaderiv(shader_fp, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader_fp, 512, NULL, err_log);
		::kernelDebugLogPattern("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", err_log);
	};

	shader_id = glCreateProgram();
	glBindAttribLocation(shader_id, 0, "in_Position"); // Bind a constant attribute location for positions of vertices
	glBindAttribLocation(shader_id, 1, "in_Color"); // Bind another constant attribute location, this time for color
	glAttachShader(shader_id, shader_fp);
	glAttachShader(shader_id, shader_vp);
	glLinkProgram(shader_id);
}

Shader::~Shader() {
	glDetachShader(shader_id, shader_fp);
	glDetachShader(shader_id, shader_vp);

	glDeleteShader(shader_fp);
	glDeleteShader(shader_vp);
	glDeleteProgram(shader_id);
}

unsigned int Shader::id() {
	return shader_id;
}

void Shader::bind() {
	glUseProgram(shader_id);
}

void Shader::unbind() {
	glUseProgram(0);
}

