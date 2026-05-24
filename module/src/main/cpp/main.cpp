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
        // Deteksi package target game PUBG Mobile
        if (args->nice_name && strcmp(args->nice_name, "com.tencent.ig") == 0) {
            args->option |= zygisk::Option::FORCE_DONT_FORK;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        std::thread([]() {
            LOGI("Zygisk Modul Siap. Menunggu perintah manual dari file .sh...");

            // Perulangan untuk memantau file pemicu setiap 1 detik
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); 

                // Mengintip keberadaan file run_cheat di ruang penyimpanan sementara
                std::ifstream trigger_file("/data/local/tmp/run_cheat");
                
                if (trigger_file.good()) {
                    LOGI("=== Pemicu .sh Terdeteksi! Menjalankan MemoryTools ===");

                    // Tutup koneksi dan hapus file penanda agar tidak tereksekusi berulang
                    trigger_file.close();
                    std::remove("/data/local/tmp/run_cheat");

                    // ---------------------------------------------------------
                    // MAIN LOGIC (Eksekusi manual via .sh)
                    // ---------------------------------------------------------
                    MemoryTools::SetRange("ALL");
                    auto hasil_search = MemoryTools::Search(220.0f);

                    for (uintptr_t alamat : hasil_search) {
                        // Proses Refine: Cek nilai 178 di offset 24, dan nilai 15 di offset 28
                        if (std::fabs(MemoryTools::Offset(alamat, 24) - 178.0f) < 0.001f && 
                            std::fabs(MemoryTools::Offset(alamat, 28) - 15.0f) < 0.001f) {
                            
                            // Tulis nilai 500 ke alamat target hasil refine
                            MemoryTools::Write(alamat, 500.0f);
                            LOGI("Manual Patch Sukses: 0x%lx berhasil diubah ke 500", alamat);
                        }
                    }
                    LOGI("=== Selesai Mengeksekusi Perintah Manual ===");
                }
            }
        }).detach();
    }
};

REGISTER_ZYGISK_MODULE(PerfectModule)
