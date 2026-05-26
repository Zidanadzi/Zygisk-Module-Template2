#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <sys/inotify.h> // ──> PENGAMAN UTAMA: Menggunakan file watcher Linux

#include "MemoryTools.h" 

#define LOG_TAG "MainCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
//jemboddd
extern int handle;
extern char bm; 

// Fungsi untuk mengeksekusi logika asli MemoryTools Anda
void EksekusiSuntikMemori(int FiturAktif, const char* gamePkg) {
    LOGI("🎯 MATCH TRIGGERED: Mengaktifkan gerbang memori lokal...");
    
    if (gamePkg != nullptr) strncpy(bm, gamePkg, 63);
    handle = open("/proc/self/mem", O_RDWR);
    if (handle == -1) handle = open("/dev/null", O_RDWR);
    lseek(handle, 0, SEEK_SET);

    if (FiturAktif == 1) {
        LOGI("🎯 IN-MATCH: Menyuntikkan CASE 1...");
        ::SetSearchRange(ALL); 
        ::MemorySearch((char*)"220", TYPE_FLOAT);
        usleep(150000);
        ::MemoryOffset((char*)"178", 0x18, TYPE_FLOAT);
        usleep(100000);
        ::MemoryOffset((char*)"15", 0x1C, TYPE_FLOAT);            
        usleep(100000);
        ::MemoryWrite((char*)"600", 0, TYPE_FLOAT);   
        LOGI("✅ CASE 1 BERHASIL DIAKTIFKAN!");
    } 
    else if (FiturAktif == 2) {
        LOGI("🎯 IN-MATCH: Menyuntikkan CASE 2...");
        ::SetSearchRange(ALL);
        ::MemorySearch((char*)"0.05000000075", TYPE_FLOAT);
        usleep(150000);
        ::MemoryOffset((char*)"3.4028235e38", -0x4, TYPE_FLOAT);
        usleep(100000);
        ::MemoryOffset((char*)"8.04061356e-15", 0x48, TYPE_FLOAT);
        uskeep(100000);
        ::MemoryWrite((char*)"200", 0, TYPE_FLOAT);
        LOGI("✅ CASE 2 BERHASIL DIAKTIFKAN!");
    }

    if (handle > 0) {
        close(handle);
        handle = -1;
    }
}

int BukaFiturUtama(int argc, char *argv[], const char* gamePkg) 
{
    // ⚠️ GANTI "id_modul_anda" dengan ID modul Magisk Anda yang terdaftar di module.prop
    std::string path_modul = "/data/adb/modules/template_module";
    std::string path_file = path_modul + "/pilihan.txt";

    LOGI("Zygisk Inotify Engine: Siaga tanpa membebani sistem CPU.");

    // 1. Inisialisasi inotify untuk memantau perubahan file secara native
    int fd = inotify_init();
    if (fd < 0) {
        LOGI("❌ Gagal menginisialisasi inotify");
        return -1;
    }

    // 2. Pasang pengawas khusus pada folder modul (Mendeteksi jika file pilihan.txt dimodifikasi)
    int wd = inotify_add_watch(fd, path_modul.c_str(), IN_MODIFY | IN_CREATE);

    char buffer[BUF_LEN];
    int fitur_terakhir = 0;

    // 3. LOOPING AMAN: Loop ini akan BERHENTI (FREEZE) total dan tidak memakan CPU 
    // sampai Anda mengubah isi file via script .sh
    while (true) {
        // Baris read() di bawah ini akan mengunci thread secara damai (menunggu sinyal teks)
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) break;

        // Jika ada perubahan file pilihan.txt di dalam match
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

        if (FiturAktif != fitur_terakhir && (FiturAktif == 1 || FiturAktif == 2)) {
            // Jalankan fungsi memori murni tanpa me-restart game
            EksekusiSuntikMemori(FiturAktif, gamePkg);
            fitur_terakhir = FiturAktif;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
