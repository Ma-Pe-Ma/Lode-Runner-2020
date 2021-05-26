//
// Created by ASUS on 2020. 06. 18..
//
#include "LodeRunnerRenderer.h"
#include "android_fopen.h"
#include "NDKHelper.h"
#include "IOHandler.h"
#include "States/State.h"
#include "States/StateContext.h"

void LodeRunnerRenderer::Initialize() {
    std::string mainMenuTextureName = "Texture/MainMenu.png";
    if (championship) {
        mainMenuTextureName = "Texture/Championship.png";
    }
    else if (usCover) {
        mainMenuTextureName = "Texture/MainMenuU.png";
    }

    GLHelper::Initialize(mainMenuTextureName);

    android_fopen_set_asset_manager(ndk_helper::JNIHelper::GetInstance()->GetAssetManager());
    Audio::OpenAudioFiles(Audio::soundNames);
    Audio::InitializeAudioStream(new AudioCallback());

#ifdef VIDEO_RECORDING
    AudioParameters* audioIn = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);
	AudioParameters* audioOut = new AudioParameters(44100, AV_CODEC_ID_AC3, 327680, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16);

	VideoParameters* videoIn = new VideoParameters(GLHelper::viewPortWidth, GLHelper::viewPortHeight, AV_CODEC_ID_NONE, 400000, AV_PIX_FMT_RGB24, STREAM_FRAME_RATE);
	VideoParameters* videoOut = new VideoParameters(0, recordingHeight, AV_CODEC_ID_H264, 400000, AV_PIX_FMT_YUV420P, STREAM_FRAME_RATE);

	MultiMedia media(audioIn, audioOut, videoIn, videoOut);
	media.setGLViewPortReferences(&GLHelper::viewPortX, &GLHelper::viewPortY, &GLHelper::viewPortWidth, &GLHelper::viewPortHeight);
	media.setGenerateName(GLHelper::generateNewVideoName);
	media.setVideoOutputSizeWanted(0, recordingHeight);
	Audio::multiMedia = &media;
	GLHelper::multiMedia = &media;
#endif
	if (!initialized) {
        initialized = true;
        offset = 0;

        State::initialize(new StateContext());
        GameState::initialize(State::stateContext->gamePlay);
	}

    start = std::chrono::system_clock::now();
    prevFrameStart = std::chrono::system_clock::now();
}

void LodeRunnerRenderer::ResumeWindow() {
    Audio::OnWindowResume();
}

void LodeRunnerRenderer::CloseWindow() {
    std::chrono::duration<double, std::milli> sessionTime = (std::chrono::system_clock::now() - start);
    offset += sessionTime.count() / 1000;

    Audio::OnWindowClose();
}

void LodeRunnerRenderer::Terminate() {
    GLHelper::Terminate();
    Audio::CloseAudioFiles();
}

void LodeRunnerRenderer::PauseGame() {

}

void LodeRunnerRenderer::Render(float currentFrameX) {
    prevFrameStart = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> work_time = prevFrameStart - start;
    GameTime::calculateTimeValues(work_time.count() / 1000 + offset);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    State::stateContext->update(GameTime::getCurrentFrame());

    //take a screenshot
    //if (pButton.simple()) {
        //GLHelper::screenCapture();
    //}

#ifdef VIDEO_RECORDING
    //With the help of this function you can record videos
    media.recordAndControl(REC.simple());
#endif
}