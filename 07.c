// 07.c

#include <stdio.h>
#include <stdlib.h>   // strtol
#include <dirent.h>   // dirent, readdir
#include <limits.h>   // PATH_MAX
#include <errno.h>    // errno
#include <string.h>   // strcmp, strcat
#include <sys/stat.h> // stat
#include <stdbool.h>  // bool
#include <time.h>

int ERR_BUFFER_SIZE = 256;

extern int errno;

int check_if_error(char err_prefix[]) {
    if (errno != 0) {
        perror(err_prefix);
        return 1;
    }
    return 0;
}

int try_to_close_dir(DIR *dir_ptr) {
    closedir(dir_ptr);
    return check_if_error("Error while closing directory");
}

int write_line_to_file(FILE* f_ptr, char line[]) {
    int str_len = strlen(line);
    for (int i = 0; i < str_len; i++) {
        fputc(line[i], f_ptr);
    }
    if (check_if_error("Error while writing to file")) {
        return 1;
    }
    return 0;
}

bool check_if_satisfies(struct stat properties, off_t n1, off_t n2, time_t m1, time_t m2) {
    if (properties.st_size > n1 && properties.st_size < n2
        && m1 < properties.st_mtime && properties.st_mtime < m2) {
        return true;
    }
    return false;
}

int print_dir_content(char path[], FILE* f_ptr, off_t n1, off_t n2, time_t m1, time_t m2) {
    DIR *dir_ptr = opendir(path);

    char err_message_buf[ERR_BUFFER_SIZE];
    sprintf(err_message_buf, "Error while opening directory %s", path);
    if (check_if_error(err_message_buf) != 0) {
        return 1;
    }

    for(;;) {
        struct dirent* dirent_ptr = readdir(dir_ptr);

        if (dirent_ptr == NULL) {
            sprintf(err_message_buf, "Error while reading directory %s", path);
            if (check_if_error(err_message_buf) != 0) {
                try_to_close_dir(dir_ptr);
                return 1;
            }
            break;
        }

        if ((strcmp(dirent_ptr->d_name, ".") == 0) ||
            (strcmp(dirent_ptr->d_name, "..") == 0)) {
            continue;
        }

        struct stat properties;
        char entry_name[PATH_MAX];
        strcpy(entry_name, path);
        if (entry_name[strlen(entry_name) - 1] != '/') {
            strcat(entry_name, "/");
        }
        strcat(entry_name, dirent_ptr->d_name);

        if (stat(entry_name, &properties) < 0) {
            continue;
        }

        if (S_ISDIR(properties.st_mode)) {
            print_dir_content(entry_name, f_ptr, n1, n2, m1, m2);
        } else if (check_if_satisfies(properties, n1, n2, m1, m2) == true) {
            char line_to_write[PATH_MAX + 128];
            struct tm* modification_time;
            modification_time = localtime(&properties.st_mtime);
            sprintf(line_to_write, "%s\t%zu\t%s\n",
                    entry_name, properties.st_size, asctime(modification_time));
            printf("%s", line_to_write);
            write_line_to_file(f_ptr, line_to_write);
        }
    }

    try_to_close_dir(dir_ptr);
    return 0;
}

time_t get_time_from_string(char string[]) {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;

    sscanf(string, "%d.%d.%d %d:%d:%d", &day, &month, &year, &hour, &minute, &sec);

    struct tm local_time = {
            .tm_sec  = sec,
            .tm_min  = minute,
            .tm_hour = hour,
            .tm_mday = day,
            .tm_mon  = month - 1,
            .tm_year = year - 1900,
    };

    time_t calendar_time = mktime(&local_time);
    return calendar_time;
}

bool check_if_args_were_supplied(int argc) {
    bool all_args_were_supplied = true;
    if (argc < 2) {
        printf("Directory name was not supplied!\n");
        all_args_were_supplied = false;
    }
    else if (argc < 3) {
        printf("File name where to write was not supplied!\n");
        all_args_were_supplied = false;
    }
    else if (argc < 5) {
        printf("Size borders were not supplied!\n");
        all_args_were_supplied = false;
    }
    else if (argc < 7) {
        printf("Time borders were not supplied!\n");
        all_args_were_supplied = false;
    }
    return all_args_were_supplied;
}

int main(int argc, char *argv[]) {
    if (check_if_args_were_supplied(argc) == false) {
        return 1;
    }

    char err_buffer_size[ERR_BUFFER_SIZE];

    FILE* f_ptr = fopen(argv[2], "w");

    sprintf(err_buffer_size, "Error while opening file %s", argv[2]);
    if (check_if_error(err_buffer_size)) {
        return 1;
    }

    off_t n1 = strtol(argv[3], NULL, 10);
    off_t n2 = strtol(argv[4], NULL, 10);

    time_t m1 = get_time_from_string(argv[5]);
    time_t m2 = get_time_from_string(argv[6]);

    print_dir_content(argv[1], f_ptr, n1, n2, m1, m2);
    return 0;
}
