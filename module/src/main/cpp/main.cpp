#include <jni.h>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <android/log.h>
#include "zygisk.hpp"
#include "memorytools.h" 

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "ZygiskMT", __VA_ARGS__)

class PerfectModule : public zygisk::ModuleBase {
public:
    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args->nice_name && strcmp(args->nice_name, "com.tencent.ig") == 0) {
            args->option |= zygisk::Option::FORCE_DONT_FORK;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        std::thread([]() {
            LOGI("Zygisk Modul Siap. Menunggu perintah dari menu interaktif .sh...");

            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); 

                std::ifstream trigger_file("/data/local/tmp/run_cheat");
                
                if (trigger_file.good()) {
                    // Baca isi angka menu yang dikirim oleh skrip .sh (1 atau 2)
                    std::string kode_menu;
                    std::getline(trigger_file, kode_menu);
                    trigger_file.close();
                    
                    // Langsung hapus file pemicu setelah dibaca
                    std::remove("/data/local/tmp/run_cheat");

                    MemoryTools::SetRange("ALL");

                    if (kode_menu == "1") {
                        LOGI("=== Menerima Perintah: AKTIFKAN FITUR ===");
                        // Cari nilai awal 220 untuk diubah ke 500
                        auto hasil_search = MemoryTools::Search(220.0f);
                        for (uintptr_t alamat : hasil_search) {
                            if (std::fabs(MemoryTools::Offset(alamat, 24) - 178.0f) < 0.001f && 
                                std::fabs(MemoryTools::Offset(alamat, 28) - 15.0f) < 0.001f) {
                                MemoryTools::Write(alamat, 500.0f);
                            }
                        }
                        LOGI("=== Fitur Sukses Diaktifkan ===");

                    } else if (kode_menu == "2") {
                        LOGI("=== Menerima Perintah: NONAKTIFKAN FITUR ===");
                        // Balikkan logika: cari nilai 500 untuk dikembalikan ke 220
                        auto hasil_search = MemoryTools::Search(500.0f);
                        for (uintptr_t alamat : hasil_search) {
                            if (std::fabs(MemoryTools::Offset(alamat, 24) - 178.0f) < 0.001f && 
                                std::fabs(MemoryTools::Offset(alamat, 28) - 15.0f) < 0.001f) {
                                MemoryTools::Write(alamat, 220.0f); // Kembalikan ke normal
                            }
                        }
                        LOGI("=== Fitur Sukses Dinonaktifkan ===");
                    }
                }
            }
        }).detach();
    }
};

REGISTER_ZYGISK_MODULE(PerfectModule)
