#include "GLRenderWidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


GLRenderWidget::GLRenderWidget(QWidget*parent)
	: QOpenGLWidget(parent)
{}

GLRenderWidget::~GLRenderWidget()
{
    uninit();
}

void GLRenderWidget::init(int w, int h, int size)
{
    m_height = h;
    m_width = w;
    if (m_videoData)
    {
        delete[] m_videoData;
        m_videoData = nullptr;
    }
    m_size = size;
    m_videoData = new uint8_t[size];
    clear();
}

void GLRenderWidget::uninit()
{
    if (m_videoData)
    {
        delete[] m_videoData;
        m_videoData = nullptr;
    }
    m_size = 0;
}

void GLRenderWidget::reset(int w, int h, int size)
{
    uninit();
    init(w, h, size);
}

void GLRenderWidget::setFrame(const uint8_t* data, int size)
{
    memcpy_s(m_videoData, m_size, data, size);
    //update();
}

void GLRenderWidget::setFrame(int pos, const uint8_t* data, int size)
{
    memcpy_s(m_videoData + pos, m_size - pos, data, size);
    //update();
}

void GLRenderWidget::clear()
{
    if (m_videoData)
    {
        int indexU = m_height * m_width;
        memset(m_videoData, 0x00, m_size);
        memset(m_videoData + indexU, 0x80, m_size - indexU);
    }

}

void GLRenderWidget::initializeGL()
{
    qDebug() << "initializeGL()";
	initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!m_pShaderProgram)
    {
        m_pShaderProgram = new QOpenGLShaderProgram(this);
    }
    m_pShaderProgram->create();
#if 1
    // 顶点着色器源码
    const char* vsrc =  "#version 330 core \n"
                        "layout(location = 0) in vec4 vertexIn; \n"
                        "layout(location = 1) in vec2 textureIn; \n"
                        "out vec2 textureOut; \n"
                        "void main(void) {\n"
                        "   gl_Position = vertexIn; \n"
                        "   textureOut = textureIn; \n"
                        "}";

    // 片段着色器源码
    // mat3 矩阵相乘有点问题
    const char* fsrc =  "#version 330 core \n"
                        "in highp vec2 textureOut; \n"
                        "uniform sampler2D textureY; \n"
                        "uniform sampler2D textureU; \n"
                        "uniform sampler2D textureV; \n"
                        "out vec4 fragColor; \n"
                        "void main(void) {        \n"
                        "vec3 yuv;            \n"
                        "vec3 rgb;            \n"
                        "yuv.x = texture2D(textureY, textureOut).r; \n"
                        "yuv.y = texture2D(textureU, textureOut).r - 0.5; \n"
                        "yuv.z = texture2D(textureV, textureOut).r - 0.5; \n"
                        "rgb = mat3( 1.0,    1.0,       1.0, \
                        0.0,     -0.3455 ,  1.779, \
                        1.4075,  -0.7169,  0.0) * yuv; \n"
                        "fragColor = vec4(rgb, 1); \n"
                        "}\n";
#else

    const char* vsrc = "#version 330 core \n"
                        "in vec3 aPos;\n"
                        "void main(){ \n"
                            "gl_Position = vec4(aPos, 1.0); \n"
                        "}\n";
    const char* fsrc = "#version 330 core \n"
                        "void main() {\n"
                        "gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); \n"
                        "}\n";
#endif


    m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);

    if (!m_pShaderProgram->link())
    {
        qDebug() << m_pShaderProgram->log();
    }
    if (!m_pShaderProgram->bind())
    {
        qDebug() << m_pShaderProgram->log();
    }
    // 顶点位置
    static const GLfloat vertexVertices[] = {
        -1.0f,  -1.0f,
         1.0f,  -1.0f,
        -1.0f,   1.0f,
         1.0f,   1.0f,
    };
    // 纹理位置
    static const GLfloat textureVertices[] = {
        0.0f,   1.0f,
        1.0f,   1.0f,
        0.0f,   0.0f,
        1.0f,   0.0f,
    };
#if 1
    // 绑定顶点着色器出入
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertexVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // 绑定片元着色器输入
    glGenTextures(1, &m_texUniformY);
    glGenTextures(1, &m_texUniformU);
    glGenTextures(1, &m_texUniformV);
    // Y分量
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texUniformY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // U分量
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texUniformU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // V分量
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texUniformV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    m_pShaderProgram->enableAttributeArray(0);
    m_pShaderProgram->enableAttributeArray(1);

#endif

    // 从shader获取材质
    unis[0] = m_pShaderProgram->uniformLocation("textureY");
    unis[1] = m_pShaderProgram->uniformLocation("textureU");
    unis[2] = m_pShaderProgram->uniformLocation("textureV");

 }

void GLRenderWidget::resizeGL(int w, int h)
{
    qDebug() << "resizeGL()";
 }

void GLRenderWidget::paintGL()
{
    //qDebug() << "paintGL()";
    // 清屏
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // 目前只支持YUV420格式
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texUniformY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData);
    // 关键中的关键，这里之前没有定义unis。还绑定错了
    //glUniform1i(glGetUniformLocation((GLuint)m_pShaderProgram, "textureY"), 0);
    glUniform1i(unis[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texUniformU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData + m_width * m_height);
    glUniform1i(unis[1], 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texUniformV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData + m_width * m_height * 5 / 4);
    glUniform1i(unis[2], 2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

 }
