#include <stdio.h>
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_GLCOREARB
//define GLFW_INCLUDE_GLCOREARB to avoid glfw include gl.h, which is conflict with gl3.h
//ref http://stackoverflow.com/questions/19825582/modern-opengl-with-glfw3-on-mac-osx-10-8-4-missing-glgenvertexarrays-and-glb
#include <GLFW/glfw3.h>
#include "ShaderLoader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <FreeImage.h>

#define TEXTURE0 0
#define LOCATION_POSITION 0
#define LOCATION_COLOR 1
#define LOCATION_UV 2

GLuint vao;
GLuint vbo[3];

ShaderLoader shaderLoader;
GLuint texUnitIndex;

//图片大小
GLint width,height;

//存储图片信息
unsigned char * imgBuf;
GLuint mTextureID;

//加载图片
//以RGBA格式存储图片
static bool LoadImg(const char* fname)
{
    
    //初始化FreeImage
    FreeImage_Initialise(TRUE);
    
    //定义图片格式为未知
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    
    //获取图片格式
    fif = FreeImage_GetFileType(fname,0);
    
    //根据获取格式读取图片数据
    FIBITMAP* bitmap = FreeImage_Load(fif,fname,0);
    
    if(!bitmap)
    {
        printf("load error!\n");
        return false;
    }
    
    int x,y;
    RGBQUAD m_rgb;
    
    //获取图片长宽
    width = (GLint)FreeImage_GetWidth(bitmap);
    height = (GLint)FreeImage_GetHeight(bitmap);
    
    imgBuf = new unsigned char[width*height*4];
    
    //获取图片数据
    //按RGBA格式保存到数组中
    for(y=0;y<height;y++)
    {
        for(x=0;x<width;x++)
        {
            //获取像素值
            FreeImage_GetPixelColor(bitmap,x,y,&m_rgb);
            
            //将RGB值存入数组
            imgBuf[y*width*4+x*4+0] = m_rgb.rgbRed;
            imgBuf[y*width*4+x*4+1] = m_rgb.rgbGreen;
            imgBuf[y*width*4+x*4+2] = m_rgb.rgbBlue;
            
            //判断是否透明图片
            //如果是就取alpha值保存
            if(FreeImage_IsTransparent(bitmap))
                imgBuf[y*width*4+x*4+3] = m_rgb.rgbReserved;
            else
                imgBuf[y*width*4+x*4+3] = 255;
        }
    }
    
    //释放FreeImage
    FreeImage_Unload(bitmap);
    return true;
}

void initTriangle(void)
{
    static const GLfloat g_vertex_buffer_data[] = {
		-0.4f,0.4f,0.0f,
        -0.4f,-0.6f,0.0f,
        0.4f,0.4f,0.0f,
        0.4f,-0.6f,0.0f
	};
    
	static const GLfloat g_color_buffer_data[] = {
		1,0,0,
        0,1,0,
        0,0,1,
        0,1,0
	};
    
    static const GLfloat uv_data[] = {
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,1.0f,
        1.0f,0.0f,
    };
    
    //生成vbo
    glGenBuffers(3, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);
    
    //生成vao,并绑定数据
    glGenVertexArrays(1,&vao);
    
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(LOCATION_POSITION,3,GL_FLOAT,GL_FALSE,0,0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(LOCATION_COLOR,3,GL_FLOAT,GL_FALSE,0,0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glVertexAttribPointer(LOCATION_UV,2,GL_FLOAT,GL_FALSE,0,0);
        
        //设置shader属性可访问
        glEnableVertexAttribArray(LOCATION_POSITION);
        glEnableVertexAttribArray(LOCATION_COLOR);
        glEnableVertexAttribArray(LOCATION_UV);
    
    glBindVertexArray(0);
    
    
    //生成2D纹理
    glGenTextures(1, &mTextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, TEXTURE0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuf);
    glUniform1i(texUnitIndex,TEXTURE0);//设置纹理单元索引

    //设置纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

//设置并使用着色器
void initShader(void)
{
    shaderLoader.load("/Users/wistoneqqx/Documents/github/sbxOpenGL/textureMapping/draw-triangle/draw-triangle/simpleShader.vert",
                      "/Users/wistoneqqx/Documents/github/sbxOpenGL/textureMapping/draw-triangle/draw-triangle/simpleShader.frag");
    shaderLoader.bind();
    
    //获取uniform变量的位置
    texUnitIndex = glGetUniformLocation(shaderLoader.getProgramID(), "cube_texture");
}

/**
 * 绘制四边形
*/
void draw(void)
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

}

int main(void)
{
    if(false == LoadImg("/Users/wistoneqqx/Documents/github/sbxOpenGL/textureMapping/draw-triangle/draw-triangle/test1.jpg")){
        printf("加载图片失败！\n");
    }
    
    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    //使用OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    initShader();
    initTriangle();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

