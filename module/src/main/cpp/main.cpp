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
extern void MemorySearch(char* value, int type);
extern void MemoryOffset(char* value, long offset, int type);
extern void MemoryWrite(char* value, long offset, int type);

int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
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

    // 1. Jalankan inisialisasi awal SEKALI SAJA agar siap digunakan kapan saja
    ::initXMemoryTools(pkg, getRoot);
    LOGI("Zygisk siap. Memulai pemantauan fitur di dalam match...");

    // Tentukan jalur file teks yang sama dengan module.cpp
    // ⚠️ GANTI "id_modul_anda" dengan ID modul Magisk Anda
    std::string path_file = "/data/adb/modules/template_modul/pilihan.txt";

    // Variabel untuk mengingat status fitur terakhir agar tidak terjadi injeksi berulang-ulang
    int fitur_terakhir = 0; 

    // 2. LOOPING SEUMUR HIDUP GAME: Terus memantau file pilihan.txt secara senyap
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

        // Jika Anda mengaktifkan fitur lewat script .sh saat match dimulai
        if (FiturAktif != fitur_terakhir) {
            
            if (FiturAktif == 1) {
                LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 1...");
                ::SetSearchRange(1); // Rentang ANON aman
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
                LOGI("🔄 Fitur dimatikan / dikosongkan.");
            }

            // Catat perubahan agar fungsi pencarian tidak berjalan terus-menerus (bikin crash)
            fitur_terakhir = FiturAktif; 
        }

        // Jeda 2 detik setiap kali memeriksa file teks agar hemat baterai dan tidak membebani CPU
        sleep(2); 
    }

    return 0;
}
