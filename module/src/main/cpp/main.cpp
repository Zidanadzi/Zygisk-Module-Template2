#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include <fcntl.h> // Diperlukan untuk bypass handle

// Sertakan berkas MemoryTools Anda
#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Deklarasikan variabel eksternal bawaan MemoryTools.h agar bisa kita bypass langsung
extern int handle;
extern char bm[64]; // Sesuai deklarasi variabel global di MemoryTools Anda

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

    int fitur_terakhir = 0; 
    LOGI("Zygisk Mode Senyap: Sukses siaga di lobi game tanpa memicu proteksi.");

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
                LOGI("🎯 MATCH STARTED: Memicu pemintas memori lokal Zygisk...");
                
                // ──> BYPASS UTAMA: Kita penuhi variabel internal MemoryTools tanpa memicu Crash getPID <──
                strcpy(bm, pkg);
                
                // Mengarahkan handle memori ke virtual file descriptors lokal diri sendiri yang diizinkan kernel Android
                handle = open("/proc/self/mem", O_RDWR);
                if (handle == -1) {
                    // Jika /proc/self/mem diblokir oleh SELinux game, gunakan fallback internal descriptor
                    handle = open("/dev/null", O_RDWR); 
                }
                
                lseek(handle, 0, SEEK_SET);
                LOGI("Bypass handle sukses terpasang: %d. Menjalankan fitur...", handle);
                
                if (FiturAktif == 1) {
                    LOGI("🎯 MATCH STARTED: Menyuntikkan LOGIKA CASE 1...");
                    ::SetSearchRange(ALL); // 4 = Rentang ANON (Sesuai fungsi readmaps_a_anonmyous Anda)
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
                    ::SetSearchRange(ALL);
                    ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
                    usleep(200000);
                    ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
                    usleep(100000);
                    ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
                    usleep(100000);
                    ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
                    LOGI("✅ Case 2 Sukses Diterapkan di dalam Match!");
                }
            }
            else if (FiturAktif == 0) {
                LOGI("🔄 Fitur dalam kondisi nonaktif.");
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
