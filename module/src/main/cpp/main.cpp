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
#include <initializer_list>

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
            uintptr_t s, e;
            char perm[8], path[256];
            if (sscanf(line, "%lx-%lx %s %*x %*s %*d %s", &s, &e, perm, path) < 3) continue;

            bool shouldScan = false;
            switch (currentRange) {
                case RANGE_ALL: shouldScan = true; break;
                case RANGE_CODE_APP: shouldScan = (perm[0] == 'r' && perm[2] == 'x' && strstr(path, ".so")); break;
                case RANGE_ANONYMOUS: shouldScan = (strlen(path) == 0); break;
                case RANGE_OTHER: shouldScan = (perm[0] == 'r' && perm[1] == 'w' && !strstr(path, "/system/") && !strstr(path, "libc")); break;
                case RANGE_LIBUE4: shouldScan = (strstr(path, "libUE4.so") != nullptr); break;
            }

            if (shouldScan) {
                for (uintptr_t a = s; a < e - 4; a += 4) {
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
                LOGD("Patch sukses di: %lx", addr);
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
                sleep(20);
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string line;
                    if (std::getline(f, line)) {
                        f.close();
                        remove("/data/local/tmp/trigger.txt");
                        int caseNum = std::atoi(line.c_str());

                        switch (caseNum) {
                            case 1:
                                MemoryTools::SetRange(RANGE_ANONYMOUS);
                                MemoryTools::ExecuteGroupPatch("100", TYPE_DWORD, 
                                {
                                    {"8200", TYPE_DWORD, 4},
                                    {"8300", TYPE_DWORD, 32}
                                }, 0, 50);
                                break;
                            case 2:
                                MemoryTools::SetRange(RANGE_LIBUE4);
                                for (auto addr : MemoryTools::MemorySearch("1.0", TYPE_FLOAT)) {
                                    MemoryTools::MemoryPatch(addr, 0, 0.0f);
                                }
                                LOGD("Black Sky diaktifkan pada libUE4.so");
                                break;
                        }
                    }
                    usleep(500000);
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
