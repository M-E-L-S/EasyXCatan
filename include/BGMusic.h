//BGMusic.h
#pragma once
#include <string>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

enum class MusicType {
    NONE,
    PANEL,
    BANK,
    HARBOR,
    DEV_CARD,
    MANBA
};

class MusicManager {
private:
    MusicType current = MusicType::NONE;

    std::string getMusicPath(MusicType type) {
        switch (type) {
            case MusicType::PANEL: return "./assets/music/panel.mp3";//
            case MusicType::BANK:     return "./assets/music/bank.mp3";//
            case MusicType::HARBOR:       return "./assets/music/harbor.mp3";
            case MusicType::DEV_CARD:       return "./assets/music/devcard.wav";//
            case MusicType::MANBA:       return "./assets/music/manba.mp3";
            default: return "";
        }
    }

public:
    void play(MusicType type) {
        if (type == current) return;
        current = type;

        std::string path = getMusicPath(type);
        if (path.empty()) {
            stop();
            return;
        }

        // 无论使用哪种API，都先停止之前的播放
        stop();

        // 检查文件扩展名
        if (path.length() >= 4 && path.substr(path.length() - 4) == ".mp3") {
            // 使用 mciSendString 播放 MP3

            // 1. 关闭任何之前的 mci 设备
            mciSendString("close MusicBGM", NULL, 0, NULL);

            // 2. 打开 MP3 文件
            std::string open_command = "open \"" + path + "\" type mpegvideo alias MusicBGM";
            mciSendString(open_command.c_str(), NULL, 0, NULL);

            // 3. 播放并循环
            mciSendString("play MusicBGM repeat", NULL, 0, NULL);

        }
        else {
            // 使用 PlaySound 播放 WAV
            PlaySound(path.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        }
    }

    void stop() {
        // 停止 PlaySound (WAV)
        PlaySound(NULL, NULL, 0);

        // 停止 mciSendString (MP3)
        mciSendString("stop MusicBGM", NULL, 0, NULL);
        mciSendString("close MusicBGM", NULL, 0, NULL);

        current = MusicType::NONE;
    }
};
