#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include "MemoryTools.h" // Otomatis memuat file MemoryTools eksternal Anda

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

// Deklarasi fungsi dari MemoryTools Anda agar compiler tidak error
extern int isapkrunning(const char* pkgName);
extern void initXMemoryTools(const char* pkgName, const char* rootMode);

int main(int argc, char *argv[]) 
{
    // Memastikan argumen argv[1] tersedia sebelum diproses oleh atoi
    if (argc < 2 || argv[1] == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // Mengambil pilihan fitur sesuai awalan cpp Anda
    int Fitur = atoi(argv[1]); 

    {
        char pkg[100] = {0};
        if (isapkrunning("com.tencent.ig") == 1)
        {
            sprintf(pkg, "com.tencent.ig");
        }
        else if (isapkrunning("com.vng.pubgmobile") == 1)
        {
            sprintf(pkg, "com.vng.pubgmobile");
        }
        else if (isapkrunning("com.pubg.krmobile") == 1)
        {
            sprintf(pkg, "com.pubg.krmobile");
        }
        else if (isapkrunning("com.rekoo.pubgm") == 1)
        {
            sprintf(pkg, "com.rekoo.pubgm");
        }

        char getRoot[100] = {0};
        if (getuid() == 0) {
            sprintf(getRoot, "MODE_ROOT");
        }
        else {
            sprintf(getRoot, "MODE_NO_ROOT");
        }

        // Jalankan inisialisasi bawaan memorytools Anda
        initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk meluncurkan Game: %s dengan Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
        case 1:
            LOGI("Menjalankan Fitur LOGIKA CASE 1");
            SetSearchRange(ALL); //wide
            MemorySearch("220", TYPE_FLOAT);
            MemoryOffset("178", 0x18, TYPE_FLOAT);
            MemoryOffset("15", 0x1C, TYPE_FLOAT);            
            MemoryWrite("600", 0, TYPE_FLOAT);
            ClearResults();            
           
            SetSearchRange(ALL); //black sky
            MemorySearch("0.05000000075", TYPE_FLOAT);
            MemoryOffset("3.4028235e38", -0x4, TYPE_FLOAT);
            MemoryOffset("8.04061356e-15", 0x48, TYPE_FLOAT);
            MemoryWrite("200", 0, TYPE_FLOAT);
            ClearResults();      
            break;
            
        case 2:
            LOGI("Menjalankan Fitur LOGIKA CASE 2");
            // ──> MASUKKAN LOGIKA MANIPULASI MEMORI CASE 2 ANDA DI SINI <──
            break;
            
        case 3:
            LOGI("Menjalankan Fitur LOGIKA CASE 3");
            // ──> MASUKKAN LOGIKA MANIPULASI MEMORI CASE 3 ANDA DI SINI <──
            break;
            
        default:
            LOGI("Case tidak dikenal atau kosong.");
            break;
        }
    }

    return 0;
}
