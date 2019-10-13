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
#define BUF_SIZE 31876
using namespace std;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* server_handle_connection(void*);
void* client_handle_connection(void*);
void server(int);
void client(int,char*);

struct file{
            char *fileName;
            int port;
            int no_chunks;
            int chunks[100];
        };
struct sktChunk{
    int no_chunks;
    char fName[200];
    int chunks[100];
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
       //struct sktChunk *sC=(struct sktChunk *)malloc(sizeof(struct sktChunk));
        struct sktChunk sC;
        strcpy(sC.fName,fileName);
        cout<<"cluent:f:"<<sC.fName<<endl;
        sC.chunks[0]=1;
        sC.chunks[1]=3;
        sC.chunks[2]=5;
        sC.no_chunks=3;
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
    char buff[sizeof(struct sktChunk)];
        // memcpy(buff,&sC,sizeof(struct sktChunk));    
        write(client_socket,&sC,sizeof(buff));
    long fd1, fd2;    
    long ret_in, ret_out;    
    long long fileSize;
    recv(client_socket,&fileSize,sizeof(fileSize),0); 
    cout<<"fileSize:"<<fileSize<<endl;
    long long fd=open(sC.fName, O_CREAT|O_TRUNC|O_WRONLY,0644);
    lseek(fd, fileSize-1, SEEK_SET); //<- err check
    write(fd, "", 1);
    ftruncate(fd, fileSize);
    close(fd); 
    // string cmd;
    // getline(cin,cmd);
    // char *fileName=(char *)malloc(sizeof(cmd.length()+1));
    // strcpy(fileName,cmd.c_str());
    fd2=open(sC.fName,O_WRONLY,0644);
    char buffer[BUF_SIZE];
   // int arr[3]={1,3,5};
    for(int i=0;i<sC.no_chunks;i++)
    {

     //lseek ( fd1 , ((arr[i]-1)*100)-1 , SEEK_SET );
        lseek ( fd2 , ((sC.chunks[i]-1)*BUF_SIZE)-1 , SEEK_SET );
        if((ret_in=read(client_socket,buffer,BUF_SIZE))>0)
        {
            cout<<"ret_in::"<<ret_in<<endl;
            ret_out=write(fd2,buffer,ret_in);
        }
    }
//   int arr2[3]={2,4,6};
//   for(int i=0;i<3;i++){
//      lseek ( fd1 , ((arr2[i]-1)*100)-1 , SEEK_SET );
//      lseek ( fd2 , ((arr2[i]-1)*100)-1 , SEEK_SET );
//  if ((ret_in=read(fd1,buffer,100))>0){
//      cout<<"ret_in::"<<ret_in<<endl;
//   ret_out=write(fd2,buffer,ret_in);
//   }
//   }  
  close ( fd1 );
  close ( fd2 );
    // fd2=open(fileName,O_WRONLY|O_TRUNC|O_CREAT,0777);
    // int counter=0;
    // char buf[BUF_SIZE];
    // while((ret_in=read(client_socket,buf,sizeof(buf)))>0){
    // ret_out=write(fd2,buf,BUF_SIZE);
    // }   

    close(client_socket);    
    return NULL;
}

void* server_handle_connection(void* input){
    int client_socket=((struct file*)input)->port;
    char *fileName=((struct file*)input)->fileName;
    int no_chunks=((struct file*)input)->no_chunks;
    int chunk_array[no_chunks];
    for(int i=0;i<no_chunks;i++)
        chunk_array[i]=((struct file*)input)->chunks[i];
    cout<<"chunk_no:"<<chunk_array[0]<<" "<<chunk_array[1]<<" "<<chunk_array[2]<<endl;    
    cout<<"fileName:"<<fileName<<endl;
    free(input);
    long fd1, fd2;    
    long ret_in, ret_out;    
    char buffer[BUF_SIZE];
    fd1=open(fileName,O_RDONLY);
    printf("fd1==%ld\n", fd1);
    for(int i=0;i<no_chunks;i++)
    {   
        lseek ( fd1 , ((chunk_array[i]-1)*BUF_SIZE)-1 , SEEK_SET );
       // lseek ( fd2 , ((chunk_array[i]-1)*100)-1 , SEEK_SET );
        if((ret_in=read(fd1,buffer,BUF_SIZE))>0)
        {
            ret_out=write(client_socket,buffer,ret_in);

        }   
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
        // getline(cin,cmd);
        // char *fName=(char *)malloc(sizeof(cmd.length()+1));
        // strcpy(fName,cmd.c_str());
        char fName[200];
        cout<<"waiting for read\n";
        char buf[sizeof(struct sktChunk)];

        struct sktChunk sC;
        if(read(client_socket,&sC,sizeof(buf)))
            cout<<"fileName read"<<endl;
       // memcpy(&sC,buf,sizeof(buf));
       // char fName[20]="sahoo.mp4";
       struct stat sb;
        long fd1 = open ( sC.fName , O_RDONLY );
        if(fstat(fd1, &sb)!=-1)
        cout<<"server FileSize::"<<sb.st_size<<endl;
        send(client_socket,&sb.st_size,sizeof(long long),0); 
        cout<<"fName::"<<sC.fName<<endl;
        cout<<"no_chunks::"<<sC.no_chunks<<endl;
        
        f_s->fileName=sC.fName;
        for(int i=0;i<sC.no_chunks;i++)
            f_s->chunks[i]=sC.chunks[i];
        f_s->no_chunks=sC.no_chunks;
        pthread_t t;
        pthread_create(&t,NULL,server_handle_connection,(void *)f_s);
    }
return NULL;
}

int main(int argc, char* argv[]){
        string val=string(argv[1]);
        freopen("outputpeer.txt", "a", stdout);
        int prt=stoi(val);
        cout<<"prt::"<<prt;
        int *port=(int *)malloc(sizeof(int));
        *port=prt;
        pthread_t ts;
        pthread_create(&ts,NULL,server,port);
        pthread_detach(ts);
        while(1){
        pthread_t t;
        int val1=pthread_create(&t,NULL,client,NULL);
        if(!val1)
            cout<<"error"<<endl;
        pthread_join(t,NULL);
        }
   }
