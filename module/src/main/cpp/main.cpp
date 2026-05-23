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

// --- LOGIKA MEMORY TOOLS ---
enum DataType { TYPE_DWORD, TYPE_FLOAT };
enum MemoryRange { RANGE_ALL, RANGE_OTHER };

class MemoryTools {
private:
    static std::vector<uintptr_t> results;
    static MemoryRange current_range;

    static void stringToBytes(const std::string& val, DataType type, void* output) {
        if (type == TYPE_DWORD) {
            int d = std::stoi(val);
            memcpy(output, &d, 4);
        } else {
            float f = std::stof(val);
            memcpy(output, &f, 4);
        }
    }

public:
    static void SetSearchRange(MemoryRange range) { current_range = range; }
    static void ClearResults() { results.clear(); }

    static void MemorySearch(const std::string& val, DataType type) {
        results.clear();
        char target[4];
        stringToBytes(val, type, target);
        FILE* fp = fopen("/proc/self/maps", "r");
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            uintptr_t s, e;
            char path[256];
            if (sscanf(line, "%lx-%lx r--p %*x %*s %*d %s", &s, &e, path) == 3) {
                bool shouldScan = (current_range == RANGE_ALL);
                if (current_range == RANGE_OTHER) {
                    if (strstr(path, ".so") || strlen(path) == 0) {
                        if (!strstr(path, "libc") && !strstr(path, "stack")) shouldScan = true;
                    }
                }
                if (shouldScan) {
                    for (uintptr_t a = s; a < e - 4; a += 4) {
                        if (memcmp((void*)a, target, 4) == 0) results.push_back(a);
                    }
                }
            }
        }
        fclose(fp);
    }

    static void MemoryWrite(const std::string& val, int offset, DataType type) {
        char target[4];
        stringToBytes(val, type, target);
        for (auto addr : results) {
            uintptr_t targetAddr = addr + offset;
            uintptr_t page = targetAddr & ~4095;
            mprotect((void*)page, 4096, PROT_READ | PROT_WRITE);
            memcpy((void*)targetAddr, target, 4);
            mprotect((void*)page, 4096, PROT_READ);
        }
    }
};

std::vector<uintptr_t> MemoryTools::results;
MemoryRange MemoryTools::current_range = RANGE_ALL;

// --- KELAS MODUL UTAMA ---
class MyModule : public ModuleBase {
public:
    void postAppSpecialize(const AppSpecializeArgs *args) override {
        // Ambil JNIEnv untuk konversi jstring ke char*
        JNIEnv* env = args->env;
        
        // Konversi jstring ke const char*
        const char *name = env->GetStringUTFChars(args->nice_name, nullptr);
        
        // Sekarang gunakan 'name' (yang bertipe const char*)
        if (!name || strcmp(name, "com.tencent.ig") != 0) {
            env->ReleaseStringUTFChars(args->nice_name, name); // Penting: release memory
            return;
        }

        LOGD("Module aktif di: %s", name);
        env->ReleaseStringUTFChars(args->nice_name, name); // Release setelah digunakan

        std::thread([]() {
            sleep(20); // Tunggu game inisialisasi

            while (true) {
                std::ifstream f("/data/local/tmp/trigger.txt");
                std::string line;
                if (std::getline(f, line)) {
                    f.close();
                    remove("/data/local/tmp/trigger.txt");

                    int moduleNum = (line.length() > 0) ? (line.back() - '0') : 0;
                    LOGD("Trigger diterima: %d", moduleNum);

                    switch (moduleNum) {
                        case 1:
                            MemoryTools::SetSearchRange(RANGE_OTHER);
                            MemoryTools::MemorySearch("8200", TYPE_DWORD);
                            MemoryTools::MemoryWrite("6", 0, TYPE_DWORD);
                            LOGD("Module 1 dieksekusi");
                            break;
                        case 2:
                            // Logic module 2
                            break;
                    }
                    MemoryTools::ClearResults();
                }
                usleep(500000);
            }
        }).detach();
    }
}
// Register module
REGISTER_ZYGISK_MODULE(MyModule)
