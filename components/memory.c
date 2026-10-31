#include "memory.h"

uint8_t MEMORY[0x1000] = {0};   // Initialize the memory array

int LOAD_ROM(char* location) {
    FILE *fp = fopen(location, "rb");
    if (!fp) {
        printf("Error: Could not open ROM file %s\n", location);
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    long fileLen = ftell(fp);
    if (fileLen < 0) {
        fclose(fp);
        return -1;
    }

    size_t maxLen = sizeof(MEMORY) - ROM_STARTING_LOC; /* 4096 - 512 = 3584 */
    if ((size_t)fileLen > maxLen) {
        printf("Error: ROM too large (%ld bytes, max %zu)\n", fileLen, maxLen);
        fclose(fp);
        return -1;
    }

    rewind(fp);
    size_t read = fread(&MEMORY[ROM_STARTING_LOC], 1, (size_t)fileLen, fp);
    fclose(fp);

    if (read != (size_t)fileLen) {
        printf("Error: Failed to read ROM file %s\n", location);
        return -1;
    }
    return 0;
}