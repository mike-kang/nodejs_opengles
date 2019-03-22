#include <stdio.h>
#include <unistd.h>
#include "../common/startScreen.h"
#include "../common/LoadShaders.h"
#include "../common/texture.h"
#include <chrono>
#include "Server.h"

void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        printf("after %s() glError (0x%x)\n", op, error);
    }
}

void calculate_phi_mask(int p1_1, int p1_2, int p2, int p3_1, int p3_2, unsigned char phi_dark[], unsigned char phi_bright[])
{
    float a = p1_1;
    float b = p1_2;
    float c = p2;
    float d = p3_1;
    float e = p3_2;
    unsigned char bright_1_p, bright_2_p, dark_p;
    for(int i=0; i<256; i++){
        bright_1_p = 255. * b * a /(255.*(b-c) - a*c);
        bright_2_p = 255.*((255.-a)*d + a*b)/((255.-a)*(d-e) + 255.*b);
        dark_p = 255. * d / (255. + d - e);
        // bright transfer function
        if(i < bright_1_p)
            phi_bright[i] = c/255*i;
        else if(i < bright_2_p)
            phi_bright[i] = b/(255-a)*(i - a);
        else 
            phi_bright[i] = (e-d)/255*i + d;    
        
        // dark transfer function
        if(i < dark_p)
            phi_dark[i] = i;
        else 
            phi_dark[i] = (e-d)/255*i + d;  
    }
    printf("%d %d %d\n", bright_1_p, dark_p, bright_2_p);
}

float g_alpha = 2.0;
void onMessage(char* buf, int length)
{
    //printf("*** %s\n",buf);
    g_alpha = atof(buf);
}

int main(int argc, const char **argv)
{
    std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
    CServer server;
    server.setOnMessage(onMessage);
    server.start();

    InitGraphics();
    printf("Screen started\n");
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "simplevertshader.glsl", "texturefragshader.glsl" );
    printf("Shaders loaded\n");

    // Load the texture using any two methods
    //GLuint Texture = loadBMP_custom("uvtemplate.bmp", 0);
    const char *filename = "uvtemplate_1920_1080_NV12.yuv";
    if(argc > 1 && strstr(argv[1], ".yuv")){
        filename = argv[1];
    }
    if(!loadYUV_NV12_custom(filename, 0)){
        exit(1);
    }
    //GLuint Texture =  loadDDS("uvtemplate.DDS"); //loadTGA_glfw("uvtemplate.tga"); 
    
    //phi_dark, phi_bright parameters
    int para1_1 = 32;
    int para1_2 = 224;
    int para2 = 32;
    int para3_1 = 64;
    int para3_2 = 200;
    unsigned char phi_dark[256];
    unsigned char phi_bright[256];
    
    GLuint TextureID_y_location  = glGetUniformLocation(programID, "tex_y");
    GLuint TextureID_uv_location  = glGetUniformLocation(programID, "tex_uv");
    GLuint bright_phi_location = glGetUniformLocation(programID, "bright_phi");
    GLuint dark_phi_location = glGetUniformLocation(programID, "dark_phi");
    GLuint alpha_location = glGetUniformLocation (programID, "alpha");
    calculate_phi_mask(para1_1,para1_2,para2,para3_1,para3_2, phi_dark, phi_bright);
    for(int i=0; i<256; i++){
        //phi_dark[i] = i;
        //printf("[%d]=%d\n", i, phi_dark[i]);
    }
    GLuint phiTextureID[2];
    glGenTextures(2, phiTextureID);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, phiTextureID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 256, 1, 0, GL_ALPHA, GL_UNSIGNED_BYTE, phi_bright);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glUniform1i(bright_phi_location, 2);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, phiTextureID[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 256, 1, 0, GL_ALPHA, GL_UNSIGNED_BYTE, phi_dark);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glUniform1i(dark_phi_location, 3);
    
    // Get a handle for our "myTextureSampler" uniform

    GLfloat g_vertex_buffer_data[] = { 
                -1.0f,  1.0f, 0.0f, 0.0, 0.0, 
                -1.0f,  -1.0f, 0.0f, 0.0, 1.0, 
                1.0f,  -1.0f, 0.0f, 1.0, 1.0,
                -1.0f,  1.0f, 0.0f, 0.0, 0.0, 
                1.0f,  -1.0f, 0.0f, 1.0, 1.0,
                1.0f,  1.0f, 0.0f, 1.0, 0.0
    };

    GLuint m_vertexLoc = glGetAttribLocation(programID, "vertex");
    printf("m_vertexLoc %d\n", m_vertexLoc);
    GLuint m_TexCoordLoc = glGetAttribLocation(programID, "a_texCoord");
    printf("m_TexCoordLoc %d\n", m_TexCoordLoc);
    checkGlError("glGetAttribLocation");

    // Use our shader
    glUseProgram(programID);

    // Bind our texture in Texture Unit 0
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID_y_location, 0);
    glUniform1i(TextureID_uv_location, 1);
    glVertexAttribPointer(
            m_vertexLoc, //vertexPosition_modelspaceID, // The attribute we want to configure
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            5 * sizeof(float),                  // stride
            g_vertex_buffer_data // (void*)0            // array buffer offset
    );

    glVertexAttribPointer(
            m_TexCoordLoc, //vertexPosition_modelspaceID, // The attribute we want to configure
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            5 * sizeof(float),                  // stride
            g_vertex_buffer_data + 3 // (void*)0            // array buffer offset
    );
    // see above glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glEnableVertexAttribArray ( m_vertexLoc );
    glEnableVertexAttribArray ( m_TexCoordLoc );
    checkGlError("glVertexAttribPointer");

    do{
        std::chrono::steady_clock::time_point curr_time= std::chrono::steady_clock::now();
        std::chrono::duration<double> sec = curr_time - pre_time;
        pre_time = curr_time;
        //printf("time %f\n", sec);
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );
        glUniform1f(alpha_location, g_alpha);     
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

        updateScreen();
    }
    while(1);

    glDeleteProgram(programID);
    //glDeleteTextures(1, &TextureID_y_location);
    //glDeleteTextures(1, &TextureID_uv_location);
}

