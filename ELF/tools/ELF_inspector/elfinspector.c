#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define  MACHINES 101
#define ABIS 3
#define FILE_TYPES 5
#define P_TYPES 8
#define P_FLAGS 8

int main(int argc, char * argv[]){
    printf("===== ELF INSPECTOR =====\n");
    char * magic = (char *)malloc(sizeof(char) * 4);
    Elf64_Ehdr elf_headers;

    if (argc > 1){
        char * file_path = argv[1];
        FILE * file = fopen(file_path, "rb");
        if (file){        
            fread(magic, 4, 1, file);
            if (strcmp(&magic[1],"ELF") == 0){
                free(magic);
                rewind(file);

                /*Output strings definition*/
                const char * machine_names[MACHINES] = {
                    [0] = "No machine",
                    [1] = "AT&T WE 32100",
                    [2] = "SPARC",
                    [3] = "Intel 80386",
                    [4] = "Motorola 68000",
                    [5] = "Motorola 88000",
                    [6] = "Reserved for future use (was EM_486)",
                    [7] = "Intel 80860",
                    [8] = "MIPS I Architecture",
                    [9] = "IBM System/370 Processor",
                    [10] = "MIPS RS3000 Little-endian",
                    [11] = "Reserved for future use",
                    [12] = "Reserved for future use",
                    [13] = "Reserved for future use",
                    [14] = "Reserved for future use",
                    [15] = "Hewlett-Packard PA-RISC",
                    [16] = "Reserved for future use",
                    [17] = "Fujitsu VPP500",
                    [18] = "Enhanced instruction set SPARC",
                    [19] = "Intel 80960",
                    [20] = "PowerPC",
                    [21] = "64-bit PowerPC",
                    [22] = "IBM System/390 Processor",
                    [23] = "Reserved for future use",
                    [24] = "Reserved for future use",
                    [25] = "Reserved for future use",
                    [26] = "Reserved for future use",
                    [27] = "Reserved for future use",
                    [28] = "Reserved for future use",
                    [29] = "Reserved for future use",
                    [30] = "Reserved for future use",
                    [31] = "Reserved for future use",
                    [32] = "Reserved for future use",
                    [33] = "Reserved for future use",
                    [34] = "Reserved for future use",
                    [35] = "Reserved for future use",
                    [36] = "NEC V800",
                    [37] = "Fujitsu FR20",
                    [38] = "TRW RH-32",
                    [39] = "Motorola RCE",
                    [40] = "Advanced RISC Machines ARM",
                    [41] = "Digital Alpha",
                    [42] = "Hitachi SH",
                    [43] = "SPARC Version 9",
                    [44] = "Siemens TriCore embedded processor",
                    [45] = "Argonaut RISC Core, Argonaut Technologies Inc.",
                    [46] = "Hitachi H8/300",
                    [47] = "Hitachi H8/300H",
                    [48] = "Hitachi H8S",
                    [49] = "Hitachi H8/500",
                    [50] = "Intel IA-64 processor architecture",
                    [51] = "Stanford MIPS-X",
                    [52] = "Motorola ColdFire",
                    [53] = "Motorola M68HC12",
                    [54] = "Fujitsu MMA Multimedia Accelerator",
                    [55] = "Siemens PCP",
                    [56] = "Sony nCPU embedded RISC processor",
                    [57] = "Denso NDR1 microprocessor",
                    [58] = "Motorola Star*Core processor",
                    [59] = "Toyota ME16 processor",
                    [60] = "STMicroelectronics ST100 processor",
                    [61] = "Advanced Logic Corp. TinyJ embedded processor family",
                    [62] = "AMD x86-64 architecture",
                    [63] = "Sony DSP Processor",
                    [64] = "Digital Equipment Corp. PDP-10",
                    [65] = "Digital Equipment Corp. PDP-11",
                    [66] = "Siemens FX66 microcontroller",
                    [67] = "STMicroelectronics ST9+ 8/16 bit microcontroller",
                    [68] = "STMicroelectronics ST7 8-bit microcontroller",
                    [69] = "Motorola MC68HC16 Microcontroller",
                    [70] = "Motorola MC68HC11 Microcontroller",
                    [71] = "Motorola MC68HC08 Microcontroller",
                    [72] = "Motorola MC68HC05 Microcontroller",
                    [73] = "Silicon Graphics SVx",
                    [74] = "STMicroelectronics ST19 8-bit microcontroller",
                    [75] = "Digital VAX",
                    [76] = "Axis Communications 32-bit embedded processor",
                    [77] = "Infineon Technologies 32-bit embedded processor",
                    [78] = "Element 14 64-bit DSP Processor",
                    [79] = "LSI Logic 16-bit DSP Processor",
                    [80] = "Donald Knuth's educational 64-bit processor",
                    [81] = "Harvard University machine-independent object files",
                    [82] = "SiTera Prism",
                    [83] = "Atmel AVR 8-bit microcontroller",
                    [84] = "Fujitsu FR30",
                    [85] = "Mitsubishi D10V",
                    [86] = "Mitsubishi D30V",
                    [87] = "NEC v850",
                    [88] = "Mitsubishi M32R",
                    [89] = "Matsushita MN10300",
                    [90] = "Matsushita MN10200",
                    [91] = "picoJava",
                    [92] = "OpenRISC 32-bit embedded processor",
                    [93] = "ARC Cores Tangent-A5",
                    [94] = "Tensilica Xtensa Architecture",
                    [95] = "Alphamosaic VideoCore processor",
                    [96] = "Thompson Multimedia General Purpose Processor",
                    [97] = "National Semiconductor 32000 series",
                    [98] = "Tenor Network TPC processor",
                    [99] = "Trebia SNP 1000 processor",
                    [100] = "STMicroelectronics (www.st.com) ST200 microcontroller"
                };
                const char * file_type[FILE_TYPES] = {
                    [0] = "ET_NONE - Unkown type\0",
                    [1] = "ET_NONE - Unkown type\0",
                    [2] = "ET_EXEC - Executable no-PIE\0",
                    [3] = "ET_DYN - Shared library (.so or PIE executable)\0",
                    [4] = "ET_CORE - Core dump\0"
                };
                const char * file_class[3] = {"None", "ELF32\0", "ELF64\0"};
                const char * endianness[3] = {"None", "Little endian\0", "Big endian\0"};
                const char * abi[ABIS] = {
                    [0] = "Unix / System V\0",
                    [1] = "Linux\0",
                    [2] = "Solaris\0"
                };

                const char *p_types[P_TYPES] = {
                    [0] = "PT_NULL\0",
                    [1] = "PT_LOAD\0",
                    [2] = "PT_DYNAMIC\0",
                    [3] = "PT_INTERP\0",
                    [4] = "PT_NOTE\0",
                    [5] = "PT_SHLIB\0",
                    [6] = "PT_PHDR\0",
                    [7] = "PT_TLS\0"
                };
                const char * p_flags[P_FLAGS] = {
                    [0] = "None\0",
                    [1] = "X\n",
                    [2] = "W\0",
                    [3] = "WX\0",
                    [4] = "R\0",
                    [5] = "RX\0",
                    [6] = "RW\0",
                    [7] = "RWX\0"
                };


                /* General header reading and displaying */
                fread(&elf_headers, 64, 1, file);                
                printf("\n[+] HEADERS\n");

                printf("    Magic numbers: ");
                for (int c=0; c<4; c++) printf("%x ", elf_headers.e_ident[c]);
                printf(" == ");
                for (int c=0; c<4; c++) printf("%c ", elf_headers.e_ident[c]);
                printf("\n    File Class: ");
                if (elf_headers.e_ident[4] < 1 || elf_headers.e_ident[4] > 2) printf("Unrecognized\n");
                else printf("%s\n", file_class[elf_headers.e_ident[4]]);
                printf("    Endianness: ");
                if (elf_headers.e_ident[5] < 0 || elf_headers.e_ident[5] > 1) printf("Unrecognized\n");
                else printf("%s\n", endianness[elf_headers.e_ident[5]]);
                printf("    Header version: %d\n", elf_headers.e_ident[6]);
                if (elf_headers.e_ident[7] < 0 || elf_headers.e_ident[7] > ABIS) printf("Unrecognized\n");
                else printf("    ABI: %s\n", abi[elf_headers.e_ident[7]]);
                if (elf_headers.e_type < 0 || elf_headers.e_type > FILE_TYPES) printf("Unrecognized\n");
                else printf("    File Type: %s\n", file_type[elf_headers.e_type]);
                if (elf_headers.e_machine < 0 || elf_headers.e_machine >= MACHINES) printf("Unrecognized.\n");
                else printf("    Architecture: %s\n", machine_names[elf_headers.e_machine]);
                printf("    Entry: 0x%lx\n", elf_headers.e_entry);
                printf("\n[+] PROGRAM HEADERS\n");
                
                /*Advance file pointer in e_phoff bytes to reach the Program Header's Table*/
                char buf[elf_headers.e_phoff];
                fread(buf, elf_headers.e_phoff-64, 1, file);

                /* Program Headers reading and displaying */
                if (elf_headers.e_ident[4] == 2) {
                    Elf64_Phdr program_headers;
                    for (int c = 0; c<elf_headers.e_phnum; c++){
                        printf("\n");
                        fread(&program_headers, elf_headers.e_phentsize, 1, file);
                        if (program_headers.p_type < 0 || program_headers.p_type >= P_TYPES) printf("[Unknown type] ");
                        else printf("%s ", p_types[program_headers.p_type]);
                        if (program_headers.p_flags < 0 || program_headers.p_flags >= P_FLAGS) printf("[Unknowm flags] ");
                        else printf("%s ", p_flags[program_headers.p_flags]);
                        printf("vaddr=0x%lx ", program_headers.p_vaddr);
                        printf("memsize=0x%lx\n", program_headers.p_memsz);
                    } 
                }else{
                    Elf32_Phdr program_headers;
                    for (int c = 0; c<elf_headers.e_phnum; c++){
                        fread(&program_headers, elf_headers.e_phentsize, 1, file);
                        if (program_headers.p_type < 0 || program_headers.p_type >= P_TYPES) printf("[Unknown type] ");
                        else printf("%s ", p_types[program_headers.p_type]);
                        if (program_headers.p_flags < 0 || program_headers.p_flags >= P_FLAGS) printf("[Unknowm flags] ");
                        else printf("%s ", p_flags[program_headers.p_flags]);
                        printf("vaddr=0x%x ", program_headers.p_vaddr);
                        printf("memsize=0x%x\n", program_headers.p_memsz);
                    }
                }
            /* Error messages */
            }else{
                printf("Not an ELF file.\nQuiting.\n");
            }
        }else{
            printf("An error occurred while attempting to read \"%s\" file. Quiting.\n", file_path);

        }
    }else{
        printf("Usage: elfinspect [file_path]\n");
        printf("No file path given. Quiting.\n");
    }
    
    return 0;
}
