//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Geometry.h"

#include "stb_image.h"


//External dependencies //
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>
#include "model.h"
#include "skybox.h"
#include "instanced.h"
#include "filysystem.h"
#include "texture.h"



static const GLsizei WIDTH = 1024, HEIGHT = 1024; //размеры окнa
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нa
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse         = true;  // Мышка захвачена нашим приложением или нет&
static bool g_capturedMouseJustNow = false;
static int g_shaderProgram = 0;

unsigned int loadCubemap(vector<std::string> faces);

uint32_t scene_idx = 0;
constexpr uint32_t N_SCENES = 2;

uint32_t idx = 0;
constexpr uint32_t N_GO = 2;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(float3(0.0f, 0.0f, 3.0f));

RenderTexture2D* fullScreenTexture = 0;

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: //на Esc выходим из программы
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_3:
      if (action == GLFW_PRESS)
        scene_idx = (scene_idx + 1) % N_SCENES;
      break;
  case GLFW_KEY_4:
      if (action == GLFW_PRESS)
          idx = (idx + 1) % N_GO;
      break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}
//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    g_captureMouse = !g_captureMouse;


  if (g_captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}
//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = float(xpos);
    lastY = float(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - lastX;
  GLfloat yoffset = lastY - float(ypos);  

  lastX = float(xpos);
  lastY = float(ypos);

  if (g_captureMouse)
    camera.ProcessMouseMove(xoffset, yoffset);
}

void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

GLsizei CreateSphere(float radius, int numberSlices, GLuint &vao)
{
  int i, j;

  int numberParallels = numberSlices;
  int numberVertices = (numberParallels + 1) * (numberSlices + 1);
  int numberIndices = numberParallels * numberSlices * 3;

  float angleStep = (2.0f * 3.14159265358979323846f) / ((float) numberSlices);
  //float helpVector[3] = {0.0f, 1.0f, 0.0f};

  std::vector<float> pos(numberVertices * 4, 0.0f);
  std::vector<float> norm(numberVertices * 4, 0.0f);
  std::vector<float> texcoords(numberVertices * 2, 0.0f);

  std::vector<int> indices(numberIndices, -1);

  for (i = 0; i < numberParallels + 1; i++)
  {
    for (j = 0; j < numberSlices + 1; j++)
    {
      int vertexIndex = (i * (numberSlices + 1) + j) * 4;
      int normalIndex = (i * (numberSlices + 1) + j) * 4;
      int texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

      pos.at(vertexIndex + 0) = radius * sinf(angleStep * (float) i) * sinf(angleStep * (float) j);
      pos.at(vertexIndex + 1) = radius * cosf(angleStep * (float) i);
      pos.at(vertexIndex + 2) = radius * sinf(angleStep * (float) i) * cosf(angleStep * (float) j);
      pos.at(vertexIndex + 3) = 1.0f;

      norm.at(normalIndex + 0) = pos.at(vertexIndex + 0) / radius;
      norm.at(normalIndex + 1) = pos.at(vertexIndex + 1) / radius;
      norm.at(normalIndex + 2) = pos.at(vertexIndex + 2) / radius;
      norm.at(normalIndex + 3) = 1.0f;

      texcoords.at(texCoordsIndex + 0) = (float) j / (float) numberSlices;
      texcoords.at(texCoordsIndex + 1) = (1.0f - (float) i) / (float) (numberParallels - 1);
    }
  }

  int *indexBuf = &indices[0];

  for (i = 0; i < numberParallels; i++)
  {
    for (j = 0; j < numberSlices; j++)
    {
      *indexBuf++ = i * (numberSlices + 1) + j;
      *indexBuf++ = (i + 1) * (numberSlices + 1) + j;
      *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);

      *indexBuf++ = i * (numberSlices + 1) + j;
      *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);
      *indexBuf++ = i * (numberSlices + 1) + (j + 1);

      int diff = int(indexBuf - &indices[0]);
      if (diff >= numberIndices)
        break;
    }
    int diff = int(indexBuf - &indices[0]);
    if (diff >= numberIndices)
      break;
  }

  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboIndices);

  glBindVertexArray(vao);

  glGenBuffers(1, &vboVertices);
  glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
  glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), &pos[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
  glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &vboTexCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
  glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);

  return indices.size();
}

