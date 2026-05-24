#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include "MemoryTools.h" // Memuat file MemoryTools eksternal Anda

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

// Deklarasi fungsi eksternal global dari MemoryTools Anda
extern int isapkrunning(char* pkgName);
extern void initXMemoryTools(char* pkgName, char* rootMode);

// Pustaka fungsi pencarian memori global Anda
extern int SetSearchRange(int type);
extern void MemorySearch(char* value, int TYPE);
extern void MemoryOffset(char *value, long int offset, int type);
extern void MemoryWrite(char *value, long int offset, int type);

int main(int argc, char *argv[]) 
{
    // Memastikan argumen argv dan isinya tersedia sebelum diproses
    if (argc < 2 || argv == nullptr || argv[1] == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // Membaca pilihan fitur dari argumen Zygisk dengan indeks array yang sah
    int Fitur = atoi(argv[1]); 

    // Blok pencarian nama paket otomatis
    {
        char pkg[128] = {0}; 
        
        // Membaca file cmdline internal untuk mendeteksi nama package secara otomatis
        FILE* f = fopen("/proc/self/cmdline", "r");
        if (f) {
            fgets(pkg, sizeof(pkg), f);
            fclose(f);
        } else {
            // Cadangan darurat jika file sistem tidak terbaca
            sprintf(pkg, "com.tencent.ig"); 
        }

        char getRoot[32] = {0};
        if (getuid() == 0) {
            sprintf(getRoot, "MODE_ROOT");
        } else {
            sprintf(getRoot, "MODE_NO_ROOT");
        }

        // Jalankan inisialisasi tools memori Anda secara aman
        ::initXMemoryTools(pkg, getRoot);
        
        // PERBAIKAN MAKRO LOGI: Memaksa pengiriman string agar lolos dari audit format-security NDK
        LOGI("Zygisk sukses mendeteksi Game aktif: %s, Mode: %s, Fitur: %d", (const char*)pkg, (const char*)getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {
            case 1:
                LOGI("Menjalankan Fitur LOGIKA CASE 1");
                ::SetSearchRange(ALL); // Sesuaikan angka 1 dengan tipe range Anda (misal: ALL atau ANON)
                ::MemorySearch((char*)"220", TYPE_FLOAT);
                ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
                ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
                ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
                break;
                
            case 2:
                LOGI("Menjalankan Fitur LOGIKA CASE 2");
                ::SetSearchRange(ALL);
                ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                break;
                
            default:
                LOGI("Case tidak dikenal atau kosong.");
                break;
        } // Batas penutup Switch-Case
        
    } // Batas penutup blok kurung kurawal pencarian nama paket

    return 0;
} // Batas penutup fungsi int main()
