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
#define BUF_SIZE 2048
using namespace std;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* server_handle_connection(void*);
void* client_handle_connection(void*);
void server(int);
void client(int,char*);

struct file{
            char *fileName;
            int port;
        };
void* client(void* args){

    cout<<"enter port for clinet"<<endl;    
        int port;
        cin>>port;
   // free(input);
        string cmd;
        cin.get();
        getline(cin,cmd);
        char *fileName=(char *)malloc(sizeof(cmd.length()+1));
        strcpy(fileName,cmd.c_str());
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=INADDR_ANY;
    int connection_status=connect(client_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(connection_status==-1)
        printf("connection failed\n");
    else
        printf("connection success\n");
    long fd1, fd2;    
    long ret_in, ret_out;    
     
    // string cmd;
    // getline(cin,cmd);
    // char *fileName=(char *)malloc(sizeof(cmd.length()+1));
    // strcpy(fileName,cmd.c_str());
    fd2=open(fileName,O_WRONLY|O_TRUNC|O_CREAT,0777);
    int counter=0;
    char buf[BUF_SIZE];
    while((ret_in=read(client_socket,buf,sizeof(buf)))>0){
    ret_out=write(fd2,buf,BUF_SIZE);
    }   

    close(client_socket);     
    return NULL;
}

void* server_handle_connection(void* input){
    int client_socket=((struct file*)input)->port;
    char *fileName=((struct file*)input)->fileName;
    cout<<"fileName:"<<fileName<<endl;
    free(input);
    long fd1, fd2;    
    long ret_in, ret_out;    
     //char fileName[20];
    //  printf("enter input file\n");
    //  scanf("%s",fileName);
    // struct file_name{
    //     char file_name[100];
    // };
    // struct file_data{
    //     int seq_no;
    //     char buffer[BUF_SIZE];
    // };

    // struct file_data f_d;
    // struct file_name f_n;
    //scanf("%s",f_n.file_name);
    char buffer[BUF_SIZE];
    fd1=open(fileName,O_RDONLY);
    printf("fd1==%ld\n", fd1);
    //char buf[sizeof(struct file_data)];
    //int counter=0;
    while((ret_in=read(fd1,buffer,BUF_SIZE))>0){
        //f_d.seq_no=counter+10;
    //memcpy(buf,&f_d,sizeof(struct file_data)); 
    //counter++;
   // cout<<"ret::"<<ret_in<<endl;
    ret_out=write(client_socket,buffer,sizeof(buffer));

    }    

     close(client_socket);
     return NULL;

}
void* server(void* pport){
    int port=*((int *)pport);
    cout<<"port server"<<port<<endl;
    int server_socket=socket(AF_INET,SOCK_STREAM,0);
    int client_socket;
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=INADDR_ANY;
    
    bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address));

    listen(server_socket,5);
    while(1){
        printf("waiting for connection\n");
        client_socket=accept(server_socket,NULL,NULL);
        if(client_socket>=0)
        printf("connection estbhsd\n");
        else
        {
            printf("connection failed");
        }
        //handle_connection(client_socket);
        struct file *f_s=(struct file *)malloc(sizeof(struct file));
        f_s->port=client_socket;
        string cmd;
        getline(cin,cmd);
        char *fName=(char *)malloc(sizeof(cmd.length()+1));
        strcpy(fName,cmd.c_str());
        cout<<"fName::"<<fName<<endl;
       // char fName[20]="sahoo.mp4";
        f_s->fileName=fName;
        pthread_t t;
        pthread_create(&t,NULL,server_handle_connection,(void *)f_s);
    }
return NULL;
}

int main(int argc, char* argv[]){
        string val=string(argv[1]);

        int prt=stoi(val);
        cout<<"prt::"<<prt;
        int *port=(int *)malloc(sizeof(int));
        *port=prt;
        pthread_t ts;
        pthread_create(&ts,NULL,server,port);
        pthread_detach(ts);
        pthread_t t;
        int val1=pthread_create(&t,NULL,client,NULL);
        if(!val1)
            cout<<"error"<<endl;
        pthread_join(t,NULL);

   }
