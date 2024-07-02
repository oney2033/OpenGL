#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath); /* 这里没判断文件是否能正常打开 is_open */
    enum class ShaderType 
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) 
    {
        if (line.find("#shader") != std::string::npos) 
        { /* 找到#shader标记 */
            if (line.find("vertex") != std::string::npos) 
            { /* 顶点着色器标记 */
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) 
            { /* 片段着色器标记 */
                type = ShaderType::FRAGMENT;
            }
        }
        else 
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); /* 创建对应类型的着色器 */
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); /* 设置着色器源码 */
    glCompileShader(id); /* 编译着色器 */

    /* 编译错误处理 */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // 获取当前着色器编译状态
    if (result == GL_FALSE) 
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); // 获取日志长度
        char* msg = (char*)_malloca(length * sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, msg); // 获取日志信息
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << msg << std::endl;
        glDeleteShader(id); // 删除着色器
        return 0;
    }
    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); /* 创建程序 */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    /* 将着色器附加到程序上 */
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program); /* 链接程序 */
    glValidateProgram(program); /* 验证 */

    /* 删除着色器 */
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //指定版本和兼容性配置文件
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        /* 顶点位置浮点型数组 */
        float positions[] =
        {
            -0.5f, -0.5f, // 0
            0.5f, -0.5f,  // 1
            0.5f, 0.5f,   // 2
            -0.5f, 0.5f,  // 3
        };

        /* 索引缓冲区所需索引数组 */
        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao; /* 保存顶点数组对象ID */
        GLCall(glGenVertexArrays(1, &vao)); /* 生存顶点数组 */
        GLCall(glBindVertexArray(vao)); /* 绑定顶点数组 */

        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        glEnableVertexAttribArray(0); /* 激活顶点属性-索引0-位置 */
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); /* 设置顶点属性-索引0 */

        IndexBuffer ib(indices, 6);

        /* 从文件中解析着色器源码 */
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

        glUseProgram(shader); /* 使用着色器程序 */

        int location;
        GLCall(location = glGetUniformLocation(shader, "u_Color")); /* 获取指定名称统一变量的位置 */
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f)); /* 设置对应的统一变量 */

        /* 解绑 */
        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            GLCall(glBindVertexArray(vao));
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // 绘制

            if (r > 1.0f)
            {
                increment = -0.05f;
            }
            else if (r < 0.0f)
            {
                increment = 0.05f;
            }
            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        glDeleteProgram(shader); // 删除着色器程序
    }
    glfwTerminate();
    return 0;
}