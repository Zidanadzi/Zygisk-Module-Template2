#include <unistd.h>
#include <android/log.h>
#include <thread>
#include <fstream>
#include <vector>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>
#include "zygisk.hpp"

using namespace zygisk;

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MyModule", __VA_ARGS__)

enum DataType { TYPE_FLOAT, TYPE_DWORD };
enum Range    { CODE_APP, ANONYMOUS, ALL };

class MemoryTools {
public:
    static int get_mem_fd() { static int fd = open("/proc/self/mem", O_RDWR); return fd; }

    static void Patch(uintptr_t addr, float f, int d, DataType type) {
        if (!addr) return;
        uintptr_t page = addr & ~0xFFF;
        if (mprotect((void*)page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
            type == TYPE_FLOAT ? pwrite(get_mem_fd(), &f, 4, addr) : pwrite(get_mem_fd(), &d, 4, addr);
            mprotect((void*)page, 0x1000, PROT_READ | PROT_EXEC);
        }
    }

    static std::vector<uintptr_t> Search(const std::vector<std::pair<int, float>>& group, Range r, DataType t, const char* lib = nullptr) {
        std::vector<uintptr_t> results;
        char line[256], path[256], p[5];
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return results;

        while (fgets(line, sizeof(line), fp)) {
            unsigned int s, e;
            sscanf(line, "%x-%x %4s %*x %*s %*s %s", &s, &e, p, path);
            if (!strstr(p, "w")) continue;

            bool matchR = (r == ALL) || (r == CODE_APP && lib && strstr(path, lib)) || (r == ANONYMOUS && strlen(path) == 0);
            if (matchR) {
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 64; a += 4) {
                    bool match = true;
                    for (auto& i : group) {
                        float v; pread(get_mem_fd(), &v, 4, a + i.first);
                        if (v != i.second) { match = false; break; }
                    }
                    if (match) results.push_back(a);
                }
            }
        }
        fclose(fp); return results;
    }
};

class MyModule : public ModuleBase {
public:
    void postAppSpecialize(const AppSpecializeArgs *args) override {
        const char *proc = env->GetStringUTFChars(args->nice_name, nullptr);
        if (proc && strcmp(proc, "com.tencent.ig") == 0) {
            std::thread([]() {
                sleep(60); // Waktu tunggu agar sistem stabil dan game sudah ter-load
                
                std::ifstream f("/data/local/tmp/trigger.txt");
                std::string cmd;
                if (std::getline(f, cmd)) {
                    remove("/data/local/tmp/trigger.txt");

                    // FITUR 1: Search Group (PUBG Logic)
                    if (cmd == "1") {
                        auto addrs = MemoryTools::Search({
                            {0,    8200.0f},
                            {-8,   8204.0f},
                            {-16,  8199.0f},
                            {-24,  8196.0f}
                        }, ANONYMOUS, TYPE_DWORD);

                        for (auto a : addrs) {
                            MemoryTools::Patch(a, 0, 6, TYPE_DWORD);
                            LOGD("Patch 1 Success: %p", (void*)a);
                        }
                    }
                    // FITUR 2: Search Single Float
                    else if (cmd == "2") {
                        auto addrs = MemoryTools::Search({{0, 220.0f}}, CODE_APP, TYPE_FLOAT, "libUE4.so");
                        for (auto a : addrs) {
                            MemoryTools::Patch(a, 500.0f, 0, TYPE_FLOAT);
                            LOGD("Patch 2 Success: %p", (void*)a);
                        }
                    }
                }
            }).detach();
        }
        env->ReleaseStringUTFChars(args->nice_name, proc);
    }
private:
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)
