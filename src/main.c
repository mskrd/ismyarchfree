#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LENGTH 512

int main(void) {
    FILE *fp;
    char line[MAX_LENGTH];
    char packages[MAX_LINES][MAX_LENGTH];
    int index = 0;

    fp = popen("pacman -Qi | awk -F ': ' '/^Name/ {name=$2} /^License/ {print name \":\" $2 }'", "r");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        strncpy(packages[index], line, sizeof(packages[index]));
        index++;

        if (index >= MAX_LINES) {
            break;
        }
    }

    fclose(fp);

    char *free[] = {"GPL", "WTFPL", "CC0", "CC-PDDC"};
    char *permissive[] = {"MIT", "Apache" , "OFL", "BSD", "MPL", "SIL", "HPND", "ISC", "X11", "APACHE", "PerlArtistic", "PSF", "Zlib", "PostgreSQL"};
    char *restrictive[] = {"sleepycat", "BSL"};
    char *proprietary[] = {"chrome"};
    char *custom[] = {"custom", "LicenseRef"};

    int freeCount = 0;
    int permissiveCount = 0;
    int restrictiveCount = 0;
    int proprietaryCount = 0;
    int customCount = 0;
    int unidentifiedCount = 0;

    for (int i = 0; i < index; i++) {
        printf("%s", packages[i]);
        int is_free = 0;
        int is_permissive = 0;
        int is_restrictive = 0;
        int is_proprietary = 0;
        int is_custom = 0;

        // Check for "free" license
        for (int j = 0; j < 4; j++) {
            if (strstr(packages[i], free[j])) {
                is_free = 1;
                break;
            }
        }

        // Check for "permissive" license
        for (int j = 0; j < 14; j++) {
            if (strstr(packages[i], permissive[j])) {
                is_permissive = 1;
                break;
            }
        }

        // Check for "restrictive" license
        for (int j = 0; j < 2; j++) {
            if (strstr(packages[i], restrictive[j])) {
                is_restrictive = 1;
                break;
            }
        }

        for (int j = 0; j < 1; j++) {
            if (strstr(packages[i], proprietary[j])) {
                is_proprietary = 1;
                break;
            }
        }

        // Check for "custom" license
        for (int j = 0; j < 2; j++) {
            if (strstr(packages[i], custom[j])) {
                is_custom = 1;
                break;
            }
        }

        // Output color-coded status
        if (is_free) {
            printf("\033[0;32m [FREE]\n\033[0m");
            freeCount++;
        } else if (is_permissive) {
            printf("\033[0;32m [PERMISSIVE]\n\033[0m");
            permissiveCount++;
        } else if (is_restrictive) {
            printf("\033[1;33m [RESTRICTIVE]\n\033[0m");
            restrictiveCount++;
        } else if (is_proprietary) {
            printf("\033[0;31m [PROPRIETARY]\n\033[0m");
            proprietaryCount++;
        } else if (is_custom) {
            printf("\033[1;33m [CUSTOM]\n\033[0m");
            customCount++;
        } else {
            printf("\033[0;31m [UNIDENTIFIED]\n\033[0m");
            unidentifiedCount++;
        }

    }

    printf("\nIsMyArchFree results (%d packages analyzed):\n", freeCount+permissiveCount+restrictiveCount+customCount+proprietaryCount+unidentifiedCount);
    printf("\033[0;32m%d packages contain free license\033[0m\n", freeCount);
    printf("\033[0;32m%d packages contain permissive license\033[0m\n", permissiveCount);
    printf("\033[1;33m%d packages contain restrictive license\033[0m\n", restrictiveCount);
    printf("\033[1;33m%d packages contain custom license\033[0m\n", customCount);
    printf("\033[0;31m%d packages contain proprietary license\033[0m\n", proprietaryCount);
    printf("\033[0;31m%d packages contain unidentified license\033[0m\n", unidentifiedCount);
    printf("If you encountered packages with unidentified or misidentified licenses, please submit an issue with the program's output.\n");

    return 0;
}
