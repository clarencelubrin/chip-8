#include "memory.h"

uint8_t MEMORY[0x1000] = {0};   // Initialize the memory array

int LOAD_ROM(char* location) {
    FILE *fp = fopen(location, "rb");                                   // Open the file from the file location
    if (!fp) {
        printf("Error: Could not open ROM file %s\n", location);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t fileLen = ftell(fp);                                         // Get the file length
    fseek(fp, 0, SEEK_SET);

    fread(&(MEMORY[ROM_STARTING_LOC]), sizeof(uint8_t), fileLen, fp);   // Load the file binary to the memory array
    return 0;
}
