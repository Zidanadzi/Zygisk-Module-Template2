#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>

// ──> SISIPKAN 4 BARIS INI TEPAT DI ATAS INClUDE MEMORYTOOLS <──
#ifdef pread64
#undef pread64
extern "C" ssize_t pread64(int fd, void* buf, size_t count, off64_t offset);
#endif

#include "MemoryTools.h" // File MemoryTools Anda dipanggil di sini

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ... sisa kode main.cpp ke bawahnya tetap sama seutuhnya ...

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

// HAPUS KATA 'const' AGAR SAMA PERSIS DENGAN MEMORYTOOLS ANDA
extern int isapkrunning(char* pkgName);
extern void initXMemoryTools(char* pkgName, char* rootMode);

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
        // Tambahkan (char*) di depan setiap nama paket aplikasi
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


        // Jalankan inisialisasi bawaan memorytools Anda
        initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk meluncurkan Game: %s dengan Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
        case 1:
            MemorySearch((char*)"220", TYPE_FLOAT);
            MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
            MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
            MemoryWrite((char*)"600", 0, TYPE_FLOAT);
    break;

        case 2:
            MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
            MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
            MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
            MemoryWrite((char*)"200", 0, TYPE_FLOAT);
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
