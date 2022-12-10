// 03.c

#include <stdio.h>
#include <signal.h>

static volatile int keep_running = 1;

void interruption_handler(int sig) {
    keep_running = 0;
}

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

    if (argc < 2) {
        printf("File name to write was not supplied!\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "w");
    if (fp == NULL) {
        printf("Error while opening the file!\n");
        return 1;
    }

    signal(SIGINT, interruption_handler);

    printf("Enter Ctrl+C, then Enter to save and exit\n");

    while (keep_running) {
        int c = fgetc(stdin);
        if (check_if_error(c, EOF, "Error when reading from console!\n") != 0) {
            return 1;
        }
        int writing_result = fputc(c, fp);
        if (check_if_error(writing_result, EOF, "Error while writing to the file!\n") != 0) {
            try_to_close_file(fp);
            return 1;
        }
    }

    return try_to_close_file(fp);
}
