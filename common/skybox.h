#pragma once
#include "GLShader.h"
#include "camera.h"
#include <GL/gl.h>
#include <map>
#include <string>
#include <vector>

#ifndef SKYBOX_H
#define SKYBOX_H

class Skybox {
    public:
        Skybox(void);
        ~Skybox(void);

        void SetCamera(Camera* camera);
        void AddCubemap(const std::string& name, const std::vector<std::string>& cube_faces);
        
        void Draw();
        void Init(const std::string& shader_path, const std::string& shader_name); // Loads cubemap, shders..

        void LoadShader(const std::string& shader_path, const std::string& shader_name); 
        
        unsigned int LoadCubemap(const std::string& face); // taken from https://learnopengl.com/Advanced-OpenGL/Cubemaps
        void ChangeCubeMap(const std::string& face);
        
        void GenerateBuffers();
        void DestroyBuffers();
        void DestroyShader();

        std::vector<std::string> GetKeyCubeMap();
    private:
        GLuint texture_id; // Texture buffer ID
        GLShader shader_program; // Shader program loaded at creation
        
        GLuint VAO;
        GLuint VBO;

        std::map<std::string, std::vector<std::string>> faces; // a map matching the "name" of the cubemap to its vector of files
        std::string selected;

        Camera* camera;
};

#endif