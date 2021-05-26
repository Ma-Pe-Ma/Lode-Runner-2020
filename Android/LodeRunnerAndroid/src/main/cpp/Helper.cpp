//
// Created by ASUS on 2020. 06. 15..
//

#include "Helper.h"

void Helper::log(std::string text) {
    __android_log_print(ANDROID_LOG_INFO, "CPP", "%s", text.c_str());
}