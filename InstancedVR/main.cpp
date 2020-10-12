/*
Title: Normal Maps
File Name: main.cpp
Copyright � 2016, 2019
Author: David Erbelding, Niko Procopi
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "FreeImage.h"
#include <vector>
#include "mesh.h"
#include "fpsController.h"
#include "transform3d.h"
#include "material.h"
#include "texture.h"
#include <iostream>


// This is the resolution of iPhone 6, which I am personally using
glm::vec2 viewportDimensions = glm::vec2(1366, 768);
glm::vec2 mousePosition = glm::vec2();

// The texture we will be rendering to. It will match the dimensions of the screen.
GLuint screenTexture;

// The render buffer used to store depth and stencil information.
GLuint renderBuffer;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    viewportDimensions = glm::vec2(width, height);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}

bool IsExtensionSupported(const char* name)
{
    GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (GLint i = 0; i < n; i++)
    {
        const char* extension =
            (const char*)glGetStringi(GL_EXTENSIONS, i);
        if (!strcmp(name, extension))
        {
            return true;
        }
    }
    return false;
}

#define NikoIphone6 true

int main(int argc, char **argv)
{
	// Initialize GLFW
	glfwInit();

	// Initialize window
    GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "Virtual Reality",

    // If you want fullscreen, change this 
    // to glfwGetPrimaryMonitor()
    //glfwGetPrimaryMonitor(),
    nullptr, 
    
    nullptr);
    
    glfwMakeContextCurrent(window);

	// Set window callbacks
	glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // hide mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Initialize glew
	glewInit();

    printf("\Extension supported?: %d\n\n",IsExtensionSupported("GL_ARB_fragment_layer_viewport"));

    // The mesh loading code has changed slightly, we now have to do some extra math to take advantage of our normal maps.
    // Here we pass in true to calculate tangents.
    Mesh* model = new Mesh("../Assets/plane.obj", true);
    Mesh* car = new Mesh("../Assets/car.3Dobj", true);
    Mesh* dog = new Mesh("../Assets/dog.3Dobj", true);
    Mesh* kitten = new Mesh("../Assets/kitten.3Dobj", true);
    Mesh* crate = new Mesh("../Assets/cube.3Dobj", true);
    Mesh* helix = new Mesh("../Assets/helix.3Dobj", true);
    Mesh* torus = new Mesh("../Assets/torus.3Dobj", true);
    Mesh* wheel = new Mesh("../Assets/wheel.3Dobj", true);
    Mesh* bear = new Mesh("../Assets/bear5.obj", true);

    // The transform being used to draw our second shape.
    Transform3D transform;

    // Make a first person controller for the camera.
    FPSController controller = FPSController();

	// Create Shaders
    Shader* vertexShader1 = new Shader("../Assets/vertex.glsl", GL_VERTEX_SHADER);
    Shader* fragmentShader1 = new Shader("../Assets/fragment.glsl", GL_FRAGMENT_SHADER);
    Shader* vertexShader2 = new Shader("../Assets/2ndVS.glsl", GL_VERTEX_SHADER);
    Shader* fragmentShader2 = new Shader("../Assets/2ndFS.glsl", GL_FRAGMENT_SHADER);

    // Create A Shader Program
    // The class wraps all of the functionality of a gl shader program.
    ShaderProgram* shaderProgram1 = new ShaderProgram();
    shaderProgram1->AttachShader(vertexShader1);
    shaderProgram1->AttachShader(fragmentShader1);

    ShaderProgram* shaderProgram2 = new ShaderProgram();
    shaderProgram2->AttachShader(vertexShader2);
    shaderProgram2->AttachShader(fragmentShader2);

	// fields that are used in the shader, on the graphics card
	char cameraView1VS[] = "cameraView1";
    char cameraView2VS[] = "cameraView2";
	char worldMatrixVS[] = "worldMatrix";

	char colorTexFS[] = "tex";
	char normalTexFS[] = "tex2";

	// files that we want to open
	char colorTexFile[] = "../Assets/BrickColor.png";
	char normalTexFile[] = "../Assets/BrickNormal.png";

    char blankNorm[] = "../Assets/blankNormal.PNG";
    char colCar[] = "../Assets/car.png";

    char colKitten[] = "../Assets/kitten.png";
    char colDog[] = "../Assets/Dog.png";
    char colCrate[] = "../Assets/Crate.png";
    char colRusty[] = "../Assets/rusty.jpg";

    // Create a material using a texture for our model
    Material* material1 = new Material(shaderProgram1);
    Material* material2 = new Material(shaderProgram2);

    Texture* colPlaneTex = new Texture(colorTexFile);
    Texture* normPlaneTex = new Texture(normalTexFile);

    Texture* blankNormTex = new Texture(blankNorm);
    Texture* colCarTex = new Texture(colCar);

    Texture* kittenTex = new Texture(colKitten);
    Texture* dogTex = new Texture(colDog);
    Texture* crateTex = new Texture(colCrate);
    Texture* rustyTex = new Texture(colRusty);

    glm::mat4 view;
    glm::mat4 viewProjection;

    // Print instructions to the console.
    std::cout << "Use WASD to move, and the mouse to look around." << std::endl;
    std::cout << "Press escape or alt-f4 to exit." << std::endl;

    float rotY = 0;
    float moveX = -0.3f;
    Transform3D temp;

    // In order to render to a texture, we need a frameBuffer.
    // A framebuffer is a buffer that the gpu renders to. By default, OpenGL renders to the backbuffer.

    // Create and bind the framebuffer. This is done exactly the same as it's done for everything else in OpenGL.
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

    // The framebuffer actually consists of a few smaller objects.
    // These are primarily textures and renderbuffers, each which have different uses.
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    // Here we're going to create a texture that matches the size of our viewport. (this also gets resized in the viewport resizing code)
    // Instead of passing in data for the texture, we pass in null, which leaves the texture empty.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportDimensions.x, viewportDimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // clamp to border, do not allow texture wrapping
    // otherwise, pixels that go off the top of the screen will wrap to the bottom
    // and pixels that go too far right will show up on the left
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // When we read from the texture, it will be 1 to 1 with the screen, so we shouldnt have any filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // We also need to create a render buffer.
    // A render buffer is similar to a texture, but with less features.
    // When rendering to a texture, we can later use that texture in another draw call, and draw it onto a surface.
    // A render buffer can't do that. The pixel data is temporary, and gets thrown away by the gpu when it's done.

    // We're going to create one here for the depth stencil buffer.
    // This isn't actually required for this example, because we only render one object, but if we were rendering with depth, this would be very important.
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportDimensions.x, viewportDimensions.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Finally, we attach both the texture and render buffer to our frame buffer.
    // The texture is attached as a "color attachment". This is where our fragment shader outputs.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    // The render buffer is attached to the depth stencil attachment.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    // Here we prepare to put three timestamps,
    // one before the frame starts, one after 
    // the first eye finishes, one when the
    // entire frame finishes
    GLuint64 startTime, startEye1, endEye1, endEye2, endBlur;
    unsigned int queryID[5];

    // generate two queries
    glGenQueries(4, queryID);

    bool useBlur = true;

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Exit when escape is pressed.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            moveX += 0.01f;

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            moveX -= 0.01f;

        if (moveX > 0)
            moveX = 0;

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            useBlur = false;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            useBlur = true;

        // dont need to print for now
#if 0
        printf("%f\n", rotY);
#endif

        // Determine if you want to benchmark, by pressing 'b'
        bool benchmarkThisFrame = false;
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            benchmarkThisFrame = true;


        // Update the player controller
        controller.Update(window, viewportDimensions, mousePosition, dt);
        
        // aspect ratio is 45% of window width/height, 
        // since the eyes have different resolution than window

        // Projection matrix.
        glm::mat4 projection = glm::perspective(0.9f, 
            
#if NikoIphone6
            0.45f * viewportDimensions.x / viewportDimensions.y, 
#else
            0.5f * viewportDimensions.x / viewportDimensions.y,
#endif
            .1f, 100.f);

        if (benchmarkThisFrame)
        {
            // This is added to the GPU command buffer, and 
            // will take effect when the command buffer executes.
            // First stamp is at the beginning of the frame
            glQueryCounter(queryID[0], GL_TIMESTAMP);
        }

        if (useBlur)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        }

        // Clear the depth buffers
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (benchmarkThisFrame)
        {
            // This is added to the GPU command buffer, and 
            // will take effect when the command buffer executes.
            // First stamp is at the beginning of the frame
            glQueryCounter(queryID[1], GL_TIMESTAMP);
        }

        int screenW = viewportDimensions.x;


#if NikoIphone6
        int sizeX = 45 * screenW / 100; // 45%
        int v2startX = 55 * screenW / 100; // start at 55%
#else
        int sizeX = screenW / 2; // 50%
        int v2startX = screenW / 2; // 50%
#endif

        glViewportIndexedf(0,   0,        0, sizeX, viewportDimensions.y);
        glViewportIndexedf(1, v2startX,   0, sizeX, viewportDimensions.y);

		// camera
        view = controller.GetTransform().GetInverseMatrix();
        viewProjection = projection * view;
        material1->SetMatrix(cameraView1VS, viewProjection);

        // camera
        temp = controller.GetTransform();
        temp.RotateY(rotY);
        view = temp.GetInverseMatrix();
        view = glm::translate(view, glm::vec3(moveX, 0, 0));
        viewProjection = projection * view;
        material1->SetMatrix(cameraView2VS, viewProjection);
		
        // plane
        transform.SetPosition(glm::vec3(0, 0, -10));
        transform.SetRotation(glm::vec3(0, 0, 0));
        transform.SetScale(10.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, colPlaneTex);
        material1->SetTexture(normalTexFS, normPlaneTex);
        material1->Bind();
        model->Draw();

        // car
        transform.SetPosition(glm::vec3(-2.5, -1, -15));
        transform.SetRotation(glm::vec3(0, 0.75, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, colCarTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        car->Draw();

        // car (again)
        transform.SetPosition(glm::vec3(2.5, -1, -15));
        transform.SetRotation(glm::vec3(0, 2, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, colCarTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        car->Draw();

        // cone
        transform.SetPosition(glm::vec3(2.5, -0.5, -10));
        transform.SetRotation(glm::vec3(3.14/2, 0, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, blankNormTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        helix->Draw();

        // cube
        transform.SetPosition(glm::vec3(-4, -0.5, -10));
        transform.SetRotation(glm::vec3(0, 1, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, crateTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        crate->Draw();

        // cube
        transform.SetPosition(glm::vec3(6, -0.5, -10));
        transform.SetRotation(glm::vec3(1, 0, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, crateTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        crate->Draw();

        // cube
        transform.SetPosition(glm::vec3(7.5, -0.5, -10));
        transform.SetRotation(glm::vec3(2, 0, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, crateTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        crate->Draw();

        // dog
        transform.SetPosition(glm::vec3(0, -1, -12));
        transform.SetRotation(glm::vec3(0, 3.5/2, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, dogTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        dog->Draw();

        // kitten
        transform.SetPosition(glm::vec3(-1, -1, -12));
        transform.SetRotation(glm::vec3(0, 3.5 / 2, 0));
        transform.SetScale(1.0f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, blankNormTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        kitten->Draw();

        // bear
        transform.SetPosition(glm::vec3(-1, 0.2, -8));
        transform.SetRotation(glm::vec3(0, -1.2, 0));
        transform.SetScale(0.3f);
        material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material1->SetTexture(colorTexFS, blankNormTex);
        material1->SetTexture(normalTexFS, blankNormTex);
        material1->Bind();
        bear->Draw();

        for (int i = 0; i < 10; i++)
        {
            // torus
            transform.SetPosition(glm::vec3(i*2 - 10, 0, -20));
            transform.SetRotation(glm::vec3(i, i*2, i*3));
            transform.SetScale(1.0f);
            material1->SetMatrix(worldMatrixVS, transform.GetMatrix());
            material1->SetTexture(colorTexFS, rustyTex);
            material1->SetTexture(normalTexFS, blankNormTex);
            material1->Bind();
            torus->Draw();
        }

        if (benchmarkThisFrame)
        {
            // Set another timestamp after the first eye finishes
            // rendering, and before the second eye starts
            glQueryCounter(queryID[2], GL_TIMESTAMP);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (useBlur)
        {
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0, 0.0, 0.0, 0.0);

            glViewport(0, 0, viewportDimensions.x, viewportDimensions.y);
            shaderProgram2->Bind();

            int loc = glGetUniformLocation(shaderProgram2->GetGLShaderProgram(), "tex");
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, screenTexture);
            glUniform1i(loc, 0);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            if (benchmarkThisFrame)
            {
                // time blur finishes
                glQueryCounter(queryID[3], GL_TIMESTAMP);
            }
        }


        if (benchmarkThisFrame)
        {
            // assume the stamps are not immediately available
            GLint stopTimerAvailable = 0;

            // keep looping while they aren't available
            while (!stopTimerAvailable)
            {
                // Keep checking the status of the last time stamp, to see if the GPU
                // recorded the stamp, and sent it back to the CPU
                glGetQueryObjectiv(queryID[2], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
            }

            // get query results
            glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
            glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &startEye1);
            glGetQueryObjectui64v(queryID[2], GL_QUERY_RESULT, &endEye1);
            glGetQueryObjectui64v(queryID[3], GL_QUERY_RESULT, &endBlur);

            system("cls");
            printf("Both eyes: %f ms\n", (endEye1 - startEye1) / 1000000.0);
            if (!useBlur)
            {
                printf("Full Frame: %f ms\n", (endEye1 - startTime) / 1000000.0);
            }
            else
            {
                printf("Blur eyes: %f ms\n", (endBlur - endEye1) / 1000000.0);
                printf("Full Frame: %f ms\n", (endBlur - startTime) / 1000000.0);
            }
            
        }

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();
	}

    delete model;

    // Free material should free all objects used by material
    delete material1;
    delete material2;

	// Free GLFW memory.
	glfwTerminate();

	// End of Program.
	return 0;
}
