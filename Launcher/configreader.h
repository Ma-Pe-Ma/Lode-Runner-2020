#ifndef CONFIGREADER_H
#define CONFIGREADER_H

namespace ConfigReader
{
    void read();
    bool write();

    extern int resolutionMode;
    extern bool levelset;
    extern bool usCover;

    extern float enemySpeed;
    extern float playerSpeed;

    extern int width;
    extern int height;

    extern int startingLevel;
    extern int fps;
    extern int recordingHeight;
}

#endif // CONFIGREADER_H
