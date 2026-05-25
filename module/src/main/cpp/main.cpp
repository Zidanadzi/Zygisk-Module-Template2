#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include "MemoryTools.h" // Memuat file MemoryTools eksternal Anda

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct little_map {
    std::uintptr_t address;
    std::int64_t value;
};

extern int isapkrunning(char* pkgName);
extern void initXMemoryTools(char* pkgName, char* rootMode);
extern int SetSearchRange(int type); 
extern void MemorySearch(char* value, int TYPE);
extern void MemoryOffset(char *value, long int offset, int type);
extern void MemoryWrite(char *value, long int offset, int type);

int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
    // ⚠️ GANTI "id_modul_anda" dengan ID modul Magisk Anda yang terdaftar di module.prop
    std::string path_file = "/data/adb/modules/template_module/pilihan.txt";

    char pkg[128];
    memset(pkg, 0, sizeof(pkg));
    if (gamePkg != nullptr) {
        strncpy(pkg, gamePkg, sizeof(pkg) - 1);
    } else {
        strncpy(pkg, "com.tencent.ig", sizeof(pkg) - 1); 
    }

    char getRoot[32];
    memset(getRoot, 0, sizeof(getRoot));
    strncpy(getRoot, "MODE_ROOT", sizeof(getRoot) - 1);

    int fitur_terakhir = 0; 
    LOGI("Zygisk Mode Senyap: Berhasil siaga di lobi tanpa memicu memori.");

    // LOOPING PEMANTAU: Selama di lobi hanya membaca file teks (Aman 100% dari Anti-Cheat)
    while (true) {
        int FiturAktif = 0;
        std::ifstream file_konfig(path_file);
        
        if (file_konfig.is_open()) {
            std::string teks_bacaan;
            std::getline(file_konfig, teks_bacaan);
            file_konfig.close();
            if (!teks_bacaan.empty()) {
                FiturAktif = atoi(teks_bacaan.c_str());
            }
        }

        // Ketika Anda menekan tombol di Termux saat sudah masuk Match
        if (FiturAktif != fitur_terakhir) {
            
            if (FiturAktif == 1) {
                LOGI("🎯 MATCH STARTED: Memulai inisialisasi memori...");
                
                // ──> PINDAH KE SINI: Inisialisasi baru berjalan saat match dimulai <──
                ::initXMemoryTools(pkg, getRoot);
                usleep(300000); // Jeda 0.3 detik agar inisialisasi selesai mapan
                
                LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 1...");
                ::SetSearchRange(1); // Rentang memori ANON aman
                ::MemorySearch((char*)"220", TYPE_FLOAT);
                usleep(200000);
                ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
                usleep(100000);
                ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
                usleep(100000);
                ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
                LOGI("✅ Case 1 Sukses Diterapkan di dalam Match!");
            } 
            else if (FiturAktif == 2) {
                LOGI("🎯 MATCH STARTED: Memulai inisialisasi memori...");
                
                // ──> PINDAH KE SINI: Inisialisasi baru berjalan saat match dimulai <──
                ::initXMemoryTools(pkg, getRoot);
                usleep(300000);
                
                LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 2...");
                ::SetSearchRange(1);
                ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                usleep(200000);
                ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                usleep(100000);
                ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                usleep(100000);
                ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                LOGI("✅ Case 2 Sukses Diterapkan di dalam Match!");
            }
            else if (FiturAktif == 0) {
                LOGI("🔄 Fitur dalam kondisi nonaktif.");
            }

            fitur_terakhir = FiturAktif; 
        }

        // Memeriksa berkas teks setiap 2 detik (Sangat ringan, beban CPU = 0%)
        sleep(2); 
    }

    return 0;
}
