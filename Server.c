
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <sys/select.h>
#include <sys/time.h>



void SigChld_Handler ()
{
pid_t pid;
int State;
while ( (pid = waitpid(-1, &State, WNOHANG)  ) > 0)
printf("Child:[%d] Terminated, \n", pid);
return;
}


// ********************* Cipher **************************************




int cipher (char* input, char* output, int mod)
{
unsigned long size ;   
char key[7];
char index = 0 ; 
printf("Please Enter a Key\n") ; 

int flag = 0 ; 
while (1)
{
    flag = 0 ;
fgets(key, 100 , stdin) ;
printf("%d", strlen(key)) ; 

if ( (int) strlen(key) != 8 )
{ printf("THe length of the given key = %lu, must be 7, Try Again. \n", strlen(key)-1) ;  continue ;} 


for (int i = 0 ; i < 7 ; i++)
if (key[i]< 'a' || key[i] > 'z')
{printf ("Invalid Key, all characters must be lowercase, Try Again. \n");   flag = 1 ; break; }

if(flag) continue; else break ;
}



if (mod ==  0 )
for (int i = 0 ; i < strlen(input) ; i++ )
output[i] = ((input[i] + key[i%7] - 194)% 26) + 97 ; 

else if (mod == 1)
for (int i = 0 ; i < strlen(input) ; i++ )
output[i] =  (((input[i] - 97) + (26-(key[i%7]-97))) % 26) + 97 ;    

else {printf("In Cipher: INVALID CIPHER MODE\n"); return -1 ; }


printf("Cipher Text: (%s)\n", output) ; 
  return 0 ; 
}




// ********************* UDP **************************************

int ServingUdpClient  (int fd , char* SendBuff , char* ReceiveBuff)
 {
size_t Received ; 
struct sockaddr_in  ClientAdd ; 
socklen_t addrlen ; 

memset( &ClientAdd, 0, sizeof(ClientAdd));

while (1)
{
printf("Waiting For Client Request .. \n") ; 

Received = recvfrom(fd , ReceiveBuff , 99 , 0, (struct sockaddr*) &ClientAdd, &addrlen ); 
printf("Received (%s)", ReceiveBuff) ; 

cipher(ReceiveBuff , SendBuff , 0 ) ; 
printf("Sending: \"%s\" To the Client ", SendBuff) ; 

 sendto(fd, SendBuff, strlen(SendBuff), 0, (struct sockaddr*) &ClientAdd, addrlen);
}


return 0 ; 
}



// ********************* TCP **************************************




int ServClient_TCP (int  TcpServingSock, char* SendBuff, char* ReceiveBuff,  struct sockaddr_in*  ClientAdd)  
{
    while (1)
    {

      memset(SendBuff, 0 , 99) ; 
      memset(ReceiveBuff, 0 , 99) ; 

printf("Waiting for (%s) Request: \n", inet_ntoa((*ClientAdd).sin_addr)) ;


if (read (TcpServingSock, ReceiveBuff, 99) == -1 ) 
if( errno == EINTR ) continue ;
else { perror("While Reading Socket"); return -1 ; }

printf("@[%d]: :(%s, %d):", getpid(),  inet_ntoa((*ClientAdd).sin_addr)  , (int) ntohs((*ClientAdd).sin_port) ) ; 
printf(" (%s)\n",   ReceiveBuff);



if(strcmp(ReceiveBuff , "exit")==0) 
{printf   (   "Client Sent EXIT, Child terminates .. \n"  ) ; break ; }

cipher (ReceiveBuff, SendBuff, 1 ) ; 

SendBuff[strlen(SendBuff)] = '\0'  ; 
printf("Plain Text: (%s)\n", SendBuff) ; 
if (write ( TcpServingSock, SendBuff, strlen(SendBuff)) == -1)
 {perror("Error While Writing To the Socket: "); exit(-1) ; }
    
} 
    return 0 ; 

}

