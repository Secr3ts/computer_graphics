#include <GL/glew.h>
#include "common/camera.h"
#include "common/model.h"
#include <GL/gl.h>
#include "common/GLShader.h"
#include "final.h"
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include "common/stb_image.h"

/* IMGui*/
#include "common/imgui/imgui.h"
#include "common/imgui/backends/imgui_impl_glfw.h"
#include "common/imgui/backends/imgui_impl_opengl3.h"

GLuint fbo_1;
GLuint color_tex_1;
GLuint depth_tex_1;

GLuint fbo_2;
GLuint color_tex_2;

GLuint tex_id; // cubemap (skybox)
GLuint tex_id_2; // perlin noise procedural

constexpr int PROC_TEX_WIDTH = 512;
constexpr int PROC_TEX_HEIGHT = 512;

GLuint quand_VAO, quadVBO;

GLShader blur_shader_id;
GLShader sepia_shaper_id;
GLShader skybox_shader_id;
GLComputeShader procedural_shader_id;
GLuint skybox_VAO;
GLuint skybox_VBO;

GLFWwindow *window;
vector<Model *> models;

Camera camera;

bool Initialize() {
  /* beginning of window initialization */
  if (glfwInit() != GLFW_TRUE) {
    fprintf(stderr, "Error initiliazing GLFW");
    return EXIT_FAILURE;
  }

  window = glfwCreateWindow(APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, "Arcball Navigation", nullptr,
                            nullptr);
  if (window == nullptr) {
    fprintf(stderr, "Error initializing Window");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  if (GLEW_OK != glewInit()) {
    fprintf(stderr, "Error initializing GLEW");
    return EXIT_FAILURE;
  }
  /* end of window initialization */

  /* beginning of post-processing shader initialization */
  sepia_shaper_id.LoadVertexShader("./shaders/sepia.vert");
  sepia_shaper_id.LoadFragmentShader("./shaders/sepia.frag");
  sepia_shaper_id.Create();

  blur_shader_id.LoadVertexShader("./shaders/blur.vert");
  blur_shader_id.LoadFragmentShader("./shaders/blur.frag");
  blur_shader_id.Create();
  /* end of post-processing shader initialization */

  const glm::vec3 positions[3] = {glm::vec3(-1.6f, 0.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                  glm::vec3(1.6f, 0.0f, 0.0f)};
  
  /*
    const glm::vec3 colors[3] = {
        glm::vec3(0.95f, 0.30f, 0.30f),
        glm::vec3(0.30f, 0.95f, 0.35f),
        glm::vec3(0.35f, 0.45f, 0.95f)};

  */

  /* beginning of model and instance initialization */
  const vector<string> mtls = {
      "obsidian",
      "brass",
      "green",
  };
  for (size_t model_i = 0; model_i < mtls.size(); model_i++) {
    Model *model = new Model();
    model->Load("./models/cube.obj", "./models", mtls[model_i]);
    model->LoadShader("./shaders/", "hemis");

    vector<InstanceData> instances;
    instances.reserve(std::size(positions));
    InstanceData base_instance = model->instance_data.empty() ? InstanceData{} : model->instance_data[0];
    for (size_t inst_i = 0; inst_i < std::size(positions); inst_i++) {
      glm::mat4 instance = glm::mat4(1.0f);
      instance = glm::translate(instance, positions[inst_i]);
      instance = glm::scale(instance, glm::vec3(0.30f + (0.05 * inst_i)));
      instance = glm::translate(instance, glm::vec3(-1.0f, -1.0f, -1.0f));

      float model_offset = (static_cast<float>(model_i) - 1.0f) * 3.0f;
      instance = glm::translate(instance, glm::vec3(0.0f, 0.0f, model_offset));

      InstanceData inst_data = base_instance;
      inst_data.m_matrix = instance;
      inst_data.m_shine = 83.2f;
      instances.push_back(inst_data);
    }

    model->SetInstanceData(instances);
    model->Upload();
    models.push_back(model);
  }
  /* end of model and instance initialization */

  /* beginning of skybox initialization */
  vector<string> faces = {
    "./images/cubemap/pisa_posx.jpg",
    "./images/cubemap/pisa_negx.jpg",
    "./images/cubemap/pisa_posy.jpg",
    "./images/cubemap/pisa_negy.jpg",
    "./images/cubemap/pisa_posz.jpg",
    "./images/cubemap/pisa_negz.jpg",
  };
  tex_id = LoadCubemap(faces);

  skybox_shader_id.LoadVertexShader("./shaders/skybox.vert");
  skybox_shader_id.LoadFragmentShader("./shaders/skybox.frag");
  skybox_shader_id.Create();

  float skybox_vertices[] = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
  };

  glGenVertexArrays(1, &skybox_VAO);
  glGenBuffers(1, &skybox_VBO);
  glBindVertexArray(skybox_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)0);
  glBindVertexArray(0);
  /* end of skybox initialization */

  /* beginning of procedural texture initialization */
  glGenTextures(1, &tex_id_2);
  glBindTexture(GL_TEXTURE_2D, tex_id_2);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PROC_TEX_WIDTH, PROC_TEX_HEIGHT,
               0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if (!procedural_shader_id.LoadComputeShader("./shaders/procedural.comp") ||
      !procedural_shader_id.Create()) {
    std::cerr << "Failed to create compute shader program" << std::endl;
    return EXIT_FAILURE;
  }

  glUseProgram(procedural_shader_id.GetProgram());
  glBindImageTexture(0, tex_id_2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glDispatchCompute((GLuint)((PROC_TEX_WIDTH + 15) / 16),
                    (GLuint)((PROC_TEX_HEIGHT + 15) / 16), 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glUseProgram(0);
  /* end of procedural texture initialization */

  /* beginning of framebuffer initialization */
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.10f, 0.12f, 0.16f, 1.0f);

  glGenFramebuffers(1, &fbo_1);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_1);

  glGenTextures(1, &color_tex_1);
  glBindTexture(GL_TEXTURE_2D, color_tex_1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color_tex_1, 0);

  glGenTextures(1, &depth_tex_1);
  glBindTexture(GL_TEXTURE_2D, depth_tex_1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, 0,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth_tex_1, 0);

  /* FBO 2 Blurring */
  glGenFramebuffers(1, &fbo_2);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_2);

  glGenTextures(1, &color_tex_2);
  glBindTexture(GL_TEXTURE_2D, color_tex_2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color_tex_2, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  /* end of framebuffer initialization */

  /* beginning of screen quad initialization */
  float quadVertices[] = {-1.0f, 1.0f,  0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                          1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f,
                          1.0f,  -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f};
  glGenVertexArrays(1, &quand_VAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quand_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  /* end of screen quad initialization */

  /* beginning of camera callback initialization */
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  //glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetScrollCallback(window, scroll_callback);
  /* end of camera callback initialization */
  
  /* beginning of IMGUI Init */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls 
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  if (!ImGui_ImplOpenGL3_Init("#version 330")) {
    std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
    return EXIT_FAILURE;
  }
  /* end of IMGUI Init */
  return true;
}

void Terminate() {
  for (Model *model : models) {
    if (model->shader.GetProgram()) {
      model->shader.Destroy();
    }
    model->DestroyBuffers();
    delete model;
  }
  models.clear();

  if (quadVBO) {
    glDeleteBuffers(1, &quadVBO);
    quadVBO = 0;
  }
  if (quand_VAO) {
    glDeleteVertexArrays(1, &quand_VAO);
    quand_VAO = 0;
  }

  if (skybox_VBO) {
    glDeleteBuffers(1, &skybox_VBO);
    skybox_VBO = 0;
  }
  if (skybox_VAO) {
    glDeleteVertexArrays(1, &skybox_VAO);
    skybox_VAO = 0;
  }

  if (fbo_1) {
    glDeleteFramebuffers(1, &fbo_1);
    fbo_1 = 0;
  }
  if (fbo_2) {
    glDeleteFramebuffers(1, &fbo_2);
    fbo_2 = 0;
  }

  if (color_tex_1) {
    glDeleteTextures(1, &color_tex_1);
    color_tex_1 = 0;
  }
  if (depth_tex_1) {
    glDeleteTextures(1, &depth_tex_1);
    depth_tex_1 = 0;
  }
  if (color_tex_2) {
    glDeleteTextures(1, &color_tex_2);
    color_tex_2 = 0;
  }
  if (tex_id) {
    glDeleteTextures(1, &tex_id);
    tex_id = 0;
  }
  if (tex_id_2) {
    glDeleteTextures(1, &tex_id_2);
    tex_id_2 = 0;
  }

  if (blur_shader_id.GetProgram()) {
    blur_shader_id.Destroy();
  }
  if (sepia_shaper_id.GetProgram()) {
    sepia_shaper_id.Destroy();
  }
  if (skybox_shader_id.GetProgram()) {
    skybox_shader_id.Destroy();
  }
  if (procedural_shader_id.GetProgram()) {
    procedural_shader_id.Destroy();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  window = nullptr;
  glfwTerminate();
}

void Display() {
  glfwPollEvents();
  Render();
  glfwSwapBuffers(window); // Swap from GLUT to GLFW
}

void Render() {
  /* beginning of Imgui */
  // (Your code calls glfwPollEvents())
  // ...
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // ImGui::ShowDemoWindow(); // Show demo window! :)
  using namespace ImGui;
  
  /* Camera control window */
  Begin("Controls");
  SliderFloat("Theta", &camera.theta, -360.f, 360.f);
  SliderFloat("Phi", &camera.phi, -89.0f, 89.0f);
  SliderFloat("Zoom", &camera.zoom, -1.0f, 45.0f * 2.f);
  End();

  /* Material control blocks*/
  int i = 1;
  for (Model* model: models) {
    std::string name = "Model " + std::to_string(i);
    Begin(name.c_str());

    if (model->materials.empty()) {
      TextUnformatted("No materials found for this model.");
    }

    if (BeginCombo("All instances", "Select material")) {
      for (const tinyobj::material_t &material : model->materials) {
        const char *materialName = material.name.empty() ? "<unnamed>" : material.name.c_str();
        if (Selectable(materialName)) {
          for (InstanceData &data : model->instance_data) {
            model->SetMaterial(&data, material);
          }
        }
      }
      EndCombo();
    }

    Separator();

    for (size_t j = 0; j < model->instance_data.size(); ++j) {
      InstanceData &child = model->instance_data[j];
      PushID(static_cast<int>(j));

      std::string instanceLabel = "Instance " + std::to_string(j + 1);
      if (BeginCombo(instanceLabel.c_str(), "Select material")) {
        for (const tinyobj::material_t &material : model->materials) {
          const char *materialName = material.name.empty() ? "<unnamed>" : material.name.c_str();
          if (Selectable(materialName)) {
            model->SetMaterial(&child, material);
          }
        }
        EndCombo();
      }

      PopID();
    }

    Separator();

    if (Button("Random material")) {
      // https://cpppatterns.com/patterns/choose-random-element.html
      for (InstanceData &data: model->instance_data) { 
        std::random_device random_device;
        std::mt19937 engine{random_device()};
        std::uniform_int_distribution<int> dist(0, model->materials.size() - 1);
        
        tinyobj::material_t random_element = model->materials[dist(engine)]; 

        model->SetMaterial(&data, random_element);
      }
    }

    End();
    i++;
  }

  /* end of Imgui */

  /* beginning of camera update */
  camera.CalculateProj();
  camera.CalculateView();
  /* end of camera update */

  /* beginning of geometry pass */
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_1);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* beginning of model rendering */
  for (Model *model : models) {
    model->Draw(camera.view, camera.proj, camera.position);
  }
  /* end of model rendering */

  /* beginning of skybox rendering */
  glDepthFunc(GL_LEQUAL);
  glUseProgram(skybox_shader_id.GetProgram());
  glm::mat4 skybox_view = glm::mat4(glm::mat3(camera.view));
  glUniformMatrix4fv(
      glGetUniformLocation(skybox_shader_id.GetProgram(), "view"), 1,
      GL_FALSE, &skybox_view[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(skybox_shader_id.GetProgram(), "proj"), 1,
      GL_FALSE, &camera.proj[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
  glUniform1i(glGetUniformLocation(skybox_shader_id.GetProgram(), "skybox"),
              0);
  glBindVertexArray(skybox_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
  /* end of skybox rendering */
  /* end of geometry pass */

  /* beginning of blur pass */
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_2);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(blur_shader_id.GetProgram());
  glUniform1f(glGetUniformLocation(blur_shader_id.GetProgram(), "offset_x"),
              1.0f / APP_WINDOW_WIDTH);
  glUniform1f(glGetUniformLocation(blur_shader_id.GetProgram(), "offset_y"),
              1.0f / APP_WINDOW_HEIGHT);

  float blur_i = 1.2f;
  glUniform1f(glGetUniformLocation(blur_shader_id.GetProgram(), "strength"),
              blur_i);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_tex_1);
  glUniform1i(glGetUniformLocation(blur_shader_id.GetProgram(), "screen_tex"),
              0);

  glBindVertexArray(quand_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  /* end of blur pass */

  /* beginning of final post-process pass */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(sepia_shaper_id.GetProgram());
  glUniform1f(glGetUniformLocation(sepia_shaper_id.GetProgram(), "intensity"),
              0.5f);
  glUniform1f(glGetUniformLocation(sepia_shaper_id.GetProgram(), "noise_mix"),
              0.20f);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_tex_2);
  glUniform1i(glGetUniformLocation(sepia_shaper_id.GetProgram(), "screen_tex"),
              0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, tex_id_2);
  glUniform1i(glGetUniformLocation(sepia_shaper_id.GetProgram(), "noise_tex"),
              1);
  glBindVertexArray(quand_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  /* end of final post-process pass */

  // Rendering
  // (Your code clears your framebuffer, renders your other stuff etc.)
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // (Your code calls glfwSwapBuffers() etc.)
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (camera.initial) {
    camera.Init((float)xpos, (float)ypos);
    camera.initial = false;
    return;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    camera.Position((float)xpos, (float)ypos);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  camera.Zoom((float)yoffset);
}

GLuint LoadCubemap(const vector<std::string> &faces)
{
  /* beginning of cubemap loading */
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
      GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /* end of cubemap loading */
    return textureID;
}  

int main(int argc, char *argv[]) {
  if (!Initialize()) {
    return EXIT_FAILURE;
  }

  while (!glfwWindowShouldClose(window)) {
    Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  Terminate();

  return EXIT_SUCCESS;
}