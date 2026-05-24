#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>

// Panggil file header MemoryTools Anda terlebih dahulu
#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

int main(int argc, char *argv[]) 
{
    // Memastikan argumen argv dan isi argumen pertamanya tersedia
    if (argc < 2 || argv == nullptr || argv[1] == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // PERBAIKAN MUTLAK: Tambahkan [1] setelah argv agar bertipe const char*
    int Fitur = atoi(argv[1]); 

    {
            {
        char pkg[100] = {0};
        
        // Lewati fungsi isapkrunning, langsung gunakan getpid untuk mendeteksi nama proses sendiri
        char self_proc[64];
        sprintf(self_proc, "/proc/%d/cmdline", getpid());
        FILE* f = fopen(self_proc, "r");
        if (f) {
            fgets(pkg, sizeof(pkg), f);
            fclose(f);
        } else {
            // Jika gagal membaca cmdline, gunakan default aman
            sprintf(pkg, "com.tencent.ig"); 
        }

        char getRoot[100] = {0};
        if (getuid() == 0) {
            sprintf(getRoot, "MODE_ROOT");
        } else {
            sprintf(getRoot, "MODE_NO_ROOT");
        }

        // Jalankan inisialisasi dengan string nama paket yang sudah pasti valid
        ::initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk meluncurkan Game: %s dengan Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
        case 1:
            LOGI("Menjalankan Fitur LOGIKA CASE 1");
            
            // Panggil fungsi memori secara langsung menggunakan scope global (::)
            ::MemorySearch((char*)"220", TYPE_FLOAT);
            ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
            ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
            ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
            break;
            
        case 2:
            LOGI("Menjalankan Fitur LOGIKA CASE 2");
            ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
            ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
            ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
            ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
            break;
            
        default:
            LOGI("Case tidak dikenal atau kosong.");
            break;
        }
    }

    return 0;
}
