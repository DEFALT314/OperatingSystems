#include <dirent.h>
#include <stdio.h>
#include <string.h>
#define MAXPATHSIZE 1000
#define MAXLINE 1000
int checkIfTxtFile(char* filename){
    int len = strlen(filename);
    if (len < 4) {
        return 0;
    }
    return strcmp(&filename[len-4], ".txt") == 0;
}

int main(int argc, char *argv[]){
    char* oldDirPath = argv[1];
    char* newDirPath = argv[2];
    char oldPathFile[MAXPATHSIZE];
    char newPathFile[MAXPATHSIZE];
    char line[MAXLINE];
    FILE *oldFile;
    FILE *newFile;
    DIR* oldDir = opendir(oldDirPath);
    struct dirent *file= readdir(oldDir);
    while ((file= readdir(oldDir)) != NULL){
        if (!checkIfTxtFile(file->d_name)){
            continue;
        }
        strcpy(oldPathFile, oldDirPath);
        strcat(oldPathFile, "/");
        strcat(oldPathFile, file->d_name);
        strcpy(newPathFile, newDirPath);
        strcat(newPathFile, "/");
        strcat(newPathFile, file->d_name);
        oldFile = fopen(oldPathFile, "r");
        newFile = fopen(newPathFile, "w");

        while (fgets(line, MAXLINE, oldFile) != NULL)
        {
            int len = strlen(line);
            if (line[len-1] == '\n'){
                line[len-1] = '\0';
                len--;
            }
            for(int i = len-1; i>=0; i--){
                fwrite(&line[i], sizeof(char), 1, newFile);
            }
            fwrite("\n", sizeof(char), 1, newFile);
        }
        fclose(oldFile);
        fclose(newFile);
    }
}
