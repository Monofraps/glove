/*
 * Shader.cpp
 *
 *  Created on: Jul 29, 2014
 *      Author: monofraps
 */

#include "ShaderProgram.h"

#include <string>
#include <cstring>

#include <GL/glew.h>

#include "core/GloveException.h"
#include "log/Log.h"
#include "utils/FileUtils.h"
#include "utils/ShaderUtils.h"

// Default program slots
#define DPS_VERTEX 0
#define DPS_FRAGMENT 1

namespace glove {

ShaderProgram::ShaderProgram() : ShaderProgram(2) {}

ShaderProgram::ShaderProgram(int numShaders) {
	shaderProgramId = 0;
	this->numShaders = numShaders;

	shaderIds = GLOVE_NEW_ARRAY("Shader/ProgramIdsBuffer", GLuint, numShaders);
	memset(shaderIds, 0, sizeof(GLuint) * numShaders);
}

ShaderProgram::~ShaderProgram() {
	for (int i = 0; i < numShaders; ++i) {
		if (shaderIds[i] == 0) {
			LOG(logging::globalLogger, warning, "Shader not initialized");
			continue;
		}

		glDetachShader(shaderProgramId, shaderIds[i]);
		glDeleteShader(shaderIds[i]);
	}

	glDeleteProgram(shaderProgramId);

	GLOVE_DELETE(shaderIds);
}

void ShaderProgram::LoadVertexShader(std::string filePath) {
	if (shaderIds[DPS_VERTEX] != 0) {
		OLOG(error, "Vertex shader has already been set");
		return;
	}
	LoadShader(DPS_VERTEX, GL_VERTEX_SHADER, filePath);
}

void ShaderProgram::LoadFragmentShader(std::string filePath) {
	if (shaderIds[DPS_FRAGMENT] != 0) {
		OLOG(error, "Fragment shader has already been set");
		return;
	}
	LoadShader(DPS_FRAGMENT, GL_FRAGMENT_SHADER, filePath);
}

void ShaderProgram::LoadShader(int shaderSlot, GLenum shaderType, std::string& filePath) {
	std::string code = ReadFileToString(filePath);
	int codeLen = code.length();

	if (code.empty()) {
		OLOG(error, "No shader source");
	}

	GLuint shaderId = glCreateShader(shaderType);
	const char* cStrShaderSource = code.c_str();
	glShaderSource(shaderId, 1, &cStrShaderSource, &codeLen);

	GLint compileStatus = 0;
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == false) {
		OLOG(error, "Failed to compile shader");
		PrintShaderCompilerLog(shaderId);

		glDeleteShader(shaderId);
	}
	else {
		SetShader(shaderSlot, shaderId);
	}

}

void ShaderProgram::SetShader(int shaderSlot, GLuint shaderId) {
	if (shaderSlot >= numShaders)  throw new GloveException("Shader slot is out of bounds");
	if (shaderIds[shaderSlot] != 0) throw new GloveException("Shader slot has already been set");
	if (shaderId == 0) throw new GloveException("Target shader program ID is 0 (zero)");

	shaderIds[shaderSlot] = shaderId;
}

void ShaderProgram::CreateProgram() {
	shaderProgramId = glCreateProgram();
	for (int i = 0; i < numShaders; ++i) {
		if (shaderIds[i] == 0) {
			OLOG(warning, "Shader not initialized");
			continue;
		}
		glAttachShader(shaderProgramId, shaderIds[i]);
	}

	glLinkProgram(shaderProgramId);

	GLint linkStatus;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == false) {
		OLOG(error, "Failed to link shader program");
		PrintShaderProgramLinkLog(shaderProgramId);
	}
}

void ShaderProgram::Enable() {
	glUseProgram(shaderProgramId);
}

void ShaderProgram::MapVertexAttribute(MappedVertexAttribute attributeIdentifier, std::string attribName) {
	// Implicitly create shader program if it hasn't been created yet
	if (shaderProgramId == 0) {
		CreateProgram();
	}

	GLint attribIndex = glGetAttribLocation(shaderProgramId, attribName.c_str());
	if (attribIndex < 0) {
		OLOG(error, "Unknown shader attribute name " << attribName);
		return;
	}

	vertexAttributeMap[attributeIdentifier] = attribIndex;
}

void ShaderProgram::MapMaterialAttribute(MappedMaterialAttribute attributeIdentifier, std::string attribName) {
	// Implicitly create shader program if it hasn't been created yet
	if (shaderProgramId == 0) {
		CreateProgram();
	}

	GLint attribIndex = glGetUniformLocation(shaderProgramId, attribName.c_str());
	if (attribIndex < 0) {
		OLOG(error, "Unknown material attribute name " << attribName);
		return;
	}

	materialAttributeMap[attributeIdentifier] = attribIndex;
}

void ShaderProgram::Disable() {
	glUseProgram(0);
}

GLuint ShaderProgram::GetVertexAttributePosition(MappedVertexAttribute attributeIdentifier) {
	if(vertexAttributeMap.find(attributeIdentifier) == vertexAttributeMap.end()) throw new GloveException("VertexAttribute has not been mapped");
	else return vertexAttributeMap.at(attributeIdentifier);
}

GLuint ShaderProgram::GetMaterialAttributePosition(MappedMaterialAttribute attributeIdentifier) {
	if (materialAttributeMap.find(attributeIdentifier) == materialAttributeMap.end()) throw new GloveException("MaterialAttribute has not been mapped");
	else return materialAttributeMap.at(attributeIdentifier);
}

} /* namespace glove */
