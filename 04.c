// 04.c

#include <stdio.h>
#include <stdbool.h> // bool
#include <stdlib.h>  // stroll

int check_if_error(int result, int error_cond, char error_message[]) {
    if (result == error_cond) {
        printf("%s", error_message);
        return 1;
    }
    return 0;
}

int try_to_close_file(FILE *fp) {
    int closing_result = fclose(fp);
    return check_if_error(closing_result, EOF, "Error while closing the file!\n");
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Output mode value was not supplied!\n");
        return 1;
    }
    int output_mode = (int)strtol(argv[2], NULL, 10);
    printf("Output mode - %d\n", output_mode);

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error while opening the file!\n");
        return 1;
    }

    bool no_end_of_file = true;
    int line_counter = 0;

    while (no_end_of_file) {

        int c = fgetc(fp);
        if (c == EOF) {
            no_end_of_file = false;
            continue;
        }
        if (c == '\n') {
            line_counter++;
        }

        fputc(c, stdout);

        if ((output_mode != 0) && (line_counter == output_mode)) {
            line_counter = 0;
            fgetc(stdin);
        }
    }

    try_to_close_file(fp);
    return 0;
}
