/* IOMngr.h
   Implements a simple interface for reading characters from
   the source file while producing a listing file. Functionality
   includes
   - opening source and listing files by name
   - closing source and listing files
   - getting a single character from the source file
   - write an indicator mark at a specified column
   - write a message to the listing file
   - get current line number
   - get current column number
   
   - OpenFiles opens the two files given by name. ASourceName is required 
   but AListingName may be NULL. If AListingName is NULL output will be 
   directed to stderr (the system error output stream). If the opens are
   successful this function returns true, otherwise it returns false. 
   
   - CloseFiles closes the two files.
   
   - GetSourceChar returns the next character in the source file. This
   function must also arrange for source file lines to be echoed to the 
   listing file (see Note) with a prepended line number. The EOF character 
   is returned if the source file is at the end of file.
   
   - WriteIndicator writes a single error indicator line to the listing 
   file. The error indicator line consists of a single '^' character in 
   the requested column.
   
   - WriteMessage writes the given message string to the listing file 
   as a separate line.
   
   - GetCurrentLine returns the current line number in the source file.
   
   - GetCurrentColumn returns the column number of the most recently
   returned character.

   Note: Error indicator and message lines must be correctly interspersed 
   with the echoed source file lines. This requires that source file lines 
   be read and buffered in their entirety prior to returning any characters
   from the line. The source lines are then echoed to the listing file. 
   If the listing file is null then only lines which are followed by error
   messages should be echoed.  
   
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
                        
#define MAXLINE 1024
           
int OpenFiles(const char * ASourceName,
               const char * AListingName);
void CloseFiles(); 

char GetSourceChar();

void WriteIndicator(int AColumn); 
void WriteMessage(const char * AMessage);

int GetCurrentLine();
int GetCurrentColumn();
