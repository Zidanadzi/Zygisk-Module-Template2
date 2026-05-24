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
    // Memastikan argumen penentu case dari Zygisk telah mendarat dengan aman
    if (argc < 2 || argv == nullptr || argv[1] == nullptr) {
        LOGI("Error: Argumen fitur kosong.");
        return -1;
    }

    // PERBAIKAN 1: Menambahkan indeks [1] agar lolos dari error atoi
    int Fitur = atoi(argv[1]); 

    {
        // PERBAIKAN 2: Mengubah menjadi array teks char pkg[100] agar valid untuk fgets dan sprintf
        char pkg[100] = {0}; 
        
        // Membaca file cmdline internal untuk mendeteksi nama package secara otomatis (Universal)
        FILE* f = fopen("/proc/self/cmdline", "r");
        if (f) {
            fgets(pkg, sizeof(pkg), f);
            fclose(f);
        } else {
            // Cadangan darurat jika sistem gagal membaca
            sprintf(pkg, "com.tencent.ig"); 
        }

        // PERBAIKAN 3: Mengubah menjadi array teks char getRoot[100] agar aman
        char getRoot[100] = {0};
        sprintf(getRoot, "MODE_ROOT");

        // Menjalankan inisialisasi tools memori Anda secara aman
        ::initXMemoryTools(pkg, getRoot);
        
        // Memaksa pengiriman string agar lolos dari audit format-security NDK
        LOGI("Zygisk sukses mengunci Game aktif: %s, Mode: %s, Menjalankan Fitur: %d", (const char*)pkg, (const char*)getRoot, Fitur);

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
        } // Batas penutup Switch-Case
        
    } // Batas penutup blok kurung kurawal pencarian nama paket

    return 0;
} // Batas penutup fungsi int main()
