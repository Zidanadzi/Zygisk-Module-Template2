#include <zygisk.hpp>
#include <string.h>
#include <fstream>
#include <string>
#include <android/log.h>
#include <thread> // ──> 1. WAJIB TAMBAHKAN LIBRARY THREAD INI di paling atas

#define LOG_TAG "ZygiskBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern int main(int argc, char *argv[]);

// Membuat fungsi perantara lokal untuk membungkus susunan argumen ke dalam Thread
void JalankanDiBackground(std::string nomor_case) {
    // Memberikan jeda waktu 10-15 detik setelah game terbuka
    // Ini memberi waktu agar game selesai memuat anti-cheat, memuat logo, dan masuk ke layar utama
    sleep(12); 

    char* arg_fitur = (char*)nomor_case.c_str(); 

    char* susunan_argv[] = {
        (char*)"modul_zygisk",
        arg_fitur,             
        nullptr
    };
    
    int jumlah_argc = 2;
    
    LOGI("Thread Background Dimulai: Mengeksekusi main.cpp dengan Case: %s", arg_fitur);
    
    // Mengeksekusi main.cpp Anda dengan aman di jalur belakang (Background Thread)
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
            if (strcmp(processName, "com.tencent.ig") == 0 || 
                strcmp(processName, "com.vng.pubgmobile") == 0 || 
                strcmp(processName, "com.pubg.krmobile") == 0 || 
                strcmp(processName, "com.rekoo.pubgm") == 0) 
            {
                std::string path_file = "/data/adb/modules/template_module/pilihan.txt"; // GANTI ID MODUL ANDA
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

                // ──> PERBAIKAN UTAMA: JALANKAN VIA BACKGROUND THREAD <──
                // Program tidak akan memblokir game lagi karena langsung dilempar ke jalur terpisah
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
