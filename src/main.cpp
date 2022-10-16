#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "rg/Error.h"
#include "rg/coords.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool blinn = false;

glm::vec3 cameraPos = glm::vec3(-0.125, -0.8725, 1.0);


struct ProgramState {
    float ambientRed = 0.0f;
    float ambientGreen = 0.0f;
    float ambientBlue = 0.0f;
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;

    ProgramState()
            : camera(cameraPos) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << ambientRed << '\n'
        << ambientGreen << '\n'
        << ambientBlue << '\n'
        << ImGuiEnabled << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> ambientRed
           >> ambientGreen
           >> ambientBlue
           >> ImGuiEnabled;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState* programState);
void update(GLFWwindow* window);

glm::mat4 modelIgraca = glm::mat4(1.0f);

float lastX = 800.0f / 2.0f;
float lastY = 800.0f / 2.0f;
bool firstMouse = true;

float x = -0.125;
float y = -0.875;

float x_point = -0.125;
float y_point = -0.875;
unsigned square = 14;
unsigned square1 = 14;

std::vector<vector<unsigned int>> graph = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {},
                                           {}, {12, 21}, {11, 13, 22}, {12, 23}, {24}, {16, 25}, {15}, {18}, {17, 28}, {},
                                           {}, {11, 31}, {12, 32}, {13, 24, 33}, {14, 23, 25}, {15, 24}, {27, 36}, {26}, {18 ,38}, {},
                                           {}, {21}, {22, 42}, {23, 43}, {35, 44}, {34, 36}, {26, 35, 37, 46}, {36}, {28, 48}, {},
                                           {}, {42, 51}, {32, 41}, {33, 53}, {34, 45}, {44, 55}, {36, 47, 56}, {46, 48}, {38, 47}, {},
                                           {}, {41, 61}, {53}, {43, 52, 54}, {53}, {45, 65}, {46, 57, 66}, {56, 58}, {57}, {},
                                           {}, {51, 62, 71}, {61, 63}, {62, 64}, {63, 65}, {55, 64, 75}, {56, 67, 76}, {66, 68}, {67}, {},
                                           {}, {61}, {73, 82}, {72, 83}, {75}, {65, 74}, {66, 77}, {76, 78}, {77, 88}, {},
                                           {}, {82}, {72, 81}, {73, 84}, {83, 85}, {84, 86}, {85, 87}, {86, 88}, {87, 78}, {},
                                           {}, {}, {}, {}, {}, {}, {}, {}, {}, {}};

