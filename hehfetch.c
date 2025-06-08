#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <pwd.h>

#define MAX_LINE_LENGTH 256
#define ASCII_ART_LINES 8

#define RESET   "\033[0m"
#define WHITE   "\033[37m"
#define ARCH_COLOR    "\033[1;34m"
#define DEBIAN_COLOR  "\033[31m"
#define FEDORA_COLOR  "\033[34m"
#define GENTOO_COLOR  "\033[35m"
#define UBUNTU_COLOR  "\033[93m"
#define VOID_COLOR    "\033[32m"
#define DEFAULT_COLOR "\033[33m"

const char *arch_ascii[] = {
    "       /\\        ",
    "      /  \\       ",
    "     /\\   \\      ",
    "    /      \\     ",
    "   /   ,,   \\    ",
    "  /   |  |  -\\   ",
    " /_-''    ''-_\\  ",
    "                  "
};

const char *debian_ascii[] = {
    "    _____      ",
    "   /  __ \\     ",
    "  |  /    |    ",
    "  |  \\___-     ",
    "  -_           ",
    "    --_        ",
    "               ",
    "               "
};

const char *fedora_ascii[] = {
    "        ,'''''.   ",
    "       |   ,.  |  ",
    "       |  |  '_'  ",
    "  ,....|  |..     ",
    ".'  ,_;|   ..'    ",
    "|  |   |  |       ",
    "|  ',_,'  |       ",
    " '.     ,'        "
};

const char *gentoo_ascii[] = {
    "   _-----_       ",
    "  (       \\      ",
    "  \\    0   \\     ",
    "   \\        )    ",
    "   /      _/     ",
    "  (     _-       ",
    "  \\____-         ",
    "                  ",
};

const char *ubuntu_ascii[] = {
    "           _      ",
    "       ---(_)     ",
    "   _/  ---  \\     ",
    "  (_) |   |       ",
    "   \\  --- _/      ",
    "      ---(_)      ",
    "                  ",
    "                    "
};

const char *void_ascii[] = {
    "      _______     ",
    "   _ \\______ -    ",
    "  | \\  ___  \\ |   ",
    "  | | /   \\ | |   ",
    "  | | \\___/ | |   ",
    "  | \\______ \\_|   ",
    "   -_______\\      ",
    "                   "
};

const char **get_distro_ascii(const char *distro) {
    if (strstr(distro, "Arch") || strstr(distro, "arch")) {
        return arch_ascii;
    } else if (strstr(distro, "Debian") || strstr(distro, "debian")) {
        return debian_ascii;
    } else if (strstr(distro, "Fedora") || strstr(distro, "fedora")) {
        return fedora_ascii;
    } else if (strstr(distro, "Gentoo") || strstr(distro, "gentoo")) {
        return gentoo_ascii;
    } else if (strstr(distro, "Ubuntu") || strstr(distro, "ubuntu")) {
        return ubuntu_ascii;
    } else if (strstr(distro, "Void") || strstr(distro, "void")) {
        return void_ascii;
    }
    
    
    static const char *default_ascii[] = {
        "      ___      ",
        "     (.· |     ",
        "     (<> |     ",
        "    / __  \\    ",
        "   ( /  \\ /|   ",
        "  _/\\ __)/_)   ",
        "  \\/-____\\/    ",
        "                "
    };
    return default_ascii;
}

const char *get_distro_color(const char *distro) {
    if (strstr(distro, "Arch") || strstr(distro, "arch")) {
        return ARCH_COLOR;
    } else if (strstr(distro, "Debian") || strstr(distro, "debian")) {
        return DEBIAN_COLOR;
    } else if (strstr(distro, "Fedora") || strstr(distro, "fedora")) {
        return FEDORA_COLOR;
    } else if (strstr(distro, "Gentoo") || strstr(distro, "gentoo")) {
        return GENTOO_COLOR;
    } else if (strstr(distro, "Ubuntu") || strstr(distro, "ubuntu")) {
        return UBUNTU_COLOR;
    } else if (strstr(distro, "Void") || strstr(distro, "void")) {
        return VOID_COLOR;
    }
    return DEFAULT_COLOR;
}

void get_distro_name(char *distro) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    
    fp = fopen("/etc/os-release", "r");
    if (fp == NULL) {
        strcpy(distro, "Unknown");
        return;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
            char *value = strchr(line, '=') + 1;
            if (value[0] == '"') {
                value++;
                char *end = strchr(value, '"');
                if (end) *end = '\0';
            }
            strcpy(distro, value);
            fclose(fp);
            return;
        }
    }
    
    fclose(fp);
    strcpy(distro, "Unknown");
}

