#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE * ptrFile = fopen("/home/user/projects/c_cpp_projects/other/TestRHVoice/io.txt", "w");
    char ch;
    while ((ch = getchar()) != EOF)
    {
        fputc(ch, ptrFile);
        fflush(ptrFile);
    }
    
    fclose(ptrFile);
    return 0;
}
