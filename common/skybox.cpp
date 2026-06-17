#include <GL/glew.h>
#include "skybox.h"
#include <GL/gl.h>
#include "stb_image.h"
#include <GL/glext.h>
#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <iostream>

float skyboxVertices[] = {
    // Coming from https://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox_data
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox(void)
    : texture_id(0), VAO(0), VBO(0), camera(nullptr) {}

Skybox::~Skybox(void) {}

void Skybox::SetCamera(Camera* cam) {
    camera = cam;
}

void Skybox::AddCubemap(const std::string& name, const std::vector<std::string>& cube_faces) {
    faces[name] = cube_faces;
    if (selected.empty()) {
        selected = name;
    }
}

unsigned int Skybox::LoadCubemap(const std::string &face)
{
    std::vector<std::string>& faces = this->faces[face];
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    //  stbi_set_flip_vertically_on_load(false);
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

    return textureID;
}

void Skybox::LoadShader(const std::string& shader_path, const std::string& shader_name) {
    const std::string vertex_shader = shader_path + shader_name + ".vert";
    const std::string fragment_shader = shader_path + shader_name + ".frag";

    shader_program.LoadVertexShader(vertex_shader.c_str());
    shader_program.LoadFragmentShader(fragment_shader.c_str());

    if (!shader_program.Create()) {
        exit(EXIT_FAILURE);
    }
}

void Skybox::Init(const std::string& shader_path, const std::string& shader_name) {
    if (selected.empty() && !faces.empty()) {
        selected = faces.begin()->first;
    }

    if (selected.empty()) {
        return;
    }

    texture_id = LoadCubemap(selected); // Load cubemap into texture object

    LoadShader(shader_path, shader_name);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    
    glBindVertexArray(0);
}

void Skybox::ChangeCubeMap(const std::string& face) {
    if (faces.find(face) == faces.end()) {
        return;
    }

    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    
    selected = face;
    texture_id = LoadCubemap(selected);
}

void Skybox::Draw() {
    if (!camera || !texture_id || !VAO) {
        return;
    }

    glDepthFunc(GL_LEQUAL);
    GLuint program = shader_program.GetProgram();

    if (!program) {
        glDepthFunc(GL_LESS);
        return;
    }
    
    glUseProgram(program);

    glm::mat4 view = glm::mat4(glm::mat3(camera->view));

    auto viewLoc = glGetUniformLocation(program, "view");
    auto projLoc = glGetUniformLocation(program, "proj");
    auto skyboxLoc = glGetUniformLocation(program, "skybox");

    if (viewLoc < 0 || projLoc < 0 || skyboxLoc < 0) {
        glDepthFunc(GL_LESS);
        return;
    }

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &camera->proj[0][0]);
    
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glUniform1i(skyboxLoc, 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(sizeof(skyboxVertices) / (3 * sizeof(float))));

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::GenerateBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}
void Skybox::DestroyBuffers() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
}

void Skybox::DestroyShader() {
   if (shader_program.GetProgram()) {
    shader_program.Destroy();
   }
}

std::vector<std::string> Skybox::GetKeyCubeMap() {
    std::vector<std::string> keys;
    
    for (std::map<std::string, std::vector<std::string>>::iterator it = faces.begin(); it != faces.end(); it++) {
        keys.push_back(it->first);
    }

    return keys;
}
