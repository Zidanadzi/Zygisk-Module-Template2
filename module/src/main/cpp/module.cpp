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

extern int main(int argc, char *argv[]);

// Fungsi perantara untuk mengeksekusi kode memori di jalur latar belakang
void JalankanDiBackground(std::string nomor_case) {
    // Jeda awal 15 detik agar game melewati proses inisialisasi logo awal
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Melakukan pengecekan kestabilan peta memori internal game sebelum menyentuh lobi
    int percobaan = 0;
    while (percobaan < 10) {
        std::ifstream cek_status("/proc/self/maps");
        if (cek_status.is_open()) {
            cek_status.close();
            break; // Jika memori lokal sudah mapan dan bisa diakses, keluar dari loop
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
        percobaan++;
    }

    // Jeda ekstra 5 detik terakhir agar rendering aset lobi selesai sepenuhnya
    std::this_thread::sleep_for(std::chrono::seconds(5));

    char* arg_fitur = (char*)nomor_case.c_str(); 
    char* susunan_argv[] = {
        (char*)"modul_zygisk",
        arg_fitur,             
        nullptr
    };
    int jumlah_argc = 2;
    
    LOGI("Mengeksekusi main.cpp dengan aman di lobi game untuk Case Fitur: %s", arg_fitur);
    
    // Lompat langsung menjalankan fungsi memori Anda
    main(jumlah_argc, susunan_argv);
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
            // GERBANG UTAMA ANTISIPASI BOOTLOOP: Hanya jalan jika package adalah game PUBG
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                // Jalur penyimpanan file teks di dalam sistem root modul Magisk Anda
                // ⚠️ GANTI "id_modul_anda" dengan ID yang tertulis di file module.prop Anda
                std::string path_file = "/data/adb/modules/template_module/pilihan.txt";
                
                std::string nomor_case = "1"; // Default awal jika file kosong / belum ada
                std::ifstream file_konfig(path_file);
                
                if (file_konfig.is_open()) {
                    std::string teks_bacaan;
                    std::getline(file_konfig, teks_bacaan);
                    file_konfig.close();
                    
                    if (!teks_bacaan.empty()) {
                        nomor_case = teks_bacaan;
                    }
                }

                // Lepaskan eksekusi dari Main UI Thread game ke Background Thread agar game tidak crash
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
