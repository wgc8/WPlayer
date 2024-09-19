#pragma once
#ifdef __cplusplus
//extern "C"
//{
//#include <GL/gl.h>
//#include <GL/glu.h>
//}
#endif // __cplusplus
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
class QOpenGLShaderProgram;
class AVFrame;
class GLRenderWidget  : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GLRenderWidget(QWidget *parent);
	~GLRenderWidget();
	void init(int w, int h, int size);
	void uninit();
	void reset(int w, int h, int size);
	void setFrame(const uint8_t* data, int size);
	void setFrame(int pos, const uint8_t* data, int size);
	void clear();
protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

private:
	QOpenGLShaderProgram* m_pShaderProgram{nullptr};
	GLuint unis[3] = { 0 }; // shader中yuv变量地址
	GLuint m_texUniformY{ 0 }, m_texUniformU{ 0 }, m_texUniformV{0};				//opengl中y、u、v三分量纹理坐标
	GLuint m_width{ 0 }, m_height{ 0 };								// 视频宽高
	GLuint m_size{ 0 };
	uint8_t* m_videoData{ nullptr };
};
