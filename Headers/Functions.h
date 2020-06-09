#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bitset>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <RtAudio/RtAudio.h>

#include "Enums/Direction.h"
#include "variables.h"
#include "Gold.h"
#include "Audio.h"


void FindScreenShotCount();
void FindVideoCount();
void UpdateViewPortValues(int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);
void screenCapture();
unsigned int loadTexture(char const* path);
unsigned char* getRawCharArrayWithSTBI(char const* path, int* width, int* height, int* nrComponents, int type);
void playerTextureMapping(int index);
void levelTextureMapping(int index);
void abcTextureMapping(char karakter);
void TextWriting(std::string text, float i, float j);
void DrawLevel(float x, float y, int ref, float holeTimer);
void DrawEnemy(float x, float y, int ref, Direction direction, Gold* gold);
void processInput(GLFWwindow* window);
void errorCallback(int error, const char* description);
void loadConfig();
int RtAudioVorbis(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
void FullscreenSwitch();

#ifdef VIDEO_RECORDING
void InitializeGameVideo();
void WriteVideoFrame();
void CloseGameVideo();
void RecordHandling();
#endif

#endif // !FUNCTIONS_H