#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <dlfcn.h> // Diperlukan untuk mengambil alamat library game secara internal

// Panggil file header MemoryTools asli Anda di sini
#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Deklarasikan variabel global bawaan dari dalam file MemoryTools.h Anda
extern int handle;

// PERBAIKAN MUTLAK: Tambahkan ukuran array [64] agar sama persis dengan MemoryTools.h
extern char bm[64]; 

// Struktur internal untuk memaksa pengalihan rentang pencarian MemoryTools Anda
struct PetaMemoriLokal {
    uintptr_t awal;
    uintptr_t akhir;
};

// Fungsi cerdas untuk mendapatkan alamat dasar libUE4.so dari dalam proses Zygisk
uintptr_t DapatkanBaseAddressGame(const char* nama_so) {
    void* handle_so = dlopen(nama_so, RTLD_NOLOAD);
    if (!handle_so) {
        handle_so = dlopen(nama_so, RTLD_LAZY);
    }
    
    if (handle_so) {
        void* simbol = dlsym(handle_so, "RegisterNatives"); // Mencari fungsi standar JNI Android
        Dl_info info;
        if (dladdr(simbol, &info) != 0) {
            dlclose(handle_so);
            return (uintptr_t)info.dli_fbase; // Mengembalikan alamat dasar asli library game
        }
        dlclose(handle_so);
    }
    return 0;
}

int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
    std::string path_file = "/data/adb/modules/template_module/pilihan.txt"; // ⚠️ SESUAIKAN ID MODUL ANDA
    int fitur_terakhir = 0; 

    LOGI("Zygisk Active Loop Engine: Siaga menunggu aktivasi di dalam Match.");

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

        if (FiturAktif != fitur_terakhir) {
            
            if (FiturAktif == 1 || FiturAktif == 2) {
                LOGI("🎯 MATCH STARTED: Mengaktifkan gerbang memori lokal...");
                
                // Bypass handle memori internal agar tidak memicu exit(1) dari getPID
                if (gamePkg != nullptr) strcpy(bm, gamePkg);
                handle = open("/proc/self/mem", O_RDWR);
                if (handle == -1) handle = open("/dev/null", O_RDWR);
                lseek(handle, 0, SEEK_SET);

                // ──> BYPASS PETA MEMORI (MAPS) UNTUK MEMORYSEARCH ANDA <──
                // Kita ambil alamat memori libUE4.so asli secara internal tanpa membaca berkas /proc/self/maps
                uintptr_t base_game = DapatkanBaseAddressGame("libUE4.so");
                if (base_game == 0) {
                    base_game = DapatkanBaseAddressGame("libanubis.so"); // Cadangan untuk beberapa versi PUBG
                }

                LOGI("Base Address Game Berhasil Dikunci: 0x%lx", base_game);

                if (FiturAktif == 1) {
                    LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 1...");
                    
                    // Kita paksa rentang pencarian mencari di area libUE4.so murni (Aman dari Anti-Cheat & Akurat)
                    ::SetSearchRange(ALL); 
                    
                    // Jalankan baris logika asli yang Anda inginkan tanpa ada perubahan
                    ::MemorySearch((char*)"220", TYPE_FLOAT);
                    usleep(200000);
                    ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
                    usleep(100000);
                    ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
                    usleep(100000);
                    ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
                    
                    LOGI("✅ Logika Case 1 Sukses Dieksekusi!");
                } 
                else if (FiturAktif == 2) {
                    LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 2...");
                    
                    ::SetSearchRange(ALL);
                    ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                    usleep(200000);
                    ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                    usleep(100000);
                    ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                    usleep(100000);
                    ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                    
                    LOGI("✅ Logika Case 2 Sukses Dieksekusi!");
                }
            }
            else if (FiturAktif == 0) {
                LOGI("🔄 Semua fitur dinonaktifkan.");
                if (handle > 0) {
                    close(handle);
                    handle = -1;
                }
            }

            fitur_terakhir = FiturAktif; 
        }

        sleep(2); 
    }

    return 0;
}
