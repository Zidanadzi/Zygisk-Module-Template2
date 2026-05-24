#include <cstdlib>
#include <unistd.h>
#include <android/log.h>
#include <thread>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <cstring>
#include <initializer_list>
#include <fstream>

#include "zygisk.hpp"

using namespace zygisk;

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MyModule", __VA_ARGS__)

enum DataType { TYPE_DWORD, TYPE_FLOAT };
enum MemoryRange { RANGE_ALL, RANGE_OTHER, RANGE_CODE_APP, RANGE_ANONYMOUS, RANGE_LIBUE4 };

class MemoryTools {
public:
    static MemoryRange currentRange;

    struct GroupItem {
        std::string val;
        DataType type;
        int offset;
    };

    static void SetRange(MemoryRange range) { currentRange = range; }

    static bool VerifyValue(uintptr_t addr, std::string val, DataType type) {
        char buffer[4];
        memcpy(buffer, (void*)addr, 4);
        if (type == TYPE_DWORD) return *(int*)buffer == std::atoi(val.c_str());
        else return *(float*)buffer == (float)std::atof(val.c_str());
    }

    static std::vector<uintptr_t> MemorySearch(const std::string& val, DataType type) {
        std::vector<uintptr_t> results;
        char target[4];
        if (type == TYPE_DWORD) { int d = std::atoi(val.c_str()); memcpy(target, &d, 4); }
        else { float f = (float)std::atof(val.c_str()); memcpy(target, &f, 4); }

        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return results;

        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            unsigned int s, e;
            char perm[8], path[256];
            memset(path, 0, sizeof(path));
            if (sscanf(line, "%x-%x %7s %*x %*s %*d %255s", &s, &e, perm, path) < 3) continue;

            bool shouldScan = false;
            switch (currentRange) {
                case RANGE_ALL: shouldScan = true; break;
                case RANGE_CODE_APP: shouldScan = (perm[0] == 'r' && perm[2] == 'x' && strstr(path, ".so")); break;
                case RANGE_ANONYMOUS: shouldScan = (strlen(path) == 0); break;
                case RANGE_OTHER: shouldScan = (perm[0] == 'r' && perm[1] == 'w' && !strstr(path, "/system/") && !strstr(path, "libc")); break;
                case RANGE_LIBUE4: shouldScan = (strstr(path, "libUE4.so") != nullptr); break;
            }

            if (shouldScan) {
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 4; a += 4) {
                    if (memcmp((void*)a, target, 4) == 0) results.push_back(a);
                }
            }
        }
        fclose(fp);
        return results;
    }

    template <typename T>
    static void MemoryPatch(uintptr_t baseAddr, int offset, T newValue) {
        uintptr_t target = baseAddr + offset;
        uintptr_t page = target & ~4095;
        mprotect((void*)page, 4096, PROT_READ | PROT_WRITE);
        *(T*)target = newValue;
        mprotect((void*)page, 4096, PROT_READ);
    }

    static void ExecuteGroupPatch(std::string mainVal, DataType mainType, std::initializer_list<GroupItem> group, int patchOffset, int newValue) {
        for (auto addr : MemorySearch(mainVal, mainType)) {
            bool allMatch = true;
            for (const auto& item : group) {
                if (!VerifyValue(addr + item.offset, item.val, item.type)) { allMatch = false; break; }
            }
            if (allMatch) {
                MemoryPatch(addr, patchOffset, newValue);
                LOGD("Patch sukses di: %lx", (unsigned long)addr);
            }
        }
    }
};

MemoryRange MemoryTools::currentRange = RANGE_ALL;

class MyModule : public ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override { this->api = api; this->env = env; }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (strcmp(process, "com.tencent.ig") == 0) {
            std::thread([]() {
                // Tunggu 20 detik agar game sudah loading sempurna
                sleep(20);
                LOGD("Modul menunggu trigger...");

                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string line;
                    if (std::getline(f, line)) {
                        f.close();
                        remove("/data/local/tmp/trigger.txt"); // Hapus trigger agar tidak looping
                        int cmd = std::atoi(line.c_str());

                        // FITUR 1 (RANGE_ALL, target 220, offset 24 & 28, patch 500)
                        if (cmd == 1 || cmd == 3) {
                            MemoryTools::SetRange(RANGE_ALL);
                            MemoryTools::ExecuteGroupPatch("220", TYPE_FLOAT, 
                            {
                                {"178", TYPE_FLOAT, 24},
                                {"15", TYPE_FLOAT, 28}
                            }, 0, 500);
                            LOGD("Fitur 1 diaktifkan.");
                        }

                        // FITUR 2 (RANGE_LIBUE4, target 1.0, patch 0.0)
                        if (cmd == 2 || cmd == 3) {
                            MemoryTools::SetRange(RANGE_LIBUE4);
                            for (auto addr : MemoryTools::MemorySearch("1.0", TYPE_FLOAT)) {
                                MemoryTools::MemoryPatch(addr, 0, 0.0f);
                            }
                            LOGD("Fitur 2 diaktifkan.");
                        }
                    }
                    sleep(1); // Cek setiap 1 detik, sangat ringan bagi CPU
                }
            }).detach();
        }
        env->ReleaseStringUTFChars(args->nice_name, process);
        api->setOption(Option::DLCLOSE_MODULE_LIBRARY);
    }
private:
    Api *api; JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)
