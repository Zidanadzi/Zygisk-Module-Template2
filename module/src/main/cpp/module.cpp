#include <sys/types.h> 
#include "zygisk.hpp"
#include <string.h>
#include <fstream>
#include <string>
#include <android/log.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/mman.h> // Diperlukan untuk manipulasi proteksi memori tingkat kernel

#define LOG_TAG "ZygiskBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern int main(int argc, char *argv[]);

void JalankanDiBackground(std::string nomor_case) {
    // 1. Jeda penundaan awal 20 detik untuk membiarkan Anti-Cheat selesai memindai integrasi awal game
    std::this_thread::sleep_for(std::chrono::seconds(20));

    // 2. Memastikan peta memori /proc/self/maps lokal sudah tenang dan bisa diakses secara aman
    int percobaan = 0;
    while (percobaan < 5) {
        std::ifstream cek_status("/proc/self/maps");
        if (cek_status.is_open()) {
            cek_status.close();
            break; 
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
        percobaan++;
    }

    // Jeda tambahan agar UI lobi stabil
    std::this_thread::sleep_for(std::chrono::seconds(3));

    char* arg_fitur = (char*)nomor_case.c_str(); 
    char* susunan_argv[] = {
        (char*)"modul_zygisk",
        arg_fitur,             
        nullptr
    };
    int jumlah_argc = 2;
    
    LOGI("Zygisk sukses menyembunyikan jejak maps. Memulai eksekusi kode memori...");
    
    // Eksekusi fungsi utama main.cpp Anda
    main(jumlah_argc, susunan_argv);
}

class PerantaraZygisk : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    // ──> TRIK UTAMA SOLUSI 1: MODIFIKASI STATUS INJEKSI SEBELUM GAME BERAKSI <──
    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            // Saring target paket game PUBG Anda
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                // Menurunkan level pengawasan Zygisk pada thread ini agar tidak meninggalkan log jejak 
                // yang bisa diendus oleh modul pendeteksi Anti-Cheat Tencent
                api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
            }
        }
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                std::string path_file = "/data/adb/modules/template_module/pilihan.txt"; // ⚠️ SESUAIKAN ID MODUL ANDA
                
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

                // Jalankan di jalur belakang dengan proteksi Zygisk API Option aktif
                std::thread(JalankanDiBackground, nomor_case).detach();
            }
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(PerantaraZygisk)
