#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include "peer.h"
#include<map>
using namespace std;
#define SIZE 100

// struct sktData{
//             char *ipAddr;
//             int port;
//         };


struct fileData{
    int chunks[1000];
    int usrid[10];
    char fileName[100];
    char sha1[20];
};
struct userData{
    int userid;
    int password;
    int port;
    int grpid;
    int grp_owner[100];
    char ip_add[20];
    char fileNames[10][200];
};
struct grpData{
    int grpId;
    int ownerId;
    int users[10];
    char files[10][200];
};
map<int,userData>userMap;
map<int,grpData>grpMap;
map<string,fileData>fileMap;
void create_user(int usrid,int psswd,int prt,int grpid,int grpOwn,char *ipAdd)
{   int ret_in,ret_out;
    struct userData cr;
    cr.userid=usrid;
    cr.port=prt;
    cr.password=psswd;
    cr.grpid=grpid;
    cr.grp_owner[100]={grpOwn};
    strcpy(cr.ip_add,ipAdd);
    //cr.fileNames[10][]={"NULL"};
    cout<<"strt::ip="<<cr.ip_add<<endl;
    char userFile[20]="user";
    char temp[10];

    sprintf(temp,"%d",usrid);
    strcat(userFile,temp);
    cout<<"file::"<<userFile<<endl;
    //strcpy(cr.ip_add,ip);
    // for(int i=0;i<10;i++)
    //     cr.fileNames[i]="NULL";
    char buf[sizeof(struct userData)];
    memcpy(buf,&cr,sizeof(struct userData));
    int fd=open(userFile,O_WRONLY|O_CREAT|O_TRUNC,0644);
            ret_out=write(fd,buf,sizeof(struct userData));
    cout<<"ret_out::"<<ret_out<<endl;

}
int login_user(int userid, int passwd){
    char buf[sizeof(struct userData)];
    char userFile[20]="user";
    char temp[10];
    sprintf(temp,"%d",userid);
    strcat(userFile,temp);
    int fd=open(userFile,O_RDONLY);
    struct userData ud;
    int ret_in=read(fd,buf,sizeof(struct userData));
    memcpy(&ud,buf,sizeof(struct userData));
    if(ud.userid==userid && ud.password==passwd)
        return 1;
    return 0;    
}
int main(){
    while(1)
{
    cout<<"Menu"<<endl;
    cout<<"1. Create user"<<endl;
    cout<<"2.Login"<<endl;
    cout<<"3.create group"<<endl;
    cout<<"4.Join Group"<<endl;
    int choice;
    int grpid, usrid, pswd,port;
    cin>>choice;
    if(choice==1){
        cout<<"enter grpid, userid, password,port,ip"<<endl;
        
        char ip[20];
        cin>>usrid>>pswd;
        create_user(usrid,pswd,-1,-1,-1,"\0");
    }
    else if(choice==2)
    {
        cout<<"enter userid, passowd"<<endl;
            cin>>usrid>>pswd;
            int flag=login_user(usrid,pswd);
            if(flag)
            {   cout<<"enter port and ip"<<endl;
                
                int prt;
                char ip[20];
                cin>>prt>>ip;
                create_user(usrid,pswd,prt,-1,-1,ip);
                struct sktData *sD=(struct sktData *)malloc(sizeof(struct sktData));
                sD->port=prt;
                sD->ipAddr=ip;
                pthread_t ts;
                pthread_create(&ts,NULL,server,sD);
             }
            else{
                cout<<"wrong credentials or not registered"<<endl;
            } 

    }
    else if(choice==3){

    }
}

struct userData u_d;


// fd.chunks.push_back(1);
// fd.chunks.push_back(2);
// fd.chunks.push_back(3);
// fd.fileName="exp.txt";
// fd.port=9002;
// struct fileData fd1;
// fd1.chunks.push_back(4);
// fd1.chunks.push_back(5);
// fd1.chunks.push_back(6);
// fd1.fileName="exp.txt";
// fd.port=9003;
}
