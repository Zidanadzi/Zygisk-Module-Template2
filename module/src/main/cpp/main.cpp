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

// Deklarasi fungsi bawaan dari MemoryTools Anda
extern int isapkrunning(char* pkgName);
extern void initXMemoryTools(char* pkgName, char* rootMode);
extern int SetSearchRange(int type); 
extern void MemorySearch(char* value, int TYPE);
extern void MemoryOffset(char *value, long int offset, int type);
extern void MemoryWrite(char *value, long int offset, int type);

// Fungsi utama menerima lemparan data variabel gamePkg secara langsung dari module.cpp
int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
    if (argc < 2 || argv == nullptr || argv == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // 1. Ambil nilai fitur dari file pilihan.txt Anda
    int Fitur = atoi(argv); 

    // ──> SOLUSI MUTLAK: PENYARING AWAL JIKA FITUR BELUM DIAKTIFKAN <──
    // Jika file kosong, bernilai 0, atau bukan angka 1 & 2, LANGSUNG MATIKAN PROGRAM.
    // Ini menjamin initXMemoryTools TIDAK AKAN PERNAH berjalan dan membuat game crash.
    if (Fitur != 1 && Fitur != 2) {
        LOGI("Modul Mode Siaga: Tidak ada fitur yang diaktifkan. Program selesai.");
        return 0; 
    }

    // Kode di bawah ini HANYA AKAN BERJALAN jika Anda sudah mengetik angka 1 atau 2 di script .sh
    {
        char pkg;
        memset(pkg, 0, sizeof(pkg));
        
        if (gamePkg != nullptr) {
            strncpy(pkg, gamePkg, sizeof(pkg) - 1);
        } else {
            strncpy(pkg, "com.tencent.ig", sizeof(pkg) - 1); 
        }

        char getRoot;
        memset(getRoot, 0, sizeof(getRoot));
        strncpy(getRoot, "MODE_ROOT", sizeof(getRoot) - 1);

        // Inisialisasi hanya dipicu saat fitur benar-benar akan disuntikkan
        ::initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk sukses memicu inisialisasi untuk Fitur: %d", Fitur);

        switch (Fitur)
        {
            case 1:
                LOGI("Menjalankan Fitur LOGIKA CASE 1");
                ::SetSearchRange(1); // Disarankan menggunakan rentang ANON (4) agar aman dari anti-cheat
                ::MemorySearch((char*)"220", TYPE_FLOAT);
                usleep(200000);
                ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
                usleep(100000);
                ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
                usleep(100000);
                ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
                break;
                
            case 2:
                LOGI("Menjalankan Fitur LOGIKA CASE 2");
                ::SetSearchRange(1);
                ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                usleep(200000);
                ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                usleep(100000);
                ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                usleep(100000);
                ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                break;
        } 
    } 

    return 0;
}
