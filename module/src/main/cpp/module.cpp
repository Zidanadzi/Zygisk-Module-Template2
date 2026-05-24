#include <zygisk.hpp>
#include <string.h>
#include <fstream>
#include <string>

extern int main(int argc, char *argv[]);

class PerantaraZygisk : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            // Jalur file teks konfigurasi Anda
            // GANTI "id_modul_anda" sesuai folder modul Magisk Anda
            std::string path_file = "/data/adb/modules/id_modul_anda/pilihan.txt";
            
            // JIKA KOSONG / TIDAK ADA: Otomatis diisi "1" agar menjalankan Case 1
            std::string nomor_case = "1"; 
            std::ifstream file_konfig(path_file);
            
            if (file_konfig.is_open()) {
                std::string teks_bacaan;
                std::getline(file_konfig, teks_bacaan);
                file_konfig.close();
                
                // Jika file ada tapi isinya tidak kosong, gunakan isi file tersebut
                if (!teks_bacaan.empty()) {
                    nomor_case = teks_bacaan;
                }
            }

            // Kirim string angka ke argv di main.cpp
            char* arg_fitur = (char*)nomor_case.c_str(); 

            char* susunan_argv[] = {
                (char*)"modul_zygisk",
                arg_fitur,             
                nullptr
            };
            
            int jumlah_argc = 2;
            
            // Jalankan kode utama main.cpp Anda
            main(jumlah_argc, susunan_argv);
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }
};

REGISTER_ZYGISK_MODULE(PerantaraZygisk)
