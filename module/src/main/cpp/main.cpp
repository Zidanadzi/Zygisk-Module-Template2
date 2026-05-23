#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <cstring>

#include "zygisk.hpp"

using namespace zygisk;

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MyModule", __VA_ARGS__)

// --- LOGIKA MEMORYTOOLS ---
// (Diletakkan di sini agar bisa diakses oleh class MyModule)
class MemoryTools {
public:
    static void MemoryWrite(uintptr_t addr, int val) {
        uintptr_t page = addr & ~4095;
        mprotect((void*)page, 4096, PROT_READ | PROT_WRITE);
        memcpy((void*)addr, &val, 4);
        mprotect((void*)page, 4096, PROT_READ);
    }
    // Tambahkan fungsi lain di sini...
};

class MyModule : public ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        
        // Ganti dengan package game Anda
        if (strcmp(process, "com.tencent.ig") == 0) {
            LOGD("Proses ditemukan: %s, memulai thread...", process);
            
            std::thread([]() {
                sleep(20); // Tunggu game load
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string line;
                    if (std::getline(f, line)) {
                        f.close();
                        remove("/data/local/tmp/trigger.txt");
                        LOGD("Trigger diterima: %s", line.c_str());
                        // Panggil MemoryTools::MemoryWrite di sini
                    }
                    usleep(500000);
                }
            }).detach();
        }

        env->ReleaseStringUTFChars(args->nice_name, process);
        api->setOption(Option::DLCLOSE_MODULE_LIBRARY);
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)
