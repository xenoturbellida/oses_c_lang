// 05.c

#include <stdio.h>
#include <sys/types.h> // mode_t
#include <sys/stat.h>  // stat
#include <errno.h>     // errno
#include <stdbool.h>   // bool
#include <string.h>    // strerror

extern int errno;

int const ERR_BUFFER_SIZE = 127;

int check_if_error(char err_prefix[], int err_num) {
    if (err_num != 0) {
        printf("%s : %s\n", err_prefix, strerror(err_num));
        return 1;
    }
    return 0;
}

FILE* try_to_open_file(char filename[], char mode[]) {
    FILE *fp = fopen(filename, mode);
    int err_value = errno;
    char err_message_buf[ERR_BUFFER_SIZE];
    sprintf(err_message_buf, "Error while opening file %s", filename);
    check_if_error(err_message_buf, err_value);
    return fp;
}

int try_to_close_file(FILE *fp, char err_message[]) {
    fclose(fp);
    int err_value = errno;
    return check_if_error(err_message, err_value);
}

int try_to_close_both(FILE *fp_src, FILE *fp_dest) {
    int err_while_closing_src = try_to_close_file(fp_src, "Error while closing source file");
    int err_while_closing_dest = try_to_close_file(fp_dest, "Error while closing destination file");
    return err_while_closing_src | err_while_closing_dest;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("File name where to copy was not supplied!\n");
        return 1;
    }

    FILE* src_file_ptr = try_to_open_file(argv[1], "r");
    FILE* dest_file_ptr = try_to_open_file(argv[2], "w");
    if (src_file_ptr == NULL || dest_file_ptr == NULL) {
        return 1;
    }

    char err_message_buf[ERR_BUFFER_SIZE];
    int err_value;

    bool no_end_of_file = true;
    while (no_end_of_file) {
        int c = fgetc(src_file_ptr);
        err_value = errno;
        sprintf(err_message_buf, "Error while reading file %s", argv[1]);
        if (check_if_error(err_message_buf, err_value) == 1) {
            try_to_close_both(src_file_ptr, dest_file_ptr);
            return 1;
        }

        if (c == EOF) {
            no_end_of_file = false;
            continue;
        }

        fputc(c, dest_file_ptr);
        err_value = errno;
        sprintf(err_message_buf, "Error while writing to file %s", argv[2]);
        if (check_if_error(err_message_buf, err_value)) {
            try_to_close_both(src_file_ptr, dest_file_ptr);
            return 1;
        }
    }

    struct stat buf;
    stat(argv[1], &buf);
    err_value = errno;
    sprintf(err_message_buf, "Error while getting properties of file %s", argv[1]);
    if (check_if_error(err_message_buf, err_value) == 1) {
        try_to_close_both(src_file_ptr, dest_file_ptr);
        return 1;
    };

    mode_t source_file_mode = buf.st_mode;
    chmod(argv[2], source_file_mode);
    err_value = errno;
    sprintf(err_message_buf, "Error while changing mode of file %s", argv[2]);
    check_if_error(err_message_buf, err_value);

    return try_to_close_both(src_file_ptr, dest_file_ptr);
}
