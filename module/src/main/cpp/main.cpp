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

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MYMOD", __VA_ARGS__)

enum Range { CODE_APP, ANONYMOUS, ALL, OTHER };

class MemoryTools {
public:
    static int get_mem_fd() {
        static int fd = open("/proc/self/mem", O_RDWR);
        return fd;
    }

    // Fungsi MemoryOffset menggantikan VerifyValueStrict
    static bool MemoryOffset(uintptr_t addr, float val) {
        float mem_val;
        if (pread(get_mem_fd(), &mem_val, 4, addr) != 4) return false;
        return mem_val == val;
    }

    static void Patch(uintptr_t addr, float val) {
        int fd = get_mem_fd();
        uintptr_t page_start = addr & ~0xFFF;
        if (mprotect((void*)page_start, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
            pwrite(fd, &val, sizeof(float), addr);
            mprotect((void*)page_start, 0x1000, PROT_READ | PROT_EXEC);
        }
    }

    static std::vector<uintptr_t> Search(std::string val, Range range, const char* lib_name = nullptr) {
        std::vector<uintptr_t> results;
        float target = std::stof(val);
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return results;

        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            unsigned int s, e;
            char perms[5], path[256];
            sscanf(line, "%x-%x %4s %*x %*s %*s %s", &s, &e, perms, path);

            bool match = false;
            if (range == ALL) match = true;
            else if (range == CODE_APP && lib_name && strstr(path, lib_name)) match = true;
            else if (range == ANONYMOUS && strlen(path) == 0 && strstr(perms, "rw")) match = true;
            else if (range == OTHER && strlen(path) > 0 && (!lib_name || !strstr(path, lib_name))) match = true;

            if (match) {
                for (uintptr_t a = (uintptr_t)s; a < (uintptr_t)e - 4; a += 4) {
                    float buf;
                    if (pread(get_mem_fd(), &buf, 4, a) == 4 && buf == target) {
                        results.push_back(a);
                    }
                }
            }
        }
        fclose(fp);
        return results;
    }
};

class MyModule : public ModuleBase {
public:
    void postAppSpecialize(const AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process && strstr(process, "com.tencent.ig")) {
            std::thread([]() {
                sleep(20);
                while (true) {
                    std::ifstream f("/data/local/tmp/trigger.txt");
                    std::string line;
                    if (std::getline(f, line)) {
                        remove("/data/local/tmp/trigger.txt");
                        int cmd = std::stoi(line);

                        // Fitur 1 menggunakan MemoryOffset
                        if (cmd == 1 || cmd == 3) {
                            auto addrs = MemoryTools::Search("220.0", ANONYMOUS);
                            for (auto addr : addrs) {
                                if (MemoryTools::MemoryOffset(addr + 24, 178.0f) && 
                                    MemoryTools::MemoryOffset(addr + 28, 15.0f)) {
                                    
                                    MemoryTools::Patch(addr, 500.0f);
                                    LOGD("Fitur 1 Patch Success (Offset Verified): %lx", (unsigned long)addr);
                                }
                            }
                        }

                        if (cmd == 2 || cmd == 3) {
                            auto addrs = MemoryTools::Search("1.0", CODE_APP, "libUE4.so");
                            for (auto addr : addrs) {
                                MemoryTools::Patch(addr, 0.0f);
                                LOGD("Fitur 2 Patch Success: %lx", (unsigned long)addr);
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
