#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <vector>
#define BUF_SIZE 31876
using namespace std;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* server_handle_connection(void*);
void* client_download_handler(void*);
void server(int);
void client(int,char*);

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
struct sktChunk{
    int no_chunks;
    char fName[200];
    int chunks[100];
};  

void* client_download_handler(void* args)
{           struct sktChunk sC;
            int port=((struct downloadFile*)args)->port;
            strcpy(sC.fName,((struct downloadFile*)args)->fileName);
            cout<<"in client handlr:fname::"<<sC.fName<<endl;
            sC.no_chunks=((struct downloadFile*)args)->no_chunks;

            for(int i=0;i<sC.no_chunks;i++)
                sC.chunks[i]=((struct downloadFile*)args)->chunks[i];  
            free(args);
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
            write(client_socket,&sC,sizeof(struct sktChunk));      
            long fd1, fd2;    
            long ret_in, ret_out;    
            long long fileSize;

            fd2=open(sC.fName,O_WRONLY|O_CREAT,0644);
            cout<<"fd2::"<<fd2<<endl;
            char buffer[BUF_SIZE];
            for(int i=0;i<sC.no_chunks;i++)
            {

                lseek ( fd2 , ((sC.chunks[i]-1)*BUF_SIZE)-1 , SEEK_SET );
                if((ret_in=read(client_socket,buffer,BUF_SIZE))>0)
                {
                    cout<<"chunk_no:"<<sC.chunks[i]<<"ret_in from socket::"<<ret_in<<endl;
                    ret_out=write(fd2,buffer,ret_in);
                }
            }
        close ( fd1 );
        close ( fd2 );
    return NULL;    
}
void* client(void* args){

//while(1){
    
   
    return NULL;
}

void* server_handle_connection(void* input){
    int client_socket=((struct downloadFile*)input)->port;
    char fileName[100];
    strncpy(fileName,((struct downloadFile*)input)->fileName, 100);
    int no_chunks=((struct downloadFile*)input)->no_chunks;
    int chunk_array[no_chunks];
    for(int i=0;i<no_chunks;i++)
        chunk_array[i]=((struct downloadFile*)input)->chunks[i];
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
            cout<<"chunk_no:"<<chunk_array[i]<<"ret_in from file::"<<ret_in<<endl;
            ret_out=write(client_socket,buffer,ret_in);
           // cout<<"chunk_no:"<<chunk_array[i]<<"ret_out to socket::"<<ret_out<<endl;

        }   
    }

    close(fd1);

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
        struct downloadFile *f_s=(struct downloadFile *)malloc(sizeof(struct downloadFile));
        f_s->port=client_socket;
        string cmd;
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
       // send(client_socket,&sb.st_size,sizeof(long long),0); 
        cout<<"fName::"<<sC.fName<<endl;
        cout<<"no_chunks::"<<sC.no_chunks<<endl;
        
        strcpy(f_s->fileName,sC.fName);
       // f_s->fileName=sC.fName;
       cout<<":f_s: "<<f_s->fileName<<endl;
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
       //	freopen("outputPeer.txt", "a", stdout);
        int prt=stoi(val);
        cout<<"prt::"<<prt;
        int *port=(int *)malloc(sizeof(int));
        *port=prt;
        pthread_t ts;
        pthread_create(&ts,NULL,server,port);
        pthread_detach(ts);

        string val1=string(argv[2]);
        int prt_traker=stoi(val1);
        
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    
    server_address.sin_port=htons(prt_traker);

    server_address.sin_addr.s_addr=INADDR_ANY;
    int connection_status=connect(client_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(connection_status==-1)
        printf("connection failed\n");
    else
        printf("connection success\n");
    while(1){
        cin.get();
        string cmd;
        getline(cin,cmd);
        char *command=(char *)malloc(sizeof(cmd.length()+1));
        //char command[cmd.length()+1];
        strcpy(command,cmd.c_str());
        char *command1=(char *)malloc(sizeof(cmd.length()+1));
        strcpy(command1,cmd.c_str());
        cout<<"this is command::"<<command<<endl;
        char **arg=(char **)malloc(sizeof(char*)*10);
        char *parts = strtok(command," \n");
        cout<<"parts::"<<parts<<endl;
        // arg[0]=parts;
        // cout<<"cmd::"<<arg[0];
        
        
        if(!strcmp("download",parts))
        {
        	cout<<"download request from client"<<command1<<endl;
            send(client_socket,command1,sizeof(command1),0);
            cout<<"hello"<<endl;
            pthread_t cD;
            struct downloadFile dF;

            int n=recv(client_socket,&dF,sizeof(struct downloadFile),0);
            cout<<"hi "<<n<<endl;
            cout<<"client fileName:"<<dF.fileName<<endl;
            struct downloadFile *temp=(struct downloadFile*)malloc(sizeof(struct downloadFile));
            temp->no_chunks=dF.no_chunks;
            for(int i=0;i<dF.no_chunks;i++)
                temp->chunks[i]=dF.chunks[i];
            strcpy(temp->fileName,dF.fileName);
            temp->port=dF.port;    
            pthread_create(&cD,NULL,client_download_handler,(void *)temp);

        }
         if(!strcmp("upload",parts)){
            send(client_socket,command1,sizeof(command1),0);

            int flag;
            recv(client_socket,&flag,sizeof(int),0);
            if(flag){
            cout<<"enter upload file details in order : port, no.chunks, chunks[] fileName"<<endl;
             struct uploadFile upF;
             cin>>upF.port>>upF.no_chunks;
             for(int i=0;i<upF.no_chunks;i++)
                cin>>upF.chunks[i];
             cin>>upF.fileName;
             long temp=send(client_socket,&upF,sizeof(struct uploadFile),0);
             if(temp>0)
                cout<<"successfully uploaded enter next command"<<endl;

            }

        }
        else continue;

       
    }
    close(client_socket);

    } 
