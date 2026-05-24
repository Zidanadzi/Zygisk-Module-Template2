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
enum Range { CODE_APP, ANONYMOUS, ALL };

class MemoryTools {
public:
    static int get_mem_fd() { static int fd = open("/proc/self/mem", O_RDWR); return fd; }

    static void Patch(uintptr_t addr, float val_f, int val_d, DataType type) {
        uintptr_t page = addr & ~0xFFF;
        mprotect((void*)page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
        type == TYPE_FLOAT ? pwrite(get_mem_fd(), &val_f, 4, addr) : pwrite(get_mem_fd(), &val_d, 4, addr);
        mprotect((void*)page, 0x1000, PROT_READ | PROT_EXEC);
    }

    static std::vector<uintptr_t> SearchGroup(const std::vector<std::pair<int, float>>& group, Range range, DataType type, const char* lib = nullptr) {
        std::vector<uintptr_t> results;
        char line[256]; FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return results;

        while (fgets(line, sizeof(line), fp)) {
            unsigned int s, e; char p[5], path[256];
            sscanf(line, "%x-%x %4s %*x %*s %*s %s", &s, &e, p, path);
            if (!strstr(p, "w")) continue;

            bool match = (range == ALL) || (range == CODE_APP && lib && strstr(path, lib)) || (range == ANONYMOUS && strlen(path) == 0);
            if (match) {
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 100; a += 4) {
                    bool matchGroup = true;
                    for (auto& item : group) {
                        float val; pread(get_mem_fd(), &val, 4, a + item.first);
                        if (val != item.second) { matchGroup = false; break; }
                    }
                    if (matchGroup) results.push_back(a);
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
        if (proc && strstr(proc, "com.tencent.ig") && !strchr(proc, ':')) {
            std::thread([]() {
                sleep(40);
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string cmd;
                    if (std::getline(f, cmd)) {
                        remove("/data/local/tmp/trigger.txt");

                        // FITUR 1: Search Group Lengkap
                        if (cmd == "1") {
                            auto addrs = MemoryTools::SearchGroup({
                                {0,  220.0f},
                                {24, 178.0f},
                                {28, 15.0f}  // Nilai 15.0f pada offset 28 dikembalikan
                            }, ANONYMOUS, TYPE_FLOAT);
                            for (auto a : addrs) { MemoryTools::Patch(a, 500.0f, 0, TYPE_FLOAT); LOGD("Fitur 1 Patch: %lx", a); }
                        }

                        // FITUR 2: Search Dword Single
                        if (cmd == "2") {
                            auto addrs = MemoryTools::SearchGroup({{0, 100.0f}}, CODE_APP, TYPE_DWORD, "libUE4.so");
                            for (auto a : addrs) { MemoryTools::Patch(a, 0.0f, 999, TYPE_DWORD); LOGD("Fitur 2 Patch: %lx", a); }
                        }
                    }
                    sleep(2);
                }
            }).detach();
        }
        env->ReleaseStringUTFChars(args->nice_name, proc);
    }
private:
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)
