//
// Created by ASUS on 2020. 06. 14..
//

#ifndef LODERUNNER_ENGINE_H
#define LODERUNNER_ENGINE_H

#include "NDKHelper.h"
#include "Helper.h"

#include "LodeRunnerRenderer.h"

class Engine {
    LodeRunnerRenderer LRrenderer;

    ndk_helper::GLContext* gl_context_;

    bool initialized_resources_;
    bool has_focus_;

    ndk_helper::DoubletapDetector doubletap_detector_;
    ndk_helper::TapDetector tap_detector_;
    ndk_helper::PinchDetector pinch_detector_;
    ndk_helper::DragDetector drag_detector_;
    ndk_helper::PerfMonitor monitor_;

    ndk_helper::TapCamera tap_camera_;

    android_app* app_;

    ASensorManager* sensor_manager_;
    const ASensor* accelerometer_sensor_;
    ASensorEventQueue* sensor_event_queue_;

    void UpdateFPS(float fps);
    void ShowUI();
    void TransformPosition(ndk_helper::Vec2& vec);

public:
    static void HandleCmd(struct android_app* app, int32_t cmd);
    static int32_t HandleInput(android_app* app, AInputEvent* event);

    Engine();
    ~Engine();
    void SetState(android_app* app);
    int InitDisplay(android_app* app);
    void LoadResources();
    void UnloadResources();
    void DrawFrame();
    void TermDisplay();
    void TrimMemory();
    bool IsReady();
    void CloseWindow();
    void ResumeWindow();

    void InitSensors();
    void ProcessSensors(int32_t id);
    void SuspendSensors();
    void ResumeSensors();
};


#endif //LODERUNNER_ENGINE_H
