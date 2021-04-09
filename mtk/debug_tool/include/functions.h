#ifndef DEBUG_TOOL_
#define DEBUG_TOOL_

/**
 * Logging directory of the tool.
 */
#define LOG_DIRECTORY "/data/local/tmp/logging/"

/**
 * Pseudo Enforce SELinux value.
 */
#define SELINUX "/sys/fs/selinux/enforce"

/**
 * Max allowed lenght for some variables.
 */
#define MAX_LENGHT 1000

/**
 * Shows (or displays) the usage of the tool and all
 * its available options.
 */
void show_help();

/**
 * Executes the given shell command and returns its output.
 * If the output it's empty will not return anything.
 */
char *__exec_sh(char *cmd);

/**
 * Copies the given folder to the given destination path.
 */
int __copy_folder(char *src, char *dest);

/**
 * First checks if the tool was launched from a root shell (uid=0),
 * if wasn't, it will try to find su binary and warn the user to
 * launch the tool from a root shell (using su).
 */
int check_root();

/**
 * Writes an int value to a specific file.
 */
int write_int(char *file, int value);

/**
 * Checks if the specified path is empty or not.
 */
bool is_dir_empty(char *dirname);

#endif /* DEBUG_TOOL_ */
