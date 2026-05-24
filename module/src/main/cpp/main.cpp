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

// Deklarasi fungsi dari MemoryTools Anda tanpa kata 'const'
extern int isapkrunning(char* pkgName);
extern void initXMemoryTools(char* pkgName, char* rootMode);

int main(int argc, char *argv[]) 
{
    // Memastikan argumen argv tersedia sebelum diproses oleh atoi
    if (argc < 2 || argv == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // Mengambil pilihan fitur sesuai awalan cpp Anda
    int Fitur = atoi(argv[1]); 

    {
        // PERBAIKAN: Ubah menjadi array karakter agar bisa menampung teks string
        char pkg[100] = {0};
        
        if (isapkrunning((char*)"com.tencent.ig") == 1)
        {
            sprintf(pkg, "com.tencent.ig");
        }
        else if (isapkrunning((char*)"com.vng.pubgmobile") == 1)
        {
            sprintf(pkg, "com.vng.pubgmobile");
        }
        else if (isapkrunning((char*)"com.pubg.krmobile") == 1)
        {
            sprintf(pkg, "com.pubg.krmobile");
        }
        else if (isapkrunning((char*)"com.rekoo.pubgm") == 1)
        {
            sprintf(pkg, "com.rekoo.pubgm");
        }

        // PERBAIKAN: Ubah menjadi array karakter agar tidak memicu error 'undeclared identifier'
        char getRoot[100] = {0};
        
        if (getuid() == 0) {
            sprintf(getRoot, "MODE_ROOT");
        }
        else {
            sprintf(getRoot, "MODE_NO_ROOT");
        }

        // Jalankan inisialisasi bawaan memorytools Anda dengan aman
        initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk meluncurkan Game: %s dengan Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
        case 1:
            LOGI("Menjalankan Fitur LOGIKA CASE 1");
            // ──> MASUKKAN LOGIKA MANIPULASI MEMORI CASE 1 ANDA DI SINI <──
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
