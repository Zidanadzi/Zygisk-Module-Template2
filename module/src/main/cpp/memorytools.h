#pragma once
#include <vector>
#include <fstream>
#include <cmath>
#include <sys/mman.h>
#include <cinttypes> // Mengatasi eror konversi format data sscanf

namespace MemoryTools {
    inline std::vector<std::pair<uintptr_t, uintptr_t>> active_ranges;

    // 1. SET RANGE
    inline void SetRange(const std::string& range_type) {
        active_ranges.clear();
        std::ifstream maps("/proc/self/maps");
        std::string line;
        while (std::getline(maps, line)) {
            if (line.find("rw") != std::string::npos) {
                if (range_type == "ALL" || line.find(range_type) != std::string::npos || 
                   (range_type == "ANONYMOUS" && line.find("/") == std::string::npos)) {
                    uintptr_t start, end;
                    
                    // Menggunakan SCNxPTR agar lolos kompilasi di arsitektur 32-bit & 64-bit
                    if (sscanf(line.c_str(), "%" SCNxPTR "-%" SCNxPTR, &start, &end) == 2) {
                        active_ranges.push_back({start, end});
                    }
                }
            }
        }
    }

    // 2. SEARCH
    inline std::vector<uintptr_t> Search(float value) {
        std::vector<uintptr_t> results;
        for (const auto& range : active_ranges) {
            for (uintptr_t addr = range.first; addr < range.second - sizeof(float); addr += 4) {
                if (std::fabs(*reinterpret_cast<float*>(addr) - value) < 0.001f) results.push_back(addr);
            }
        }
        return results;
    }

    // 3. OFFSET
    inline float Offset(uintptr_t base_address, long offset_bytes) {
        if (base_address == 0) return 0.0f;
        return *reinterpret_cast<float*>(base_address + offset_bytes);
    }

    // 4. WRITE
    inline void Write(uintptr_t address, float new_value) {
        if (address == 0) return;
        uintptr_t page_start = address & ~(PAGE_SIZE - 1);
        mprotect(reinterpret_cast<void*>(page_start), PAGE_SIZE, PROT_READ | PROT_WRITE);
        *reinterpret_cast<float*>(address) = new_value;
    }
}
