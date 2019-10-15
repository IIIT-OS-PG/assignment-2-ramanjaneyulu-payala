#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include<pthread.h>
#include<fstream>
#include<sstream>
#include <sys/types.h> 
#include <sys/stat.h> 
#include<vector>
using namespace std;
#define BUF_SIZE 31876

struct uploadFile{
        int port;
        int no_chunks;
        int chunks[100];
        char fileName[100];
};

struct downloadFile{
            int port;
            int no_chunks;
            int chunks[100];
            char fileName[100];
            
        };

struct userData{
    int userid;
    int port;
    int grpid;
    int password;
    int grp_owner[100];
    char ip_add[20];
    char fileNames[10][200];
};
char** stringTokens(char *buf){
    char *parts = strtok(buf," ");
    char** val=(char **)malloc(sizeof(char *)*10);
    int j=0;
    while(parts){
        cout<<"parts:"<<parts<<endl;
        val[j++]=parts;
        parts=strtok(NULL," ");

    }
    return val;
}


void* server_handle_connection(void* input){
     int client_socket=*((int*)input);

    char cmd[1000];
    recv(client_socket,cmd,sizeof(cmd),0);
    cout<<"cmd::"<<cmd<<endl;
    char **args=stringTokens(cmd);

    //cout<<"tracker::"<<args[1]<<endl;

    if(!strcmp(args[0],"download")){
        //cout<<"d_file"<<args[1]<<endl;
    struct downloadFile dF;
    char fileName[100];
    int i=1;
    sprintf(fileName,"%d",i);
    char temp[20]="image.jpg";
    strcat(fileName,temp);
    cout<<"uploaded file::"<<fileName<<endl;
    char buf1[sizeof(struct downloadFile)];
    int fd=open(fileName,O_RDONLY);
    cout<<"fd::"<<fd<<endl;
    int ret_in=read(fd,buf1,sizeof(struct downloadFile));
    memcpy(&dF,buf1,sizeof(struct downloadFile));
    send(client_socket,&dF,sizeof(struct downloadFile),0);

    }
    if(!strcmp(args[0],"upload")){
        int flag=1;
        send(client_socket,&flag,sizeof(int),0);  
        struct uploadFile uF;
        recv(client_socket,&uF,sizeof(struct uploadFile),0);
        cout<<"uP::port::"<<uF.port<<endl;
        char buf[sizeof(struct uploadFile)];
        char fileName[100];
        int i=1;
        sprintf(fileName,"%d",i);
        strcat(fileName,uF.fileName);
        memcpy(buf,&uF,sizeof(struct uploadFile));
        int fd=open(fileName,O_WRONLY|O_CREAT|O_TRUNC,0644);
             long   ret_out=write(fd,buf,sizeof(struct uploadFile));
        cout<<"ret_out::"<<ret_out<<endl;  
          
    }



}

void* server(void* pport)
{
    int port=*((int *)pport);
    cout<<"port server"<<port<<endl;
    int server_socket=socket(AF_INET,SOCK_STREAM,0);
    cout<<"sockt::"<<server_socket<<endl;
    int client_socket;
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=INADDR_ANY;
    
   cout<<"bind::" <<bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<<endl;

   cout<<"listen:" <<listen(server_socket,5)<<endl;
    while(1){
        printf("waiting for connection\n");
        client_socket=accept(server_socket,NULL,NULL);
        if(client_socket>=0)
        printf("connection estbhsd\n");
        else
        {
            printf("connection failed");

        }
        int *clnt=(int *)malloc(sizeof(int));
        *clnt=client_socket;
        pthread_t t;
        pthread_create(&t,NULL,server_handle_connection,(void *)clnt);
    }    
       
    return NULL;
}

int main(int argc, char* argv[])
{
        string val=string(argv[1]);
      //  freopen("outputTracker.txt", "a", stdout);
        int prt=stoi(val);
        cout<<"prt::"<<prt<<endl;
        int *port=(int *)malloc(sizeof(int));
        *port=prt;
        pthread_t st;
        cout<<pthread_create(&st,NULL,server,port)<<endl;
       // pthread_detach(st);
        pthread_join(st,NULL);
}