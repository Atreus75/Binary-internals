#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){
    printf("===== ELF INSPECTOR =====\n");
    char * magic = (char *)malloc(sizeof(char) * 4);

    if (argc > 1){
        char * file_path = argv[1];
        FILE * file = fopen(file_path, "rb");
        if (file){        
            fread(magic, 4, 1, file);
            if (strcmp(&magic[1],"ELF") == 0){
                int file_class = 0, endianness = 0, header_version = 0, abi = 0, abi_version = 0;
                fread(&file_class, 1, 1, file);
                fread(&endianness, 1, 1, file);
                fread(&header_version, 1, 1, file);
                fread(&abi, 1, 1, file);
                fread(&abi_version, 1, 1, file);

                printf("    Magic numbers: 0x%x 0x%x 0x%x 0x%x == 0x%x %c %c %c\n", (unsigned char)magic[0], (unsigned char)magic[1], (unsigned char)magic[2], (unsigned char)magic[3], (unsigned char)magic[0], magic[1], magic[2], magic[3]);
                printf("    File Class: ");
                if (file_class == 1) printf("ELF32\n");
                else printf("ELF64\n");
                printf("    Endianness: ");
                if (endianness == 1) printf("Little endian\n");
                else printf("Big endian\n");
                printf("    Header version: %d\n", header_version);
                printf("    ABI: ");
                if (!abi) printf("System V\n");
                else if (abi == 3) printf("Linux\n");
                else if (abi_version == 6) printf("Solaris\n");
                else printf("%d - Not known\n", abi_version);
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

    
    free(magic);
    return 0;
}


