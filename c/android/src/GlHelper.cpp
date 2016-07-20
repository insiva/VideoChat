/*
 * GlHelper.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: zzh
 */

#include<android/GlHelper.h>
#include<string.h>
#include<pthread.h>

GlHelper::GlHelper(int viewWidth, int viewHeight, int videoWidth,
		int videoHeight, const char *buildModel) :
		gTexYId(0), gTexUId(0), gTexVId(0), gProgram(0), gPositionHandle(0), nGlViewWidth(
				viewWidth), nGlViewHeight(viewHeight), nVideoWidth(videoWidth), nVideoHeight(
				videoHeight), nModelIndex(0) {
	size_t len = this->nVideoWidth * this->nVideoHeight * 3 / 2;
	this->pI420Buffer = new uchar[len];
	memset(this->pI420Buffer, 0, len);
	pthread_mutex_init(&mLock, XNULL);
	if (strcmp(buildModel, MODEL_U9180) == 0) {
		this->nModelIndex = 0;
	} else if (strcmp(buildModel, MODEL_CAM_AL00) == 0) {
		this->nModelIndex = 1;
	}
	this->init();
	DLOG("GlWidth:%d,GlHeight:%d;DecodeWidth:%d,DecodeHeight:%d.\n", viewWidth,
			viewHeight, videoWidth, videoHeight);
}

GlHelper::~GlHelper() {
	pthread_mutex_destroy(&mLock);
	if (this->pI420Buffer != XNULL) {
		delete[] this->pI420Buffer;
		this->pI420Buffer = XNULL;
	}
	this->nGlViewHeight = 0;
	this->nGlViewWidth = 0;
	this->nVideoHeight = 0;
	this->nVideoWidth = 0;
}

void GlHelper::init() {
	gProgram = GlHelper::createProgram(VERTEX_SHADER, FRAG_SHADER);
	if (!gProgram) {
		LOGE("Could not create program.");
	}
	this->gPositionHandle = glGetAttribLocation(gProgram, "vPosition");
	GlHelper::checkGlError("glGetAttribLocation");
	LOG("glGetAttribLocation(\"vPosition\") = %d\n", this->gPositionHandle);

	glViewport(0, 0, this->nGlViewWidth, this->nGlViewHeight);
	checkGlError("glViewport");

	glUseProgram(gProgram);
	glGenTextures(1, &gTexYId);
	glGenTextures(1, &gTexUId);
	glGenTextures(1, &gTexVId);
}

void GlHelper::write(const uchar *buffer, size_t len) {
	pthread_mutex_lock(&mLock);
	memcpy(this->pI420Buffer, buffer, len);
	pthread_mutex_unlock(&mLock);
	//DLOG("Write A Buffer.\n");
}

void GlHelper::render() {
	if (this->pI420Buffer == XNULL || this->gProgram == XNULL) {
		return;
	}
	//DLOG("Begin Render.\n");
	pthread_mutex_lock(&mLock);
	bindTexture(gTexYId, this->pI420Buffer, this->nVideoWidth,
			this->nVideoHeight);
	bindTexture(gTexUId,
			this->pI420Buffer + this->nVideoWidth * this->nVideoHeight,
			this->nVideoWidth / 2, this->nVideoHeight / 2);
	bindTexture(gTexVId,
			this->pI420Buffer + this->nVideoWidth * this->nVideoHeight * 5 / 4,
			this->nVideoWidth / 2, this->nVideoHeight / 2);
	pthread_mutex_unlock(&mLock);
	renderFrame();
}

void GlHelper::renderFrame() {
	// HUAWEIG510-0010 4.1.1

	glClearColor(0.0f, 0.0f, 0.0f, 1);
	checkGlError("glClearColor");
	glClear(GL_COLOR_BUFFER_BIT);
	checkGlError("glClear");
	//PRINTF("setsampler %d %d %d", g_texYId, g_texUId, g_texVId);
	GLint tex_y = glGetUniformLocation(gProgram, "SamplerY");
	checkGlError("glGetUniformLocation");
	GLint tex_u = glGetUniformLocation(gProgram, "SamplerU");
	checkGlError("glGetUniformLocation");
	GLint tex_v = glGetUniformLocation(gProgram, "SamplerV");
	checkGlError("glGetUniformLocation");

	glBindAttribLocation(gProgram, ATTRIB_VERTEX, "vPosition");
	checkGlError("glBindAttribLocation");
	glBindAttribLocation(gProgram, ATTRIB_TEXTURE, "a_texCoord");
	checkGlError("glBindAttribLocation");

	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0,
			SquareVertices[this->nModelIndex]);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	checkGlError("glEnableVertexAttribArray");

	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0,
			CoordVertices[this->nModelIndex]);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(ATTRIB_TEXTURE);
	checkGlError("glEnableVertexAttribArray");

	glActiveTexture(GL_TEXTURE0);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, gTexYId);
	checkGlError("glBindTexture");
	glUniform1i(tex_y, 0);
	checkGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE1);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, gTexUId);
	checkGlError("glBindTexture");
	glUniform1i(tex_u, 1);
	checkGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE2);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, gTexVId);
	checkGlError("glBindTexture");
	glUniform1i(tex_v, 2);
	checkGlError("glUniform1i");

	//glEnable(GL_TEXTURE_2D);
	//checkGlError("glEnable");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");
}

GLuint GlHelper::createProgram(const char *vertexShaderFunc,
		const char *fragShaderFunc) {
	GLuint vertexShader = GlHelper::buildShader(vertexShaderFunc,
	GL_VERTEX_SHADER);
	GLuint fragmentShader = buildShader(fragShaderFunc,
	GL_FRAGMENT_SHADER);
	GLuint programHandle = glCreateProgram();

	if (programHandle) {
		glAttachShader(programHandle, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(programHandle, fragmentShader);
		checkGlError("glAttachShader");
		glLinkProgram(programHandle);

		GLint linkStatus = GL_FALSE;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
					LOGE("error::Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(programHandle);
			programHandle = 0;
		}
	}
	return programHandle;
}

void GlHelper::checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

GLuint GlHelper::buildShader(const char* source, GLenum shaderType) {
	GLuint shaderHandle = glCreateShader(shaderType);

	if (shaderHandle) {
		glShaderSource(shaderHandle, 1, &source, 0);
		glCompileShader(shaderHandle);

		GLint compiled = 0;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
					LOGE("error::Could not compile shader %d:\n%s\n",
							shaderType, buf);
					free(buf);
				}
				glDeleteShader(shaderHandle);
				shaderHandle = 0;
			}
		}

	}

	return shaderHandle;
}

GLuint GlHelper::bindTexture(GLuint texture, const unsigned char *buffer,
		GLuint w, GLuint h) {
	checkGlError("glGenTextures");
	glBindTexture(GL_TEXTURE_2D, texture);
	checkGlError("glBindTexture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE,
	GL_UNSIGNED_BYTE, buffer);
	checkGlError("glTexImage2D");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	checkGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	checkGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	checkGlError("glTexParameteri");
	//glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
