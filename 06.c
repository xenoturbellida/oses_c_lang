// 06.c

#include <stdio.h>
#include <dirent.h> //dirent, readdir
#include <errno.h>  //errno
#include <unistd.h> // getcwd
#include <limits.h> // PATH_MAX

int ERR_BUFFER_SIZE = 128;

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

int read_dir_entries_by_path(char path[]) {

    DIR* dir_ptr;
    struct dirent* dirent_ptr;

    dir_ptr = opendir(path);
    char err_message_buf[ERR_BUFFER_SIZE];
    sprintf(err_message_buf, "Error while opening directory %s", path);

    if (check_if_error(err_message_buf) == 0) {
        printf("---Entries of directory %s---\n", path);
        for (;;) {
            dirent_ptr = readdir(dir_ptr);
            sprintf(err_message_buf, "Error while reading directory %s", path);

            if (check_if_error(err_message_buf) == 0) {
                if (dirent_ptr == NULL) {
                    return try_to_close_dir(dir_ptr);
                }
                printf("%s\n", dirent_ptr->d_name);
            } else {
                try_to_close_dir(dir_ptr);
                break;
            }
        }
    }
    
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Directory path was not supplied!\n");
        return 1;
    }

    char* cwd;
    char buff[PATH_MAX + 1];
    cwd = getcwd(buff, PATH_MAX + 1);
    if (check_if_error("Error while getting current working directory") == 1) {
        return 1;
    }
    return read_dir_entries_by_path(cwd) | read_dir_entries_by_path(argv[1]);
}