int Max (int x , int y)
{if (x>=y) return x ; else return y ; }




// ******************** Main ***************************






int main (int argc, char** argv)
{


printf("I've bind the port 65025 as local server port Regardless of the port you have entered, DNS does not work here on the server but it works well at the client,\
nso please edit your /etc/hosts and add (nes416_port     65025/tcp) to check the DNS at the client.\n") ; 


if (argc != 2 ) {printf("Invalid Argument Count, usag: Server <Listining Port>"); return -1 ; } 

struct sockaddr_in  ServerLocal ; 
fd_set fdReadSet ; 


char SendBuff [100] ; 
char ReceiveBuff [100] ; 
char Key[7] ; 
int TcpServingSock ; 

signal (SIGCHLD, SigChld_Handler) ; 



memset( &ServerLocal, 0, sizeof(ServerLocal));
ServerLocal.sin_family = AF_INET; 
ServerLocal.sin_port= htons(atoi("65025") ); 
ServerLocal.sin_addr.s_addr= INADDR_ANY; 


int TcpSock = socket(AF_INET, SOCK_STREAM, 0 ); 
if(TcpSock<0) 
{ perror("Error While Creating Server Socket\n");   return -1 ;}
 printf("TCP Socket Created Successfully\n"); 

int UdpSock = socket(AF_INET, SOCK_DGRAM, 0 );  
if(UdpSock<0) 
{ perror("Error While Creating Server Socket\n");   return -1 ;}
 printf("UDP Socket Created Successfully\n"); 





//Binding Socket 
if (  bind(TcpSock, (struct sockaddr*) &ServerLocal, sizeof(ServerLocal))      <0)
    { perror("Warnning:"); }

if (  bind(UdpSock, (struct sockaddr*) &ServerLocal, sizeof(ServerLocal))      <0)
    { perror("Warnning:"); }


//Listen on TCP socket
if (listen(TcpSock, 5)<0)
     { perror("Error While Binding the Socket to the local address"); return -1 ; }
printf("TCP Socket State switched To: PASSIVE_LISTENNING\n") ; 

 struct sockaddr_in  ClientAdd ; 
memset( &ClientAdd, 0, sizeof(ClientAdd));
socklen_t ClientAddLength ; 
int ServingTCPSock ; 


while (1)
{
//Select multiplexing Initialization  
FD_ZERO    (&fdReadSet);               /*clear all bits in fdset */
FD_SET     (TcpSock, &fdReadSet);  /*Turn on the bit related to desc' fd */
FD_SET     (UdpSock, &fdReadSet);  /*Turn on the bit related to desc' fd */


    printf("\nWaiting for Client Request ..  \n") ; 



if(select(Max(TcpSock,UdpSock)+1, &fdReadSet, NULL, NULL, NULL)<0)
if (errno == EINTR) continue ; else
{perror("in Select:");  return -1 ; }


if(FD_ISSET (TcpSock, &fdReadSet )) 
{


TcpServingSock = accept(TcpSock, (struct sockaddr * ) &ClientAdd, &ClientAddLength);
if (TcpServingSock<0)
if( errno == EINTR || errno == ECONNABORTED) continue ; //Handling the case of accept system calll interruption
else { perror("While Accepting Connection"); return -1 ; }
printf("Connection Accepted from: %s \n", inet_ntoa(ClientAdd.sin_addr) ) ;


if (fork () == 0) 
{ close(TcpSock) ;   printf("Child #(%d) forked!\n", getpid()) ; 

if (   ServClient_TCP (TcpServingSock, SendBuff, ReceiveBuff,  &ClientAdd)  == -1) 
exit (-1) ;  exit(0) ;

 }}


else if(FD_ISSET (UdpSock, &fdReadSet )) 
{
//Call UDP Client 

 ServingUdpClient  (UdpSock , SendBuff ,  ReceiveBuff) ; 
 close(UdpSock) ; 
}

}


return 0 ; } 




