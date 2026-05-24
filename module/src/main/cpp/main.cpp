#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>

// Panggil file header Anda di sini
#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

int main(int argc, char *argv[]) 
{
    // Memastikan argumen argv tersedia sebelum diproses oleh atoi
    if (argc < 2 || argv == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // Mengambil pilihan fitur dari parameter Zygisk
    int Fitur = atoi(argv[1]); 

    {
        char pkg[100] = {0};
        
        // Membuat Objek/Instance dari Class MemoryTool yang ada di MemoryTools.h
        MemoryTool tool;

        // Memanggil fungsi pengecekan apk melalui objek 'tool'
        if (tool.isapkrunning((char*)"com.tencent.ig") == 1)
        {
            sprintf(pkg, "com.tencent.ig");
        }
        else if (tool.isapkrunning((char*)"com.vng.pubgmobile") == 1)
        {
            sprintf(pkg, "com.vng.pubgmobile");
        }
        else if (tool.isapkrunning((char*)"com.pubg.krmobile") == 1)
        {
            sprintf(pkg, "com.pubg.krmobile");
        }
        else if (tool.isapkrunning((char*)"com.rekoo.pubgm") == 1)
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

        // Jalankan inisialisasi memori melalui objek 'tool'
        tool.initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk meluncurkan Game: %s dengan Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
        case 1:
            LOGI("Menjalankan Fitur LOGIKA CASE 1");
            
            // PERBAIKAN UTAMA: Panggil fungsi memori wajib melalui objek 'tool.'
            tool.MemorySearch((char*)"220", TYPE_FLOAT);
            tool.MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
            tool.MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
            tool.MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
            break;
            
        case 2:
            LOGI("Menjalankan Fitur LOGIKA CASE 2");
            tool.MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
            tool.MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
            tool.MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
            tool.MemoryWrite((char*)"200", 0, TYPE_FLOAT);
            break;
            
        default:
            LOGI("Case tidak dikenal atau kosong.");
            break;
        }
    }

    return 0;
}