//std::vector<vector<bool>> graph = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, d{}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
//                                   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},};

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window=glfwCreateWindow(800, 800, "Maze Game", nullptr, nullptr);
    if (window==nullptr){
        std::cout << "Failed to create window!\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetKeyCallback(window, key_callback);



    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    programState = new ProgramState;
    programState->LoadFromFile("resources/programState.txt");

    if(programState->ImGuiEnabled){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader("resources/shaders/vertexShader.vs", "resources/shaders/fragmentShader.fs");
    Shader shader1("resources/shaders/vertexShader.vs", "resources/shaders/fragmentShaderBottom.fs");//vertexShaderBottom
    Shader shader2("resources/shaders/lightCubevs.vs", "resources/shaders/lightCubefs.fs");
    Shader shader3("resources/shaders/vertexShader.vs", "resources/shaders/fragmentShader.fs");
    Model ourModel("resources/objects/flashlight/Linterna.obj");
    Shader shaderModel("resources/shaders/Model.vs", "resources/shaders/Model.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader shaderBlending("resources/shaders/vertexShader.vs", "resources/shaders/blending1.fs");

    //object creation

    unsigned int VBO[5], VAO[5], EBO[5], tex[5];

    glGenVertexArrays(5, VAO);
    glGenBuffers(5, VBO);
    glGenBuffers(5, EBO);
    glGenTextures(5, tex);

    //labyrinth
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMaze), verticesMaze, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesMaze), indicesMaze, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    //textures
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    //wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load img
    int width0, height0, nChannel0;
    unsigned char *data = stbi_load("resources/textures/brickwall.jpg", &width0, &height0, &nChannel0, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width0, height0, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        ASSERT(false, "Failed to load tex!\n");
    }
    stbi_image_free(data);

    shader.use();
    shader.setInt("t0", 0);





    //floor
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBottom), verticesBottom, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBottom), indicesBottom, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, tex[1]);

    //wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_FILL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_FILL);

    //filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load img
    int width1, height1, nChannel1;
    unsigned char* data1 = stbi_load("resources/textures/marble.jpg", &width1, &height1, &nChannel1, 0);
    if(data1){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        ASSERT(false, "Failed to load tex!\n");
    }
    stbi_image_free(data1);

    shader1.use();
    shader1.setInt("t1", 0);


    //lighting

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLight), verticesLight, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLight), indicesLight, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    //player placeholder cube

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlayerCube), verticesPlayerCube, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPlayerCube), indicesPlayerCube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, tex[3]);

    //wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_FILL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_FILL);

    //filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load img
    int width3, height3, nChannel3;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data3 = stbi_load("resources/textures/brickwall.jpg", &width3, &height3, &nChannel3, 0);
    if(data3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        ASSERT(false, "Failed to load tex!\n");
    }
    stbi_image_free(data3);

    stbi_set_flip_vertically_on_load(false);
    shader3.use();
    shader3.setInt("t3", 0);

    //skybox

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/right.jpg"),
                    FileSystem::getPath("resources/textures/skybox/left.jpg"),
                    FileSystem::getPath("resources/textures/skybox/top.jpg"),
                    FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
                    FileSystem::getPath("resources/textures/skybox/front.jpg"),
                    FileSystem::getPath("resources/textures/skybox/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    //random grass texture at the end of the labyrinth for fun

    unsigned int transparentVAO, transparentVBO, transparentEBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glGenBuffers(1, &transparentEBO);

    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(transparentIndices), transparentIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    stbi_set_flip_vertically_on_load(true);
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/grass.png").c_str());
    shaderBlending.use();
    shaderBlending.setInt("tTransparent", 0);

    //render while loop
    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //starting light and model values
        glm::mat4 model      =  glm::mat4(1.0f);
        glm::mat4 modelLampa =  glm::mat4(1.0f);
        glm::mat4 view       =  programState->camera.GetViewMatrix();
        glm::mat4 projection =  glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(programState->camera.Zoom), (float)800/800, 0.1f, 100.0f);

        glm::vec3 lightColor;
        lightColor.r = 1.0;//sin(glfwGetTime()*2.0f)+1; //1.0
        lightColor.g = 1.0;//(sin(glfwGetTime()*0.7f)+1)/2; //1.0
        lightColor.b = 1.0;//(sin(glfwGetTime()*1.3f)+1)/2; //1.0

        glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);
        glm::vec3 specularColor = lightColor * glm::vec3(0.3f);

        //setting the values for the wall shader
        shader.use();

        shader.setVec3("light.position", programState->camera.Position);
        shader.setVec3("viewPos", programState->camera.Position);

        shader.setVec3("light.direction", programState->camera.Front);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        shader.setVec3("light.ambient", programState->ambientRed, programState->ambientGreen, programState->ambientBlue);
        shader.setVec3("light.diffuse", diffuseColor);
        shader.setVec3("light.specular", specularColor);


        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.035f);
        shader.setFloat("light.quadratic", 0.011f);

        shader.setFloat("material.shininess", 32.0f);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setInt("blinn", blinn);

        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glDrawElements(GL_TRIANGLES, 1020, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //setting the values for the floor shader
        shader1.use();

        shader1.setVec3("light.position", programState->camera.Position);
        shader1.setVec3("viewPos", programState->camera.Position);

        shader1.setVec3("light.direction", programState->camera.Front);
        shader1.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        shader1.setVec3("light.ambient", programState->ambientRed, programState->ambientGreen, programState->ambientBlue);
        shader1.setVec3("light.diffuse", diffuseColor);
        shader1.setVec3("light.specular", specularColor);

        shader1.setFloat("light.constant", 1.0f);
        shader1.setFloat("light.linear", 0.035f);
        shader1.setFloat("light.quadratic", 0.011f);

        shader1.setFloat("material.shininess", 32.0f);

        shader1.setMat4("model", model);
        shader1.setMat4("view", view);
        shader1.setMat4("projection", projection);

        shader1.setInt("blinn", blinn);

        glBindVertexArray(VAO[1]);
        glBindTexture(GL_TEXTURE_2D, tex[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //setting the values for the "lamp" shader
        shader2.use();

        shader2.setMat4("model", modelIgraca);
        shader2.setMat4("view", view);
        shader2.setMat4("projection", projection);
        shader2.setVec3("lightColor", lightColor);

        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //player placeholder shader
        shader3.use();

        shader3.setVec3("light.position", programState->camera.Position);
        shader3.setVec3("viewPos", programState->camera.Position);

        shader3.setVec3("light.direction", programState->camera.Front);
        shader3.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader3.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        shader3.setVec3("light.ambient", programState->ambientRed, programState->ambientGreen, programState->ambientBlue);
        shader3.setVec3("light.diffuse", diffuseColor);
        shader3.setVec3("light.specular", specularColor);

        shader3.setFloat("light.constant", 1.0f);
        shader3.setFloat("light.linear", 0.035f);
        shader3.setFloat("light.quadratic", 0.011f);

        shader3.setFloat("material.shininess", 32.0f);

        shader3.setMat4("model", modelIgraca);
        shader3.setMat4("view", view);
        shader3.setMat4("projection", projection);

        shader3.setInt("blinn", blinn);

        glBindVertexArray(VAO[3]);
        glBindTexture(GL_TEXTURE_2D, tex[3]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //transformation and drawing
        modelLampa = glm::translate(modelLampa, glm::vec3(-0.875f, 0.875f, 0.05f));
        modelLampa = glm::scale(modelLampa, glm::vec3(0.017, 0.017, 0.017));
        modelLampa = glm::rotate(modelLampa, (float)glfwGetTime(), glm::vec3(0, 0, 1.0));
        shaderModel.setMat4("model", modelLampa);
        shaderModel.setMat4("view", view);
        shaderModel.setMat4("projection", projection);

        ourModel.Draw(shaderModel);

        //crtanje discard blendovane trave oko kraja lavirinta
        shaderBlending.use();

        shaderBlending.setVec3("light.position", programState->camera.Position);
        shaderBlending.setVec3("viewPos", programState->camera.Position);

        shaderBlending.setVec3("light.direction", programState->camera.Front);
        shaderBlending.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shaderBlending.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        shaderBlending.setVec3("light.ambient", programState->ambientRed, programState->ambientGreen, programState->ambientBlue);
        shaderBlending.setVec3("light.diffuse", diffuseColor);
        shaderBlending.setVec3("light.specular", specularColor);

        shaderBlending.setFloat("light.constant", 1.0f);
        shaderBlending.setFloat("light.linear", 0.035f);
        shaderBlending.setFloat("light.quadratic", 0.011f);

        shaderBlending.setFloat("material.shininess", 32.0f);
        shaderBlending.setInt("blinn", blinn);

        shaderBlending.setMat4("model", model);
        shaderBlending.setMat4("view", view);
        shaderBlending.setMat4("projection", projection);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        //skybox
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        glm::mat4 view1 = programState->camera.GetViewMatrix();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view1)));
        skyboxShader.setMat4("projection", projection);
        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::rotate(modelSkybox, 90.0f, glm::vec3(1.0, 0.0, 0.0));
        skyboxShader.setMat4("model", modelSkybox);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        //imgui console
        if(programState->ImGuiEnabled)
            DrawImGui(programState);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/programState.txt");

    glDeleteVertexArrays(4, VAO);
    glDeleteBuffers(4, VBO);
    glDeleteBuffers(4, EBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete programState;

    glfwTerminate();

    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
void update(GLFWwindow* window) {

    if(programState->camera.Position.x < -0.77 && programState->camera.Position.x > -1.0 && programState->camera.Position.y > 0.75 && programState->camera.Position.y < 1.0){
        glfwSetWindowShouldClose(window, true);
        std::cout <<"Lampa je pronadjena! :)\n" << glfwGetTime() << "s\n";
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        programState->camera.Position = modelIgraca * glm::vec4(-0.125, -0.8725, 1.0, 1.0);
        programState->camera.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        programState->camera.Front = glm::vec3(0.0f, 0.0f, -1.0f);
        programState->camera.Zoom = 45.0f;
        programState->camera.Yaw = programState->camera.firstYaw;
        programState->camera.Pitch = 0.0f;
        programState->camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
        programState->camera.ProcessMouseMovement(0, 0);
        std::cerr << "Reset\n";
    }

    square = ((int)((x/0.25)+5)) + 10*((int)((y/0.25)+5));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(y > y_point + 0.065){
            bool check = false;
            for (int i = 0; i < graph[square].size(); i++){
                if (graph[square][i] == square + 10) {
                    check = true;
                    y_point += 0.25;
                    break;
                }
            }
            if(check){
                y = y + 0.005;
                programState->camera.Position += glm::vec3(0, 0.005, 0);
                modelIgraca = glm::translate(modelIgraca, glm::vec3(0.0, 0.005, 0.0));
            }
        }else {
            y = y + 0.005;
            programState->camera.Position += glm::vec3(0, 0.005, 0);
            modelIgraca = glm::translate(modelIgraca, glm::vec3(0.0, 0.005, 0.0));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if(x < x_point - 0.065){
            bool check = false;
            for (int i = 0; i < graph[square].size(); i++){
                if (graph[square][i] == square - 1) {
                    check = true;
                    x_point -= 0.25;
                    break;
                }
            }
            if(check) {
                x = x - 0.005;
                programState->camera.Position += glm::vec3(-0.005, 0, 0);
                modelIgraca = glm::translate(modelIgraca, glm::vec3(-0.005, 0.0, 0.0));
            }
        }else {
            x = x - 0.005;
            programState->camera.Position += glm::vec3(-0.005, 0, 0);
            modelIgraca = glm::translate(modelIgraca, glm::vec3(-0.005, 0.0, 0.0));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if(y < y_point - 0.065){
            bool check = false;
            for (int i = 0; i < graph[square].size(); i++){
                if (graph[square][i] == square - 10) {
                    check = true;
                    y_point -= 0.25;
                    break;
                }
            }
            if(check){
                y = y - 0.005;
                programState->camera.Position += glm::vec3(0, -0.005, 0);
                modelIgraca = glm::translate(modelIgraca, glm::vec3(0.0, -0.005, 0.0));
            }
        }else {
            y = y - 0.005;
            programState->camera.Position += glm::vec3(0, -0.005, 0);
            modelIgraca = glm::translate(modelIgraca, glm::vec3(0.0, -0.005, 0.0));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if(x > x_point + 0.07){
            bool check = false;
            for (int i = 0; i < graph[square].size(); i++){
                if (graph[square][i] == square + 1) {
                    check = true;
                    x_point += 0.25;
                    break;
                }
            }
            if(check){
                x = x + 0.005;
                programState->camera.Position += glm::vec3(0.005, 0, 0);
                modelIgraca = glm::translate(modelIgraca, glm::vec3(0.005, 0.0, 0.0));
            }

        }else {
            x = x + 0.005;
            programState->camera.Position += glm::vec3(0.005, 0, 0);
            modelIgraca = glm::translate(modelIgraca, glm::vec3(0.005, 0.0, 0.0));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        blinn = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        blinn = true;
    if(programState->ImGuiEnabled == false)
        programState->CameraMouseMovementUpdateEnabled=false;
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if(programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

    programState->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void DrawImGui(ProgramState* programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {

            ImGui::Begin("Ambientalno osvetljenje:");
            ImGui::Text("RGB slajderi za ambijentalno osvetljenje");
            ImGui::DragFloat("Slider za R: ", (float*)&programState->ambientRed, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Slider za G: ", (float*)&programState->ambientGreen, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Slider za B: ", (float*)&programState->ambientBlue, 0.05f, 0.0f, 1.0f);
            ImGui::End();
    }
    {
        ImGui::Begin("Camera info:");
        ImGui::Text("Camera position: (%f, %f, %f)", programState->camera.Position.x, programState->camera.Position.y, programState->camera.Position.z);
        ImGui::Text("Camera pitch: %f", programState->camera.Pitch);
        ImGui::Text("Camera yaw: %f", programState->camera.Yaw);
        ImGui::Text("Camera front: (%f, %f, %f)", programState->camera.Front.x, programState->camera.Front.y, programState->camera.Front.z);
        ImGui::Checkbox("Camera movement enabled:", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(key == GLFW_KEY_P && action == GLFW_PRESS) {
        std::cout << "Phong\n";
    }
    if(key == GLFW_KEY_B && action == GLFW_PRESS) {
        std::cout << "Blinn\n";
    }
    if(key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if(programState->ImGuiEnabled){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data7 = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data7)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
            stbi_image_free(data7);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data7);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}