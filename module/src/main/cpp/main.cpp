#include "MemoryTools.h"
struct little_map
{
    std::uintptr_t address;
    std::int64_t value;
};
int main(int argc, char *argv[])
{
    int Fitur = atoi(argv[1]);
    {
        char pkg[100];
        if (isapkrunning("com.tencent.ig") == 1)
        {
            sprintf(pkg, "com.tencent.ig");
        }
        else if (isapkrunning("com.vng.pubgmobile") == 1)
        {
            sprintf(pkg, "com.vng.pubgmobile");
        }
        else if (isapkrunning("com.pubg.krmobile") == 1)
        {
            sprintf(pkg, "com.pubg.krmobile");
        }
        else if (isapkrunning("com.rekoo.pubgm") == 1)
        {
            sprintf(pkg, "com.rekoo.pubgm");
        }

        char getRoot[100];
        if (getuid() == 0) {
            sprintf(getRoot, "MODE_ROOT");
        }
        else {
            sprintf(getRoot, "MODE_NO_ROOT");
        }

        initXMemoryTools(pkg, getRoot);
        switch (Fitur)
        {
        case 1:
            //FITUR
            SetSearchRange(ALL); //wide
            MemorySearch("220", TYPE_FLOAT);
            MemoryOffset("178", 0x18, TYPE_FLOAT);
            MemoryOffset("15", 0x1C, TYPE_FLOAT);            
            MemoryWrite("600", 0, TYPE_FLOAT);
            ClearResults();            
           
            SetSearchRange(ALL); //black sky
            MemorySearch("0.05000000075", TYPE_FLOAT);
            MemoryOffset("3.4028235e38", -0x4, TYPE_FLOAT);
            MemoryOffset("8.04061356e-15", 0x48, TYPE_FLOAT);
            MemoryWrite("200", 0, TYPE_FLOAT);
            ClearResults();      
             
            SetSearchRange(ALL); //wh
            MemorySearch("2", TYPE_FLOAT);
            MemoryOffset("4.20389539e-45", -0x88, TYPE_FLOAT);
            MemoryOffset("2.29756896e-41", 0x30, TYPE_FLOAT);
            MemoryOffset("5.60519386e-45", 0x64, TYPE_FLOAT);
            MemoryWrite("120", 0, TYPE_FLOAT);
            ClearResults();            
            
            SetSearchRange(ALL); //wh
            MemorySearch("2", TYPE_FLOAT);
            MemoryOffset("1.90576591e-43", -0x20, TYPE_FLOAT);
            MemoryOffset("3.36311631e-44", -0x18, TYPE_FLOAT);
            MemoryOffset("3.50324616e-44", 0x84, TYPE_FLOAT);
            MemoryWrite("120", 0, TYPE_FLOAT);
            ClearResults();            
            
            SetSearchRange(ALL); //bc
            MemorySearch("8200", TYPE_DWORD);
            MemoryOffset("8204", -0x8, TYPE_DWORD);
            MemoryOffset("8199", -0x10, TYPE_DWORD); 
            MemoryOffset("8196", -0x18, TYPE_DWORD);
            MemoryWrite("6", 0, TYPE_DWORD);
            ClearResults();           
                    
            SetSearchRange(ALL); //car1
            MemorySearch("8200", TYPE_DWORD);
            MemoryOffset("121", -0x18, TYPE_DWORD);
            MemoryOffset("8192", -0x10, TYPE_DWORD); 
            MemoryOffset("8196", -0x8, TYPE_DWORD);
            MemoryOffset("8204", 0x8, TYPE_DWORD);
            MemoryWrite("7", 0, TYPE_DWORD);
            ClearResults();      
                             
            SetSearchRange(ALL); //car2
            MemorySearch("8200", TYPE_DWORD);
            MemoryOffset("169", -0x18, TYPE_DWORD);
            MemoryOffset("8192", -0x10, TYPE_DWORD); 
            MemoryOffset("8196", -0x8, TYPE_DWORD);
            MemoryWrite("7", 0, TYPE_DWORD);
            ClearResults();           
                             
                            
            break;
        case 2:
            //FITUR                                    
            break;
        }
    }
}
