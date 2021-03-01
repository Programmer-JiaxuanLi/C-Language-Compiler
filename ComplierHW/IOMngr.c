#include "IOMngr.h"

char buf[1000];
FILE * sfp;
FILE * lfp;
int colum;
int line = 1;

int OpenFiles(const char * ASourceName, const char * AListingName)
{
    
    sfp = fopen(ASourceName, "r+");
    lfp = fopen(AListingName, "r+");

    if (sfp == NULL) {
        return 0;
    }
    if (lfp == NULL) {
        return 0;
    }
    
    return 1;
}

void CloseFiles()
{
    if (sfp != NULL) {
        fclose(sfp);
    }
    if (lfp != NULL) {
        fclose(lfp);
    }
}

char GetSourceChar(){
    char ch =NULL;
    if (sfp != NULL){
        
        if (colum == 0) {
            memset(buf,'\0',colum);
        }
        
        ch = fgetc(sfp);
        buf[colum] =ch;
    
        colum++;
    }
    
    return ch;
}


void WriteIndicator(int AColumn){
    
    char* c[100];
    char* b[100];
    if (lfp == NULL){
        for (int j =0; j < colum; j++) {
            printf("%c",buf[j]);
        }
        printf("\n");
        for (int i=0; i < AColumn - 1; i++) {
            printf(" ");
        }
        printf("%c\n", '^');
        } else {
            
        for (int k =0; k < colum; k++) {
            b[k] = buf[k];
        }
            fprintf(lfp,"%s\n",b);
        for (int l = 0; l < AColumn; l++) {
            fprintf(lfp,"%c",' ');
        }
         fprintf(lfp,"%c\n",'^');
        }
}

void WriteMessage(const char * AMessage)
{
    line++;
    char c = '\n';
    colum = strlen(AMessage);
    if (lfp == NULL){
        printf("%s\n", AMessage);
    } else {
        fprintf(lfp, "%s%c", AMessage, c );
    }
}

int GetCurrentLine()
{
    return line;
}

int GetCurrentColumn()
{
    return colum;
}

