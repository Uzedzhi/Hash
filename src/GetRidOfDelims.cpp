#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

const wchar_t * WORD_DELIMS = L",./|\\!?\"\';: \n\r\t()»—«”“’‘[]";
const char * const ChangedFile = "texts/ALLTEXTSINONE.txt";

size_t GetFileSize(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    size_t FileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return FileSize;
}

wchar_t * GetBufferFromFile(FILE *fp, size_t FileSize, size_t *WCharsRead) {
    wchar_t *FileBuffer = (wchar_t *) calloc(FileSize + 1, sizeof(wchar_t));

    size_t CharsRead = 0;
    wchar_t ch = 0;
    while ((ch = fgetwc(fp)) != WEOF) {
        FileBuffer[CharsRead++] = ch;
    }
    FileBuffer[CharsRead] = (wchar_t) '\0';
    *WCharsRead = CharsRead;
    return FileBuffer;
}

int main() {
    setlocale(LC_ALL, "");

    FILE *fpread = fopen(ChangedFile, "r");

    size_t FileSizeBytes = GetFileSize(fpread);
    size_t CharsRead = 0;
    wchar_t *Buffer = GetBufferFromFile(fpread, FileSizeBytes, &CharsRead);
    fclose(fpread);

    FILE *fpwrite = fopen(ChangedFile, "w");
    for (size_t i = 0; i < CharsRead; i++) {
        int Found = 0;
        wchar_t ch = Buffer[i];
        
        for (size_t j = 0; j < wcslen(WORD_DELIMS); j++) {
            if (ch == WORD_DELIMS[j]) {
                Found = 1;
                break;
            }
        }
        
        if (Found)
            ch = (wchar_t) ' ';
        
        fputwc(ch, fpwrite);
    }
    
    fclose(fpwrite);
    free(Buffer);
    return EXIT_SUCCESS;
}