void get_uptime(char *uptime) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) {
        strcpy(uptime, "Unknown");
        return;
    }
    
    long days = si.uptime / 86400;
    long hours = (si.uptime % 86400) / 3600;
    long minutes = (si.uptime % 3600) / 60;
    
    char temp[MAX_LINE_LENGTH] = "";
    char *ptr = temp;
    
    if (days > 0) {
        ptr += sprintf(ptr, "%ldd ", days);
    }
    if (hours > 0) {
        ptr += sprintf(ptr, "%ldh ", hours);
    }
    ptr += sprintf(ptr, "%ldm", minutes);
    
    strcpy(uptime, temp);
}

void get_package_count(char *count) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int packages = 0;
    
    //pacman
    fp = popen("pacman -Qq | wc -l", "r");
    if (fp != NULL) {
        if (fgets(line, sizeof(line), fp)) {
            packages = atoi(line);
            pclose(fp);
            sprintf(count, "%d", packages);
            return;
        }
        pclose(fp);
    }
    
    //apt
    fp = popen("dpkg-query -f '${binary:Package}\n' -W | wc -l", "r");
    if (fp != NULL) {
        if (fgets(line, sizeof(line), fp)) {
            packages = atoi(line);
            pclose(fp);
            sprintf(count, "%d", packages);
            return;
        }
        pclose(fp);
    }
    
    strcpy(count, "Unknown");
}

void get_memory_usage(char *memory) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) {
        strcpy(memory, "Unknown");
        return;
    }
    
    unsigned long total = si.totalram * si.mem_unit;
    unsigned long free = si.freeram * si.mem_unit;
    unsigned long used = (total - free) / 2;
    
    sprintf(memory, "%luM / %luM", 
            (unsigned long)((float)used / (1024*1024)),
            (unsigned long)((float)total / (1024*1024)));
}

void get_shell(char *shell) {
    char *shell_path = getenv("SHELL");
    if (shell_path == NULL) {
        strcpy(shell, "Unknown");
        return;
    }
    
    char *shell_name = strrchr(shell_path, '/');
    if (shell_name != NULL) {
        shell_name++;
        strcpy(shell, shell_name);
    } else {
        strcpy(shell, shell_path);
    }
}

void get_wm(char *wm) {
    char *wm_name = getenv("XDG_CURRENT_DESKTOP");
    if (wm_name == NULL) {
        wm_name = getenv("DESKTOP_SESSION");
    }
    
    if (wm_name == NULL) {
        strcpy(wm, "Unknown");
        return;
    }
    
    strcpy(wm, wm_name);
}

void get_prompt(char *prompt) {
    char hostname[MAX_LINE_LENGTH];
    struct passwd *pw;
    
    gethostname(hostname, sizeof(hostname));
    pw = getpwuid(getuid());
    
    if (pw != NULL) {
        sprintf(prompt, "%s@%s", pw->pw_name, hostname);
    } else {
        strcpy(prompt, "unknown@unknown");
    }
}



void print_usage(const char *program_name) {
    printf("Usage: %s [DISTRO]\n", program_name);
    printf("\nAvailable distributions:\n");
    printf("  arch, debian, fedora, gentoo, ubuntu, void\n");
}

int main(int argc, char *argv[]) {
    char distro[MAX_LINE_LENGTH];
    char uptime[MAX_LINE_LENGTH];
    char packages[MAX_LINE_LENGTH];
    char memory[MAX_LINE_LENGTH];
    char shell[MAX_LINE_LENGTH];
    char wm[MAX_LINE_LENGTH];
    char prompt[MAX_LINE_LENGTH];
    
    get_distro_name(distro);
    get_uptime(uptime);
    get_package_count(packages);
    get_memory_usage(memory);
    get_shell(shell);
    get_wm(wm);
    get_prompt(prompt);
    

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        strcpy(distro, argv[1]);
    }
    
    const char **ascii_art = get_distro_ascii(distro);
    const char *color = get_distro_color(distro);
    

    for (int i = 0; i < ASCII_ART_LINES; i++) {
        printf("%s%s%s", color, ascii_art[i], RESET);
        
        switch (i) {
            case 0:
                printf("  %s%s%s", "\033[35m", prompt, RESET);
                break;
            case 1:
                printf("  %sos%s      %s", color, RESET, distro);
                break;
            case 2:
                printf("  %suptime%s  %s", color, RESET, uptime);
                break;
            case 3:
                printf("  %spkgs%s    %s", color, RESET, packages);
                break;
            case 4:
                printf("  %sshell%s   %s", color, RESET, shell);
                break;
            case 5:
                printf("  %swm/de%s   %s", color, RESET, wm);
                break;
            case 6:
                printf("  %smemory%s  %s", color, RESET, memory);
                break;
        }
        printf("\n");
    }

    
    return 0;
}