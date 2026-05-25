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
    if (argc < 2 || argv == nullptr || argv[1] == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    int Fitur = atoi(argv[1]); 

    {
        // PERBAIKAN: Mengunci deklarasi array karakter secara tegas sebesar 128 bita
        char pkg[128];
        memset(pkg, 0, sizeof(pkg));
        
        if (gamePkg != nullptr) {
            strncpy(pkg, gamePkg, sizeof(pkg) - 1);
        } else {
            strncpy(pkg, "com.tencent.ig", sizeof(pkg) - 1); 
        }

        // PERBAIKAN: Mengunci deklarasi array status root sebesar 32 bita
        char getRoot[32];
        memset(getRoot, 0, sizeof(getRoot));
        strncpy(getRoot, "MODE_ROOT", sizeof(getRoot) - 1);

        // Menjalankan inisialisasi tools memori bawaan Anda secara aman
        ::initXMemoryTools(pkg, getRoot);
        
        LOGI("Zygisk sukses mengunci Game aktif: %s, Mode: %s, Menjalankan Fitur: %d", pkg, getRoot, Fitur);

        // Evaluasi Menu Multi-Case Anda
        switch (Fitur)
        {

            case 1:
                LOGI("Menjalankan Fitur LOGIKA CASE 1");
                ::SetSearchRange(1); // Sesuaikan angka 1 dengan tipe range Anda (misal: ALL atau ANON)
                ::MemorySearch((char*)"220", TYPE_FLOAT);
                ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
                ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
                ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
                break;
                
            case 2:
                LOGI("Menjalankan Fitur LOGIKA CASE 2");
                ::SetSearchRange(1);
                ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                break;
                
            default:
                LOGI("Case kosong atau default aktif, membiarkan lobi tetap normal.");
                break;
        } 
    } 

    return 0;
}
