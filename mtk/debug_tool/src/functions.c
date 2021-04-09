#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#include "functions.h"

void show_help() 
{
    printf("    USAGE:\n");
    printf("    -h Prints this help message.\n");
    printf("    -c Clean up the LOG directory.\n");
    printf("    -l Take logcat.\n");
    printf("    -d Take dmesg.\n");
    printf("    -b Take bugreport.\n");
    printf("    -g Get device properties.\n");
    printf("    -t Get last tombstones.\n");
    printf("    -s Change SELinux status.\n\n");
}

char *__exec_sh(char *cmd)
{
    FILE *fp;
    char buffer[256];
    size_t chread, alloc = 256, len = 0;
    char  *out   = malloc(alloc);

    if ((fp = popen(cmd, "r")) == NULL) return NULL;
 
    while ((chread = fread(buffer, 1, sizeof(buffer), fp)) != 0) {
        if (len + chread >= alloc) {
            alloc *= 2;
            out = realloc(out, alloc);
        }
        memmove(out + len, buffer, chread);
        len += chread;
    }

    pclose(fp);
    return out;
}

int __copy_folder(char *src, char *dest)
{
    char cmd[MAX_LENGHT];

    /* FIXME: Stop using system commands to copy folders */
    snprintf(cmd, 100, "cp -r %s %s/", src, dest);

    return system(cmd);
}

int write_int(char *file, int value)
{
    FILE *fp = fopen(file, "w");

    if (fp == NULL){
      printf("[-] Failed to open %s!\n", file);
      return 1;
   }

   printf("[?] Write %d to %s\n", value, file);
   fprintf(fp,"%d",value);

   fflush(fp);
   fclose(fp);

   return 0;
}

bool is_dir_empty(char *dirname) 
{
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(dirname);

    if (dir == NULL) return false;

    while ((d = readdir(dir)) != NULL) if(++n > 2) break;

    closedir(dir);

    if (n <= 2) return true; else return false;
}

