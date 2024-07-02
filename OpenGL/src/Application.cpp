#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath); /* ����û�ж��ļ��Ƿ��������� is_open */
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
        { /* �ҵ�#shader��� */
            if (line.find("vertex") != std::string::npos) 
            { /* ������ɫ����� */
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) 
            { /* Ƭ����ɫ����� */
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
    unsigned int id = glCreateShader(type); /* ������Ӧ���͵���ɫ�� */
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); /* ������ɫ��Դ�� */
    glCompileShader(id); /* ������ɫ�� */

    /* ��������� */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // ��ȡ��ǰ��ɫ������״̬
    if (result == GL_FALSE) 
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); // ��ȡ��־����
        char* msg = (char*)_malloca(length * sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, msg); // ��ȡ��־��Ϣ
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << msg << std::endl;
        glDeleteShader(id); // ɾ����ɫ��
        return 0;
    }
    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); /* �������� */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    /* ����ɫ�����ӵ������� */
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program); /* ���ӳ��� */
    glValidateProgram(program); /* ��֤ */

    /* ɾ����ɫ�� */
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    /* ����λ�ø��������� */
    float positions[] = 
    {
        -0.5f, -0.5f, // 0
        0.5f, -0.5f,  // 1
        0.5f, 0.5f,   // 2
        -0.5f, 0.5f,  // 3
    };

    /* ���������������������� */
    unsigned int indices[] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); /* ���ɻ����� */
    glBindBuffer(GL_ARRAY_BUFFER, buffer); /* �󶨻����� */
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW); /* ���û��������� */

    glEnableVertexAttribArray(0); /* ���������-����0-λ�� */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); /* ���ö�������-����0 */

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

     /* ���ļ��н�����ɫ��Դ�� */
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    glUseProgram(shader); /* ʹ����ɫ������ */

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // ����

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader); // ɾ����ɫ������
    glfwTerminate();
    return 0;
}