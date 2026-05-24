#ifndef MEMORY_TOOLS_H
#define MEMORY_TOOLS_H

#pragma once
#include <vector>
#include <fstream>
#include <cmath>
#include <sys/mman.h>
#include <cinttypes> 
#include <unistd.h> 

namespace MemoryTools {
    inline std::vector<std::pair<uintptr_t, uintptr_t>> active_ranges;

    // 1. SET RANGE: Membaca tabel memori secara absolut menggunakan PID Proses aktif
    inline void SetRange(const std::string& range_type) {
        active_ranges.clear();
        
        // FIX: Mengubah menjadi array char [64] agar menyediakan ruang memori yang cukup dan aman
        char maps_path[64];
        sprintf(maps_path, "/proc/%u/maps", static_cast<unsigned int>(getpid()));
        
        std::ifstream maps(maps_path);
        std::string line;
        
        while (std::getline(maps, line)) {
            // Saring memori Read-Write (rw)
            if (line.find("rw-p") != std::string::npos || line.find("rw-s") != std::string::npos) {
                
                if (range_type == "ALL" || line.find(range_type) != std::string::npos || 
                   (range_type == "ANONYMOUS" && line.find("/") == std::string::npos && line.find("[") == std::string::npos)) {
                    
                    uintptr_t start, end;
                    if (sscanf(line.c_str(), "%" SCNxPTR "-%" SCNxPTR, &start, &end) == 2) {
                        active_ranges.push_back({start, end});
                    }
                }
            }
        }
    }

    // 2. SEARCH: Pemindaian nilai float dengan pembatas validasi alamat
    inline std::vector<uintptr_t> Search(float value) {
        std::vector<uintptr_t> results;
        for (const auto& range : active_ranges) {
            if (range.first == 0 || range.second == 0 || range.first >= range.second) continue;
            
            for (uintptr_t addr = range.first; addr < range.second - sizeof(float); addr += 4) {
                float* ptr = reinterpret_cast<float*>(addr);
                if (std::fabs(*ptr - value) < 0.001f) {
                    results.push_back(addr);
                }
            }
        }
        return results;
    }

    // 3. OFFSET: Membaca nilai float pada jarak offset tertentu
    inline float Offset(uintptr_t base_address, long offset_bytes) {
        if (base_address == 0) return 0.0f;
        return *reinterpret_cast<float*>(base_address + offset_bytes);
    }

    // 4. WRITE: Menulis nilai baru dengan membuka proteksi memori penuh (RWX)
    inline void Write(uintptr_t address, float new_value) {
        if (address == 0) return;
        uintptr_t page_start = address & ~(PAGE_SIZE - 1);
        
        // Membuka izin PROT_EXEC di samping PROT_WRITE agar mesin rendering visual game langsung menerimanya
        mprotect(reinterpret_cast<void*>(page_start), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
        *reinterpret_cast<float*>(address) = new_value;
    }
}

#endif // MEMORY_TOOLS_H

