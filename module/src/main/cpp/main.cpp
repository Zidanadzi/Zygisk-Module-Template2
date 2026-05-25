#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include <fcntl.h> // Diperlukan untuk membuka berkas memori internal

// Panggil file header MemoryTools asli Anda di sini
#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Deklarasikan variabel global bawaan dari dalam file MemoryTools.h Anda
extern int handle;
extern char bm[64]; 

// Fungsi khusus Zygisk untuk mengaktifkan handle memori lokal (Bypass getPID)
void InisialisasiMemoriZygisk(const char* gamePkg) {
    // 1. Isi variabel nama package internal library Anda
    if (gamePkg != nullptr) {
        strcpy(bm, gamePkg);
    } else {
        strcpy(bm, "com.tencent.ig");
    }

    // 2. Ambil PID proses game tempat Zygisk bersarang saat ini
    pid_t pid = getpid();

    // 3. Buka memori internal game secara lokal lewat jalur subsistem Linux
    char lj[64];
    sprintf(lj, "/proc/%d/mem", pid);
    
    handle = open(lj, O_RDWR);
    if (handle == -1) {
        // Jika pembacaan terhambat, gunakan cadangan deskriptor mandiri
        handle = open("/proc/self/mem", O_RDWR);
    }
    
    if (handle != -1) {
        lseek(handle, 0, SEEK_SET);
        LOGI("✅ Zygisk Memory Handle Berhasil Diaktifkan: ID %d", handle);
    } else {
        LOGI("❌ Gagal mengaktifkan Memory Handle lokal!");
    }
}

int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
    // ⚠️ GANTI "id_modul_anda" dengan ID modul Magisk Anda yang terdaftar di module.prop
    std::string path_file = "/data/adb/modules/template_modul/pilihan.txt";
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

        // Menerima sinyal perubahan angka dari script .sh Anda di tengah pertandingan
        if (FiturAktif != fitur_terakhir) {
            
            if (FiturAktif == 1) {
                LOGI("🎯 MATCH STARTED: Membuka gerbang memori...");
                
                // Memicu inisialisasi lokal versi Zygisk agar handle terisi angka valid
                InisialisasiMemoriZygisk(gamePkg);
                usleep(200000); // Jeda singkat agar handle siap

                LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 1...");
                
                // ──> LOGIKA ASLI ANDA DIKEMBALIKAN UTUH DI SINI <──
                ::SetSearchRange(ALL); 
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
                LOGI("🎯 MATCH STARTED: Membuka gerbang memori...");
                
                InisialisasiMemoriZygisk(gamePkg);
                usleep(200000);

                LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 2...");
                
                // ──> LOGIKA ASLI CASE 2 ANDA DIKEMBALIKAN UTUH DI SINI <──
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
            else if (FiturAktif == 0) {
                LOGI("🔄 Semua fitur dinonaktifkan.");
                if (handle > 0) {
                    close(handle);
                    handle = -1;
                }
            }

            fitur_terakhir = FiturAktif; 
        }

        // Memeriksa pembaruan file teks script .sh setiap 2 detik
        sleep(2); 
    }

    return 0;
}
