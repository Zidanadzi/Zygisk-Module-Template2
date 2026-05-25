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

// Deklarasikan fungsi main dengan parameter tambahan untuk mengirim nama paket game
extern int main(int argc, char *argv[], const char* gamePkg);

void JalankanDiBackground(std::string nomor_case, std::string nama_game) {
    // Memberikan jeda waktu 35 detik agar game benar-benar selesai melewati logo awal 
    // dan Anti-Cheat mendarat dengan aman di layar masuk lobi
    std::this_thread::sleep_for(std::chrono::seconds(35));

    char* arg_fitur = (char*)nomor_case.c_str(); 
    char* susunan_argv[] = {
        (char*)"modul_zygisk",
        arg_fitur,             
        nullptr
    };
    int jumlah_argc = 2;
    
    LOGI("Menjalankan main.cpp untuk game: %s dengan nomor Case: %s", nama_game.c_str(), arg_fitur);
    
    // Eksekusi program utama Anda dengan mengirimkan nama paket game yang sudah valid
    main(jumlah_argc, susunan_argv, nama_game.c_str());
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
            // Memeriksa pakem target game secara akurat lewat memori internal Zygisk
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                // Jalur penyimpanan file teks di dalam sistem root modul Magisk Anda
                // ⚠️ GANTI "id_modul_anda" dengan ID yang tertulis di file module.prop Anda
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

                // Kirim data nama game ke dalam fungsi thread latar belakang
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