GLsizei CreatePlaneTex(GLuint& vao)
{
    float3 position[] = {
        float3(-1.0f, -1.0f, 0.0f),
        float3(1.0f, -1.0f, 0.0f),
        float3(1.0f, 1.0f, 0.0f),
        float3(-1.0f, 1.0f, 0.0f),
    };

    float3 normals[] = {
        float3(0.0f, 0.0f, 1.0f),
        float3(0.0f, 0.0f, 1.0f),
        float3(0.0f, 0.0f, 1.0f),
        float3(0.0f, 0.0f, 1.0f),
    };

    float2 texcoords[] = {
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 1.0f),
    };

    float3 tangent[] = {
        float3(1.0f, 0.0f, 0.0f),
        float3(1.0f, 0.0f, 0.0f),
        float3(1.0f, 0.0f, 0.0f),
        float3(1.0f, 0.0f, 0.0f),
    };

    uint32_t indices[] = {0, 1, 2, 2, 3, 0};

    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords, vboTangents;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, position, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 2, texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboTangents);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, tangent, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return 6;
}

int initGL()
{
	int res = 0;

	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	//выводим в консоль некоторую информацию о драйвере и контексте opengl
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 


    GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

    camera.moveSpeed = 50.0f;

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);
	glfwSetInputMode          (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	if(initGL() != 0) 
		return -1;
	
    //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram////
	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "../shaders/vertex_models.glsl";
	shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_models.glsl";
	ShaderProgram bfShader(shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> light_shaders;
    light_shaders[GL_VERTEX_SHADER] = "../shaders/vertex_light.glsl";
    light_shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_light.glsl";
    ShaderProgram lightShader(light_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> skybox_shaders;
    skybox_shaders[GL_VERTEX_SHADER] = "../shaders/vertex_skybox.glsl";
    skybox_shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_skybox.glsl";
    ShaderProgram skyboxShader(skybox_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> instanced_shaders;
    instanced_shaders[GL_VERTEX_SHADER] = "../shaders/vertex_instanced.glsl";
    instanced_shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_instanced.glsl";
    ShaderProgram instShader(instanced_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> texture_shaders;
    texture_shaders[GL_VERTEX_SHADER] = "../shaders/vertex_texture.glsl";
    texture_shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_texture.glsl";
    ShaderProgram texShader(texture_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> quad_shaders;
    quad_shaders[GL_VERTEX_SHADER] = "../shaders/vertex_quad.glsl";
    quad_shaders[GL_FRAGMENT_SHADER] = "../shaders/frag_quad.glsl";
    ShaderProgram quadShader(quad_shaders); GL_CHECK_ERRORS;

    GLuint vaoSphere;
    float radius = 1.0f;
    GLsizei sphereIndices = CreateSphere(radius, 50, vaoSphere);

    //NORMAL_MAP
    GLuint quadVAO;
    GLsizei quadIdx = CreatePlaneTex(quadVAO);

    //RENDER_TEXTURE
    fullScreenTexture = new RenderTexture2D(GL_BGRA, GL_RGBA32F, WIDTH, HEIGHT);
    auto* quad = new FullscreenQuad();

    //PARK//////
    Model bicycle("../models/bicycle/bicycle.obj");
    Model gorka("../models/gorka/parque.obj");
    Model kacheli("../models/swing/child_swing.obj");
    Model men("../models/men/mannequinMale.obj");
    Model fonar("../models/fonar/candelabre.obj");
    Model women("../models/women/mannequinBrunette.obj");
    Model bush("../models/bush/bush.obj");

    Skybox nebo("skybox");

    Texture2D tex1("../texture/tex1_base.png");
    Texture2D tex2("../texture/tex1_norm.png");
    Texture2D tex3("../texture/tex1_parallax.png");

    //////////////
    glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
    glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float angle = 0.0f;
    float ang = 0.0f;

	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window))
	{
		//считаем сколько времени прошло за кадр
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
        doCameraMovement(camera, deltaTime);

        //START RENDER_TEXTURE 
        fullScreenTexture->StartRender();

		//очищаем экран каждый кадр/
		glClearColor(0.9f, 0.5f, 0.5f, 1.0f); GL_CHECK_ERRORS;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

        glEnable(GL_DEPTH_TEST);

        float4x4 view = camera.GetViewMatrix();
        float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);
        float4x4 model;

        unsigned int amount;

        switch (scene_idx)
        {
        case 0:
            //SKYBOX///
            glDepthMask(GL_FALSE);
            skyboxShader.StartUseShader(); GL_CHECK_ERRORS;
            view = camera.GetViewMatrix();
            view.row[0].w = 0.0f;
            view.row[1].w = 0.0f;
            view.row[2].w = 0.0f;
            view.row[3].x = 0.0f;
            view.row[3].y = 0.0f;
            view.row[3].z = 0.0f;
            view.row[3].w = 1.0f;
            skyboxShader.SetUniform("view", view);
            skyboxShader.SetUniform("projection", projection);
            nebo.Draw(skyboxShader);

            //BLINN-FONG///
            bfShader.StartUseShader(); GL_CHECK_ERRORS;
            view = camera.GetViewMatrix();
            bfShader.SetUniform("view", view);       GL_CHECK_ERRORS;
            bfShader.SetUniform("projection", projection); GL_CHECK_ERRORS;
            bfShader.SetUniform("camPos", camera.pos);

            lightLoad(bfShader, angle);

            model = transpose(mul(translate4x4(float3(0.0f, 4.0f, -25.0f)), scale4x4(float3(0.05f, 0.05f, 0.05f))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            kacheli.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(40.0f, 0.0f, -25.0f)), scale4x4(float3(0.05f, 0.05f, 0.05f))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            gorka.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(40.0f, 1.0f, -35.0f)), scale4x4(float3(0.05f, 0.05f, 0.05f))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            bicycle.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(35.0f, 6.0f, -35.0f)), scale4x4(float3(0.05f, 0.05f, 0.05f))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            men.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(25.0f, 6.0f, -35.0f)), scale4x4(float3(0.05f, 0.05f, 0.05f))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            women.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(cosf(angle / 10.0f) * 50 + 25.0f, 0.0f, sinf(angle / 10.f) * 50 - 25.0f)), mul(rotate_Y_4x4(-angle / 10 - 1.57f), scale4x4(float3(0.03f, 0.03f, 0.03f)))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            fonar.Draw(bfShader);
            model = transpose(mul(translate4x4(float3(cosf(angle / 10.0f + 3.14f) * 50 + 25.0f, 0.0f, sinf(angle / 10.f + 3.14f) * 50 - 25.0f)), mul(rotate_Y_4x4(-angle / 10 + 1.57f), scale4x4(float3(0.03f, 0.03f, 0.03f)))));
            bfShader.SetUniform("model", model); GL_CHECK_ERRORS;
            fonar.Draw(bfShader);

            //LIGHT///
            lightShader.StartUseShader(); GL_CHECK_ERRORS;
            lightShader.SetUniform("view", view);       GL_CHECK_ERRORS;
            lightShader.SetUniform("projection", projection); GL_CHECK_ERRORS;

            glBindVertexArray(vaoSphere);

            model = transpose(mul(translate4x4(float3(cosf(angle / 10.0f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f) * 46.51f - 25.0f)), scale4x4(float3(0.5f, 0.5f, 0.5f))));
            lightShader.SetUniform("model", model); GL_CHECK_ERRORS;
            lightShader.SetUniform("colorLight", float3(1.0f, 1.0f, 1.0f)); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLES, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(mul(translate4x4(float3(cosf(angle / 10.0f + 3.14f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f + 3.14f) * 46.51f - 25.0f)), scale4x4(float3(0.5f, 0.5f, 0.5f))));
            lightShader.SetUniform("model", model); GL_CHECK_ERRORS;
            lightShader.SetUniform("colorLight", float3(1.0f, 0.0f, 0.0f)); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLES, sphereIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            glBindVertexArray(0); GL_CHECK_ERRORS;

            //INSTANCED
            amount = Instanced(bush, ang);
            instShader.StartUseShader(); GL_CHECK_ERRORS;
            view = camera.GetViewMatrix();
            instShader.SetUniform("view", view);       GL_CHECK_ERRORS;
            instShader.SetUniform("projection", projection); GL_CHECK_ERRORS;
            instShader.SetUniform("camPos", camera.pos);
            lightLoad(instShader, angle);
            InstancedDraw(bush, amount, instShader);
            break;
        case 1:

            break;
        }

        //TEXTURE - NormalMap/
        texShader.StartUseShader(); GL_CHECK_ERRORS;
        view = camera.GetViewMatrix();
        texShader.SetUniform("view", view);       GL_CHECK_ERRORS;
        texShader.SetUniform("projection", projection); GL_CHECK_ERRORS;
        //texShader.SetUniform("lightPos", float3(cosf(angle / 10.0f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f) * 46.51f - 25.0f)); GL_CHECK_ERRORS;
        texShader.SetUniform("camPos", camera.pos); GL_CHECK_ERRORS;
        //texShader.SetUniform("lightPos", float3(cosf(angle / 10.0f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f) * 46.51f - 25.0f)); GL_CHECK_ERRORS;
        lightLoadParallax(texShader, angle);
        bindTexture(texShader, 0, "diffuseMap", tex1);
        bindTexture(texShader, 1, "normalMap", tex2);
        bindTexture(texShader, 2, "depthMap", tex3);//////

        glBindVertexArray(quadVAO);
        model = transpose(mul(translate4x4(float3(25.0f, 0.0f, -25.0f)), mul(scale4x4(float3(50.0f, 50.0f, 50.0f)), rotate_X_4x4(-1.57f))));
        texShader.SetUniform("model", model); GL_CHECK_ERRORS;
        glDrawElements(GL_TRIANGLES, quadIdx, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        glBindVertexArray(0); GL_CHECK_ERRORS;       

        //STOP SHADER PROGRAMS
        bfShader.StopUseShader(); GL_CHECK_ERRORS;
        lightShader.StopUseShader(); GL_CHECK_ERRORS;
        skyboxShader.StopUseShader(); GL_CHECK_ERRORS;
        instShader.StopUseShader(); GL_CHECK_ERRORS;

        //RENDER_TEXTURE/
        fullScreenTexture->StopRender();
        quadShader.StartUseShader();
        bindTexture(quadShader, 0, "colorTexture", (*fullScreenTexture));
        glDisable(GL_DEPTH_TEST);
        quad->Draw();
        quadShader.StopUseShader();

		glfwSwapBuffers(window); 

        switch (idx)
        {
        case 0:

            break;
        case 1:
            ang += 0.3f;
            angle += 0.1f;
            break;
        }



	}

    /*glDeleteVertexArrays(1, &vaoSphere);
    glDeleteVertexArrays(1, &vaoTriangle);
    glDeleteVertexArrays(1, &vaoSphere1);
    glDeleteVertexArrays(1, &vaoPlane);
    glDeleteVertexArrays(1, &vaoCube);
    glDeleteVertexArrays(1, &vaoCone);
    glDeleteVertexArrays(1, &vaoСylinder);*/

	glfwTerminate();
	return 0;
}