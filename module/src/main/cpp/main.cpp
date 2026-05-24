#include <jni.h>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <android/log.h>
#include <cinttypes> // PENTING: Diperlukan untuk makro cetak PRIxPTR
#include "zygisk.hpp"
#include "memorytools.h" 

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "ZygiskMT", __VA_ARGS__)

class PerfectModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args->nice_name) {
            const char *package_name = env->GetStringUTFChars(args->nice_name, nullptr);
            if (package_name && strcmp(package_name, "com.tencent.ig") == 0) {
                // Modul dibiarkan menempel secara pasif
            }
            if (package_name) {
                env->ReleaseStringUTFChars(args->nice_name, package_name);
            }
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        std::thread([]() {
            LOGI("Zygisk Modul Siap. Menunggu perintah dari menu interaktif .sh...");

            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); 

                std::ifstream trigger_file("/data/local/tmp/run_cheat");
                
                if (trigger_file.good()) {
                    std::string kode_menu;
                    std::getline(trigger_file, kode_menu);
                    trigger_file.close();
                    
                    std::remove("/data/local/tmp/run_cheat");

                    MemoryTools::SetRange("ALL");

                    if (kode_menu == "1") {
                        LOGI("=== Menerima Perintah: AKTIFKAN FITUR ===");
                        auto hasil_search = MemoryTools::Search(220.0f);
                        int sukses_patch = 0;
                        
                        for (uintptr_t alamat : hasil_search) {
                            if (std::fabs(MemoryTools::Offset(alamat, 24) - 178.0f) < 0.001f && 
                                std::fabs(MemoryTools::Offset(alamat, 28) - 15.0f) < 0.001f) {
                                
                                MemoryTools::Write(alamat, 500.0f);
                                sukses_patch++;
                                
                                // FIX ERROR: Mengganti 0x%lx dengan 0x%" PRIxPTR " agar aman di 32-bit & 64-bit
                                LOGI("Manual Patch Sukses: 0x%" PRIxPTR " diubah ke 500", alamat);
                                break; 
                            }
                        }
                        LOGI("=== Fitur Selesai Diproses. Sukses: %d ===", sukses_patch);

                    } else if (kode_menu == "2") {
                        LOGI("=== Menerima Perintah: NONAKTIFKAN FITUR ===");
                        auto hasil_search = MemoryTools::Search(500.0f);
                        int sukses_restore = 0;
                        
                        for (uintptr_t alamat : hasil_search) {
                            if (std::fabs(MemoryTools::Offset(alamat, 24) - 178.0f) < 0.001f && 
                                std::fabs(MemoryTools::Offset(alamat, 28) - 15.0f) < 0.001f) {
                                
                                MemoryTools::Write(alamat, 220.0f); 
                                sukses_restore++;
                                
                                // FIX ERROR: Mengganti 0x%lx dengan 0x%" PRIxPTR " agar aman di 32-bit & 64-bit
                                LOGI("Manual Restore Sukses: 0x%" PRIxPTR " dikembangkan ke 220", alamat);
                                break;
                            }
                        }
                        LOGI("=== Fitur Selesai Dinonaktifkan. Sukses: %d ===", sukses_restore);
                    }
                    
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
        }).detach();
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(PerfectModule)
