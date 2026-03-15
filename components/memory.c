#include "memory.h"

uint8_t MEMORY[0x1000] = {0};

int LOAD_ROM(char* location) {
    FILE *fp = fopen(location, "rb");
    
    if (!fp) {
        printf("Error: Could not open ROM file %s\n", location);
        return -1;
    }

    // Get the file length
    fseek(fp, 0, SEEK_END);
    size_t fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fread(&(MEMORY[ROM_STARTING_LOC]), sizeof(uint8_t), fileLen, fp);
    return 0;
}
