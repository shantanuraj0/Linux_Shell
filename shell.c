#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>



int setpgid(pid_t pid, pid_t pgid);
struct dirent *readdir(DIR *dirp);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int errno;



#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64




int check_pipe(char *input)
{ 
        int p,count=0;
        
        for(p=0;p<strlen(input);p++)
        {if(input[p] == '|')
                count++;
        }
        return count;
}



char **pipe_parse(char *input)
{
        char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        char *tok = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
        int p, token_Index = 0, token_No = 0;

        for(p =0; p < strlen(input); p++)
        {

                char readChar = input[p];
                if(input[p-1] == '|')
                        continue;
                if (readChar == '|' || readChar == '\n' || readChar == '\t')
                {

                        tok[token_Index] = '\0';
                        if (token_Index != 0)
                        {
                                tokens[token_No] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                                strcpy(tokens[token_No++], tok);
                                token_Index = 0;
                        }
                } else {

                        tok[token_Index++] = readChar;
                }
        }

        free(tok);
        tokens[token_No] = NULL ;
        return tokens;

}



char **tokenize(char *input)
{
        char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        char *tok = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
        int p, token_Index = 0, token_No = 0;

        for(p =0; p < strlen(input); p++)
        {

                char readChar = input[p];

                if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
                        tok[token_Index] = '\0';
                        if (token_Index != 0)
                        {
                                tokens[token_No] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                                strcpy(tokens[token_No++], tok);
                                token_Index = 0;
                        }
                } else 
                {
                        tok[token_Index++] = readChar;
                }
        }

        free(tok);
        tokens[token_No] = NULL ;
        return tokens;
}



int main(int argc, char* argv[])
 {
        
        char  input_line[MAX_INPUT_SIZE];
        char  **tokens;
        int i;

        printf("\n\n******** Welcome to my shell ********\n\n");

        int back=0;
        while(2) 
        {

                bzero(input_line, sizeof(input_line));
                printf("<<shantanu_raj>> ");

                scanf("%[^\n]", input_line);
                getchar();

                if(strcmp(input_line,"\0") ==0 )
                        continue;


                input_line[strlen(input_line)] = '\n';

                if(check_pipe(input_line)!=0)
                { 
                        
                        int i,num_pipe = check_pipe(input_line),status;

                        tokens = pipe_parse(input_line);
                        
                        char **str;
                        
                        int pipes[2*num_pipe];
                        
                        for(i=0;i<(2*num_pipe);i+=2)
                                pipe(pipes+i);
                        int j;
                        

                        int g=0;
                        for(i=0; i<num_pipe+1; i++ )
                           {  
                                   if(i==num_pipe)
                                  { 
                                     int length = strlen(tokens[i]);
                                     tokens[i][length-1] = ' ';
                                     strcat(tokens[i]," ");
                                  }

                                 str = tokenize(tokens[i]);

                                 if(fork()==0)
                                    {
                                         if(i==0)
                                          dup2(pipes[1],1);
                                      else if(i==num_pipe)
                                          dup2(pipes[(2*i)-2],0);
                                      else
                                        {
                                          dup2(pipes[g],0);
                                          dup2(pipes[g+3],1);
                                          g=g+2;
                                        }

                                        for(j=0;j<(2*num_pipe);j++)
                                               close(pipes[j]);

                                        if(execvp(str[0],str)<0)
                                                printf("Invalid Command!\n");
                                    }
                             }

                            for(j=0;j<(2*num_pipe);j++)
                                               close(pipes[j]);

                           
                           for (i = 0; i < num_pipe+1; i++)
                                        wait(&status);


                        continue;
                   }

        tokens = tokenize(input_line);


                if(strcmp(tokens[0],"exit") == 0 )  //to handle exit command
                {
                        printf("***************Exiting****************\n");
                        exit(0);
                        
                }

                if(strcmp(tokens[0],"cd") ==0 )  // to handle cd command
                { 
                        char str[1000];
                        if(chdir(tokens[1])== 0)
                                printf("changed dir. to : %s\n",getcwd(str,1000));
                        continue;
                }

                if(strcmp(tokens[0],"pwd") ==0 ) // to handle pwd command
                {
                         char str[1000];

                        printf("current dir. : %s\n",getcwd(str,1000));
                        continue;
                }

                
                if(strcmp(tokens[0],"ls") ==0 )  // to handle ls command
                {  {

                           DIR *dir = opendir(".");
                           struct dirent *dp;

                           while ((dp = readdir(dir)) != NULL)
                                   printf("%s  ", dp->d_name);

                           closedir(dir);
                          printf("\n");
                           continue;
                   }}

                pid_t p_id = fork();


                for( i=0; tokens[i+1] !=NULL; i++ );

                back =0;
                
                if( strcmp(tokens[i],"&" ) == 0 )
                {   
                        back =1;
                        tokens[i]=NULL;
                }

                
                if (p_id == -1)
                 {
                        printf("\nFailed");
                        return;
                }
                 else if (p_id == 0)
                  {
                        if (execvp(tokens[0],tokens) < 0 ) 
                        {
                                printf("\nUnable to execute given command..\n");
                        }
                        exit(0);
                } 
                else
                 {
                        if(back==0)
                                waitpid(p_id,NULL,0);

                }



                for(i=0;tokens[i] != NULL; i++ )
                {
                        free(tokens[i]);
                }
                free(tokens);

        }
        return 0;
}
