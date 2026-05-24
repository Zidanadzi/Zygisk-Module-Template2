#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <thread>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <cstring>
#include <fstream>

#include "zygisk.hpp"

using namespace zygisk;

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MyModule", __VA_ARGS__)

enum DataType { TYPE_DWORD, TYPE_FLOAT };
enum MemoryRange { RANGE_ALL, RANGE_ANONYMOUS, RANGE_LIBUE4 };

class MemoryTools {
public:
    static MemoryRange currentRange;
    static int mem_fd;

    static void Init() { if(mem_fd < 0) mem_fd = open("/proc/self/mem", O_RDONLY); }
    static void SetRange(MemoryRange range) { currentRange = range; }

    // Membandingkan representasi bit mentah float
    static bool VerifyValueStrict(uintptr_t addr, float val) {
        uint32_t mem_bits, target_bits;
        if (pread(mem_fd, &mem_bits, 4, addr) != 4) return false;
        memcpy(&target_bits, &val, 4);
        return mem_bits == target_bits;
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
            sscanf(line, "%x-%x %7s %*x %*s %*d %255s", &s, &e, perm, path);
            
            bool shouldScan = false;
            if (currentRange == RANGE_ALL) shouldScan = (perm[0] == 'r');
            else if (currentRange == RANGE_ANONYMOUS) shouldScan = (strlen(path) == 0 && perm[0] == 'r');
            else if (currentRange == RANGE_LIBUE4) shouldScan = (strstr(path, "libUE4.so") != nullptr && perm[0] == 'r');

            if (shouldScan) {
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 4; a += 4) {
                    char buf[4];
                    if (pread(mem_fd, buf, 4, a) == 4 && memcmp(buf, target, 4) == 0) {
                        results.push_back(a);
                    }
                }
            }
        }
        fclose(fp);
        return results;
    }

    static void MemoryPatchFloat(uintptr_t target, float newValue) {
        uintptr_t page = target & ~4095;
        mprotect((void*)page, 4096, PROT_READ | PROT_WRITE);
        *(float*)target = newValue;
        mprotect((void*)page, 4096, PROT_READ);
    }
};

MemoryRange MemoryTools::currentRange = RANGE_ANONYMOUS;
int MemoryTools::mem_fd = -1;

class MyModule : public ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override { this->api = api; this->env = env; MemoryTools::Init(); }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (strcmp(process, "com.tencent.ig") == 0) {
            std::thread([]() {
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string line;
                    if (std::getline(f, line)) {
                        f.close();
                        remove("/data/local/tmp/trigger.txt");
                        int cmd = std::atoi(line.c_str());

                        if (cmd == 1 || cmd == 3) {
                            MemoryTools::SetRange(RANGE_ALL);
                            auto addrs = MemoryTools::MemorySearch("220.0", TYPE_FLOAT);
                            for (auto addr : addrs) {
                                if (MemoryTools::VerifyValueStrict(addr + 24, 178.0f) && 
                                    MemoryTools::VerifyValueStrict(addr + 28, 15.0f)) {
                                    MemoryTools::MemoryPatchFloat(addr, 500.0f);
                                    LOGD("Fitur 1 Applied.");
                                }
                            }
                        }
                        if (cmd == 2 || cmd == 3) {
                            MemoryTools::SetRange(RANGE_LIBUE4);
                            auto addrs = MemoryTools::MemorySearch("1.0", TYPE_FLOAT);
                            for (auto addr : addrs) {
                                MemoryTools::MemoryPatchFloat(addr, 0.0f);
                                LOGD("Fitur 2 Applied.");
                            }
                        }
                    }
                    sleep(2);
                }
            }).detach();
        }
        env->ReleaseStringUTFChars(args->nice_name, process);
    }
private:
    Api *api; JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)
