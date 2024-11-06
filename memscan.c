#include <stdio.h>    //For printf()
#include <stdlib.h>   //For exit calls
#include <string.h>   //for strlen()
#define MAX 1000

const char* file_path = "/proc/self/maps"; //Location of the maps files to read
const char vvar[] = "[vvar]";

enum state {mem_start,mem_end,perms,offset,device,inode,pathname,FATAL}; //Determine region of Row

int main(){

    char addr_start[13];      //Start of Address Space
    char addr_end[13];        //End of Address Space
    char permissions[5];
    char filePath[100];       //This is to check FileName
    int  num_bytes;
    int  num_A;

    FILE * maps = fopen(file_path,"r");

    //IF FILE DNE EXIT
    if(maps == NULL){
        perror("File Path is Wrong\n");
        exit(EXIT_FAILURE);
    }

    int lines;          //Rows of Maps
    char str[MAX][MAX]; //Store Maps content here

    printf("Memory Scanner\n");

    //Store into Maps into str[][]
    for(lines=0 ; fgets(str[lines],sizeof(str[lines]),maps) != NULL ; lines++){

        #ifdef DEBUG
        printf("%s\n",str[lines]);
        #endif

        //Incrementer for traversing arrays
        int i=0;  //For str[][],and addr_start[]
        int n=0;  //For addr_end[]
        int j=0;  //For permissions[]
        int k=0;  //For filePath[]

        //Set MEM_STATE to MEM_START after done reading a row from maps
        enum state mem_state = mem_start;

        //RESET for next Row
        num_A = 0;
        num_bytes = 0;
        int len = strlen(str[lines]);

        //Write into Address_start,Address_end,Permissions and the filePath
        while(i < len && str[lines][i] != '\n'){
            
            #ifdef DEBUG
            printf("%c",str[lines][i]);
            #endif
        
            //Mem_state tells exactly where we are at on each row (i.e, MEM ADDRESS,PERMS,OFFSET)
            switch(mem_state){

                case mem_start: if(str[lines][i] == '-')
                                    mem_state = mem_end;
                                break;
                case mem_end:   if(str[lines][i] == ' ')
                                    mem_state = perms;
                                break;
                case perms:     if(str[lines][i] == ' ')
                                    mem_state = offset;
                                break;
                case offset:    if(str[lines][i] == ' ')
                                    mem_state = device;
                                break;
                case device:    if(str[lines][i] == ' ')
                                    mem_state = inode;
                                break;
                case inode:     if(str[lines][i] == ' ')
                                    mem_state = pathname;
                                break;
                case pathname:  if(str[lines][i] == '\n')
                                    mem_state = mem_start;
                                break;
                default:        mem_state = FATAL;    //Location on Row is Unknown, CLOSE AND EXIT
                                fclose(maps);
                                exit(EXIT_FAILURE);

            }

            #ifdef DEBUG
            printf("We are in %s\n",mem_state);
            #endif
        
            //Ignore any spaces and make sure arrays stay within bounds
            if (mem_state == mem_start && str[lines][i] != '-'){
                addr_start[i] = str[lines][i];
            }
            if (mem_state == mem_end && str[lines][i] != '-' && str[lines][i] != ' ' && n<12 ){
                addr_end[n] = str[lines][i];
                n++;
            }
            if (mem_state == perms && str[lines][i] != ' ' && j<5){
                permissions[j] = str[lines][i];
                j++;
            }
            if (mem_state == pathname && str[lines][i] != ' '){
                filePath[k] = str[lines][i];
                k++;
            }

            i++;
        }

        #ifdef DEBUG
        printf("%s\n",filePath);
        printf("%s\n",vvar);
        #endif

        if((strcmp(filePath,vvar)) == 0)   //Check if filename is vvar
            printf("%d:\t[vvar] Skipped\n",lines);
        else{
            //Need to set the end of string with terminating byte Or Issues will occur
            addr_start[12] = '\0';
            addr_end[12]   = '\0';
            permissions[4] = '\0';

            char* hexstring;                      //This will point to the first address of the space
            char* hexstringend;                   //This will point to the last  address of the space
            sscanf(addr_start,"%p",&hexstring);   //Set pointer hexstring to address string saved in addr_start  (i.e begin of virtual mem address)
            sscanf(addr_end,"%p",&hexstringend);  //Set pointer hexstringend to address string saved in addr_end (i.e end   of virtual mem address)
    
            #ifdef DEBUG
            printf("The address of hexstring begin is %p\n",hexstring);
            printf("The address of hexstring end is %p\n",hexstringend);
            #endif

            //Iterate through the address space, count bytes and # of A's
            for( ; hexstring < hexstringend ; hexstring++){
                if(*hexstring == 'A')
                    num_A++;
                num_bytes++;

                #ifdef DEBUG
                printf("The address of hexstring is %p\n",hexstring);
                #endif
            }

            //Print Out results
            printf("%d:\t0x0x%s - 0x0x%s %s Number of Bytes: [%d] Number of 'A': [%d] \n"
                                                                        ,lines
                                                                        ,addr_start
                                                                        ,addr_end
                                                                        ,permissions
                                                                        ,num_bytes
                                                                        ,num_A);
        
            #ifdef DEBUG
            printf("The address of hexstring after loop is %p\n",hexstring);
            #endif
        }

        //Set Filepath to \0
        memset(filePath,'\0',8*sizeof(char));
    }
    fclose(maps);
    exit(EXIT_SUCCESS);
}
