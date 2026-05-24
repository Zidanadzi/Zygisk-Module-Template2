#include <sys/types.h> 

// GANTI TANDA < > MENJADI " " SEPERTI DI BAWAH INI:
#include "zygisk.hpp"

#include <string.h>
#include <fstream>
#include <string>
#include <android/log.h>

#define LOG_TAG "ZygiskBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ... sisa kode module.cpp di bawahnya tetap sama ...
// Deklarasikan fungsi main milik main.cpp Anda agar bisa dipanggil di sini
extern int main(int argc, char *argv[]);

class PerantaraZygisk : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

        void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            // ──> GERBANG PENYARING (FILTER) ANTISIPASI BOOTLOOP <──
            // Kode hanya boleh lanjut jika aplikasi yang dibuka adalah salah satu dari game ini
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

                char* arg_fitur = (char*)nomor_case.c_str(); 

                char* susunan_argv[] = {
                    (char*)"modul_zygisk",
                    arg_fitur,             
                    nullptr
                };
                
                int jumlah_argc = 2;
                
                // KODE ANDA AMAN DIJALANKAN DI SINI (HANYA DI DALAM GAME)
                main(jumlah_argc, susunan_argv);
            }
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

// Daftarkan modul ke framework Zygisk
REGISTER_ZYGISK_MODULE(PerantaraZygisk)
