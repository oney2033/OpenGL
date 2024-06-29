#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>

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
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); /* ���ɻ����� */
    glBindBuffer(GL_ARRAY_BUFFER, buffer); /* �󶨻����� */
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); /* ���û��������� */

    glEnableVertexAttribArray(0); /* ���������-����0-λ�� */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); /* ���ö�������-����0 */

    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0)in vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        " gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0)out vec4 color;"
        "\n"
        "void main()\n"
        "{\n"
        " color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader); /* ʹ����ɫ������ */

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader); // ɾ����ɫ������
    glfwTerminate();
    return 0;
}