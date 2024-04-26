#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

#define CHUNK_SIZE 512 // Adjust as needed
#define DELAY 50000 // Used to see the progress bar for smaller files, since they copy immediately

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
        return 1;
    }

    FILE *src_file = fopen(argv[1], "rb");
    if (src_file == NULL) {
        perror("Error opening source file");
        return 1;
    }

    FILE *dst_file = fopen(argv[2], "wb");
    if (dst_file == NULL) {
        perror("Error opening destination file");
        fclose(src_file);
        return 1;
    }

    fseek(src_file, 0, SEEK_END);
    long file_size = ftell(src_file);
    rewind(src_file);

    long bytes_copied = 0;
    int percentage = 0;
    int last_percentage = -1;

    printf("[*]Copying %s to %s\n", argv[1], argv[2]);

    while (!feof(src_file)) {
        char buffer[CHUNK_SIZE];
        size_t bytes_read = fread(buffer, 1, CHUNK_SIZE, src_file);
        if (bytes_read == 0) {
            break; // No more bytes to read, break from the loop
        }
        fwrite(buffer, 1, bytes_read, dst_file);
        bytes_copied += bytes_read;
        percentage = (int)(((double)bytes_copied / file_size) * 1000); // Calculates the percentage with tenths position included

        // Update progress bar if percentage changes
        if (percentage != last_percentage) {
            printf("\r[*]");
            printf("<");

            int pacman_count = percentage / 100;
            for (int i = 0; i < 10; i++) {
                if (i == pacman_count)
                    printf("%c", (percentage % 100) < 50 ? 'c' : 'C'); // Alternates between small and big pacman
                else if (i < pacman_count)
                    printf("-");
                else 
                    printf("*");
            }
            printf("> (%.1f%%)", (float)percentage / 10.0); // Adjust formatting to include tenths position 
            fflush(stdout);
            last_percentage = percentage;
        }

        // Introduce delay for smaller files
        usleep(DELAY);
    }

    printf("\n[*]Copy completed!\n");

    fclose(src_file);
    fclose(dst_file);
    return 0;
}