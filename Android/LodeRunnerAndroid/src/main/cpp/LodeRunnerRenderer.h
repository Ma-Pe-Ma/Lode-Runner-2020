//
// Created by ASUS on 2020. 06. 18..
//

#ifndef LODERUNNER_LODERUNNERRENDERER_H
#define LODERUNNER_LODERUNNERRENDERER_H

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <string>
#include <chrono>

#include "GLHelper.h"

#include <oboe/Oboe.h>
#include "AudioCallback.h"

class LodeRunnerRenderer {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point prevFrameStart;

    bool initialized = false;
    double offset = 0;

    //int counter = 0;
public:
    void Initialize();
    void Terminate();
    void Render(float);
    void CloseWindow();
    void ResumeWindow();
    void PauseGame();
};


#endif //LODERUNNER_LODERUNNERRENDERER_H
