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

    static void Patch(uintptr_t addr, int val_d, DataType type) {
        uintptr_t page = addr & ~0xFFF;
        mprotect((void*)page, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
        pwrite(get_mem_fd(), &val_d, 4, addr);
        mprotect((void*)page, 0x1000, PROT_READ | PROT_EXEC);
    }

    // Fungsi Search Group dengan offset negatif sesuai permintaan Anda
    static std::vector<uintptr_t> SearchGroup(const std::vector<std::pair<int, int>>& group, Range range) {
        std::vector<uintptr_t> results;
        char line[256]; FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return results;

        while (fgets(line, sizeof(line), fp)) {
            unsigned int s, e; char p[5];
            sscanf(line, "%x-%x %4s", &s, &e, p);
            if (strstr(p, "w")) {
                // Scan di area memory
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 32; a += 4) {
                    bool match = true;
                    for (auto& item : group) {
                        int val; pread(get_mem_fd(), &val, 4, a + item.first);
                        if (val != item.second) { match = false; break; }
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
        
        // Filter proses PUBG Mobile
        if (proc && strstr(proc, "com.tencent.ig") && !strchr(proc, ':')) {
            std::thread([]() {
                sleep(40); // Tunggu lobby
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string cmd;
                    if (std::getline(f, cmd) && cmd == "3") { // Trigger Command 3
                        remove("/data/local/tmp/trigger.txt");

                        // Konversi logika script lama Anda ke SearchGroup
                        // Script Anda: 8200 (0), 8204 (-8), 8199 (-16), 8196 (-24)
                        auto addrs = MemoryTools::SearchGroup({
                            {0,    8200},
                            {-8,   8204},
                            {-16,  8199},
                            {-24,  8196}
                        }, ANONYMOUS);

                        for (auto a : addrs) {
                            MemoryTools::Patch(a, 6, TYPE_DWORD);
                            LOGD("PUBG Value Patched at: %lx", a);
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
