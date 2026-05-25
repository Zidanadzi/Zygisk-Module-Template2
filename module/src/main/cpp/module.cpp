#include <sys/types.h> 
#include "zygisk.hpp"
#include <string.h>
#include <fstream>
#include <string>
#include <android/log.h>
#include <thread>
#include <chrono>
#include <unistd.h>

#define LOG_TAG "ZygiskBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// PERBAIKAN 1: Mengubah nama fungsi extern agar tidak melanggar aturan keyword 'main' C++
extern int BukaFiturUtama(int argc, char *argv[], const char* gamePkg);

void JalankanDiBackground(std::string nomor_case, std::string nama_game) {
    // Memberikan jeda waktu 35 detik agar game tenang di lobi utama sebelum eksekusi memori
    std::this_thread::sleep_for(std::chrono::seconds(35));

    char* arg_fitur = (char*)nomor_case.c_str(); 
    char* susunan_argv[] = {
        (char*)"modul_zygisk",
        arg_fitur,             
        nullptr
    };
    int jumlah_argc = 2;
    
    LOGI("Menjalankan C++ Utama untuk game: %s dengan nomor Case: %s", nama_game.c_str(), arg_fitur);
    
    // Panggil nama fungsi baru Anda yang aman
    BukaFiturUtama(jumlah_argc, susunan_argv, nama_game.c_str());
}

class PerantaraZygisk : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                // GANTI "id_modul_anda" dengan ID yang tertulis di file module.prop Anda
                std::string path_file = "/data/adb/modules/template_module/pilihan.txt";
                
                std::string nomor_case = "1"; 
                std::ifstream file_konfig(path_file);
                
                if (file_konfig.is_open()) {
                    std::string teks_bacaan;
                    std::getline(file_konfig, teks_bacaan);
                    file_konfig.close();
                    
                    if (!teks_bacaan.empty()) {
                        nomor_case = teks_bacaan;
                    }
                }

                std::string nama_game(processName);
                std::thread(JalankanDiBackground, nomor_case, nama_game).detach();
            }
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(PerantaraZygisk)
