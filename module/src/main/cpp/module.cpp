#include <zygisk.hpp>
#include <string.h>

// Baris ini memberi tahu Zygisk bahwa ada fungsi 'main' di file main.cpp Anda
extern int main(int argc, char *argv[]);

class PerantaraZygisk : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    // Fungsi ini otomatis berjalan setiap kali ada aplikasi Android yang dibuka
    void postAppSpecialize(const AppSpecializeArgs *args) override {
        
        // Zygisk secara otomatis mengintip nama aplikasi yang sedang dibuka
        const char *processName = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (processName) {
            // Karena main.cpp Anda butuh input 'argv[1]', kita buatkan isinya di sini.
            // Angka "1" di bawah ini adalah nilai untuk 'Fitur = atoi(argv[1])' di kode Anda.
            char* arg_fitur = (char*)"1"; 

            // Kita susun argumennya agar bisa dibaca oleh main.cpp Anda
            char* susunan_argv[] = {
                (char*)"modul_zygisk", // argv[0] (nama program, bebas)
                arg_fitur,             // argv[1] (nomor fitur yang dibaca atoi)
                nullptr
            };
            
            int jumlah_argc = 2; // Jumlah argumen yang dikirim
            
            // SEKARANG KITA PANGGIL KODE ANDA!
            // Zygisk langsung melompat dan menjalankan fungsi main(argc, argv) di main.cpp Anda
            main(jumlah_argc, susunan_argv);
        }
        
        env->ReleaseStringUTFChars(args->nice_name, processName);
    }
};

REGISTER_ZYGISK_MODULE(PerantaraZygisk)
