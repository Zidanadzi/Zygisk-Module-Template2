// TAMBAHKAN INI DI BARIS PALING ATAS UNTUK MEMPERBAIKI ERROR dev_t DAN ino_t
#include <sys/types.h> 

// Baru diikuti oleh include yang lainnya
#include <zygisk.hpp>
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
        // Zygisk mengambil nama paket aplikasi yang sedang dibuka saat ini
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            // Jalur penyimpanan file teks di dalam folder modul Magisk Anda
            // ⚠️ GANTI "id_modul_anda" dengan ID yang tertulis di file module.prop Anda
            std::string path_file = "/data/adb/modules/id_modul_anda/pilihan.txt";
            
            // JIKA FILE KOSONG ATAU TIDAK ADA: Otomatis diisi "1" sebagai Default (Case 1)
            std::string nomor_case = "1"; 
            std::ifstream file_konfig(path_file);
            
            if (file_konfig.is_open()) {
                std::string teks_bacaan;
                std::getline(file_konfig, teks_bacaan);
                file_konfig.close();
                
                // Jika file ditemukan dan isinya tidak kosong, gunakan isi tersebut
                if (!teks_bacaan.empty()) {
                    nomor_case = teks_bacaan;
                }
            }

            // Memasukkan string angka fitur ke argumen argv
            char* arg_fitur = (char*)nomor_case.c_str(); 

            // Menyusun argv tiruan untuk dikirimkan ke main.cpp Anda
            char* susunan_argv[] = {
                (char*)"modul_zygisk",
                arg_fitur,             // Ini akan dibaca oleh atoi(argv[1]) di main.cpp Anda
                nullptr
            };
            
            int jumlah_argc = 2;
            
            LOGI("Mengeksekusi main.cpp untuk aplikasi: %s dengan Case: %s", processName, arg_fitur);
            
            // LOMPAT LANGSUNG: Mengeksekusi file main.cpp Anda di dalam memori target
            main(jumlah_argc, susunan_argv);
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

// Daftarkan modul ke framework Zygisk
REGISTER_ZYGISK_MODULE(PerantaraZygisk)
