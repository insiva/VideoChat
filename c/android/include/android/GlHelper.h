/*
 * GlHelper.h
 *
 *  Created on: 2016年7月15日
 *      Author: zzh
 */

#ifndef INCLUDE_ANDROID_GLHELPER_H_
#define INCLUDE_ANDROID_GLHELPER_H_

#include<android/Config.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <pthread.h>

static const char* FRAG_SHADER = "varying lowp vec2 tc;\n"
		"uniform sampler2D SamplerY;\n"
		"uniform sampler2D SamplerU;\n"
		"uniform sampler2D SamplerV;\n"
		"void main(void)\n"
		"{\n"
		"mediump vec3 yuv;\n"
		"lowp vec3 rgb;\n"
		"yuv.x = texture2D(SamplerY, tc).r;\n"
		"yuv.y = texture2D(SamplerU, tc).r - 0.5;\n"
		"yuv.z = texture2D(SamplerV, tc).r - 0.5;\n"
		"rgb = mat3( 1,   1,   1,\n"
		"0,       -0.39465,  2.03211,\n"
		"1.13983,   -0.58060,  0) * yuv;\n"
		"gl_FragColor = vec4(rgb, 1);\n"
		"}\n";
static const char* VERTEX_SHADER = "attribute vec4 vPosition;    \n"
		"attribute vec2 a_texCoord;   \n"
		"varying vec2 tc;     \n"
		"void main()                  \n"
		"{                            \n"
		"   gl_Position = mat4("
		" 0, -1, 0, 0,\n"
		" 1, 0, 0, 0,\n"
		" 0, 0, 1, 0,\n"
		" 0, 0, 0, 1) "
		"* vPosition;  \n"
		"   tc = a_texCoord;  \n"
		"}                            \n";

enum {
	ATTRIB_VERTEX, ATTRIB_TEXTURE,
};

static GLfloat SquareVertices[][8] = {
		{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }, //中兴
		{ -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, } //华为
};
static GLfloat CoordVertices[][8] = { { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, }, //中兴
		{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } //华为
};

class GlHelper {

public:
	GlHelper(int viewWidth, int viewHeight, int videoWidth, int videoHeight,
			const char *buildModel);
	~GlHelper();

	void write(const uchar *buffer, size_t len);
	void render();
private:
	void init();
	void renderFrame();
	pthread_mutex_t mLock; // = PTHREAD_MUTEX_INITIALIZER;
	int nGlViewWidth, nGlViewHeight, nVideoWidth, nVideoHeight;
	uchar *pI420Buffer;

	GLuint gProgram;
	GLuint gPositionHandle;
	GLuint gTexYId;
	GLuint gTexUId;
	GLuint gTexVId;
	int nModelIndex;

	static GLuint createProgram(const char *vertexShader,
			const char *fragShader);
	static void checkGlError(const char* op);
	static GLuint buildShader(const char* source, GLenum shaderType);
	static GLuint bindTexture(GLuint texture, const unsigned char *buffer,
			GLuint videoWidth, GLuint videoHeight);
};

#endif /* INCLUDE_ANDROID_GLHELPER_H_ */
