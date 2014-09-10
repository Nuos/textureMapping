//
//  ShaderLoader.cpp
//  draw-triangle
//
//  Created by sbx_fc on 14-8-25.
//  Copyright (c) 2014年 rungame. All rights reserved.
//

#include "ShaderLoader.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

ShaderLoader::ShaderLoader():_programID(0)
{
    
}

/**
 * @param vertShader 顶点着色器代码
 * @param fragShader 片段着色器代码
 *
 */
bool ShaderLoader::load(const char* v_fname, const char* f_fname)
{
    //临时着色器对象
    GLuint vsId, fsId;
    compileShader(&vsId, GL_VERTEX_SHADER, v_fname);
    compileShader(&fsId, GL_FRAGMENT_SHADER, f_fname);
    
    //创建一个程序容器
    _programID = glCreateProgram();
    
    //将顶点Shader容器添加到程序中
    if (vsId)
    {
        glAttachShader(_programID, vsId);
    }
    
    if (fsId)
    {
        glAttachShader(_programID, fsId);
    }
    return link();
}

///着色器
///@param type 着色器类型 顶点着色器或者是片段着色器
bool ShaderLoader::compileShader(GLuint* shader, GLenum shaderType, const GLchar* fname)
{
    //读取Shader文件
    FILE* fp = fopen(fname,"rt");
    
    if(fp == NULL)
    {
        return false;
    }
    
    vector<string> sLines;
    char sz[255];
    
    while( fgets(sz,255,fp) )
    {
        sLines.push_back( sz );
    }
    
    const char** s_code = new const char*[sLines.size()];
    
    int i=0;
    
    for(i=0;i<int(sLines.size());i++)
    {
        s_code[i] = sLines[i].c_str();
    }

    
    //创建Shader
    auto shaderId = glCreateShader(shaderType);
    
    //替换着色器对象中的Shader源码字符
    glShaderSource(shaderId,              //GLuint shader,指定要替换源代码的Shader
                   (GLint)sLines.size(),  //GLsizei count,字符指针数组string中的元素个数
                   s_code,                //const GLchar **string,字符指针数组
                   NULL                   //const GLint *length,是一个数组，数组中的元素代表了strings相应下标的字符串的长度。
                   );
    
    //编译Shader着色器
    glCompileShader(shaderId);
    
    delete[] s_code;
    sLines.erase(sLines.begin(),sLines.end());
    
    //如果编译失败，获取相应的编译信息
    GLint succ;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &succ);
    if (succ == GL_FALSE)
    {
        GLsizei len = 0;
        GLchar *log;
        
        //获取着色器失败日志
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);
        log = (GLchar*)malloc(len);
        glGetShaderInfoLog(shaderId, len, &succ, log);
        
        /* print an error message and the info log */
        fprintf(stderr, "Unable to compile shader: log is%s\n", log);
        free(log);
        glDeleteShader(shaderId);
        return false;
    }
    
    *shader = shaderId;
    return succ == GL_TRUE;
}

 /*
 * 属性绑定到着色器制定位置。
 * 将顶点属性绑定到程序之后需要对程序进行连接。
 * @param index 绑定的属性的索引
 * @param name 绑定的属性名称
 */
void ShaderLoader::addAttribute(const char* name, GLuint index)
{
    glBindAttribLocation(_programID, index, name);
}

/**
 *  连接程序容器，
 */
bool ShaderLoader::link()
{
    GLint result;
    
    /*
    *  在连接操作执行以后，你可以任意修改shader的源代码、对shader重新编译
    *  不会影响整个程序，除非重新连接程序
    */
    glLinkProgram(_programID);
    
    if (_vertShader)
    {
        /*
        *  删除Shader,释放shader占用的内存,并使该shader失效
        *  但并非真正删除程序里的Shader
        *  删除程序里的shader参见
        *  void glDetachShader(GLuint program, GLuint shader);
        */
        glDeleteShader(_vertShader);
    }
    
    if (_fragShader)
    {
        glDeleteShader(_fragShader);
    }
    
    //获取链接状态
    glGetProgramiv(_programID, GL_LINK_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLint length;
        /* get the program info log */
        glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &length);
        char* log = (char*)malloc(length);
        glGetProgramInfoLog(_programID, length, &result, log);
        
        /* print an error message and the info log */
        fprintf(stderr, "Program linking failed: %s\n", log);
        free(log);
        
        /* delete the program */
        glDeleteProgram(_programID);
        _programID = 0;
    }
    _vertShader = _fragShader = 0;
    return result == GL_TRUE;
}

void ShaderLoader::bind()
{
    //加载并使用连连接好的程序。
    //如果程序已经在使用时候对程序进行重新编译，编译后的应用程序会自动替换以前被调用的程序，
    //不需要重新调用此函数。
    glUseProgram(_programID);
}

unsigned int ShaderLoader::getProgramID()
{
    return _programID;
}

void ShaderLoader::unbind()
{
    //使用固定功能管线
    glUseProgram(0);
}

ShaderLoader::~ShaderLoader()
{
    if(_programID)
    {
        glDeleteProgram(_programID);
    }
}

