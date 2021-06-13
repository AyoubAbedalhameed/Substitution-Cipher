

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include<stdbool.h>
 #include <netdb.h>

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





int main (int argc, char** argv)
{

fflush(stdin); 



// Checking command-line arguments count. (Must be 2)
if (argc != 3) {printf("Invalid Argument Count, usag: Server <Listining Port>"); return -1 ; } 

/*
printf("Given Serevr IP: %s\n" , argv[1]) ;
printf("Given Server Port: %s\n" , argv[2]) ;
*/
//Initializing SOcket Address Structure: 

struct in_addr **pptr;
struct in_addr *inetaddrp[2];
struct in_addr inetaddr;
struct hostent *hp;
struct servent *sp;
struct sockaddr_in ServerAddrStructure ; 

socklen_t addrlen ; 




int Choice ; 

printf("UDP Service Probably won't work\n") ; 
printf("Please choose one of the following:-\n1. Encrypt a message (TCP Client)\n2. Decrypt a message (UDP Client)\n3. Exit\n");
char pug ; 
while(1)
{
 scanf("%d%c",&Choice,&pug) ;     if(Choice >3 || Choice <1) {printf("Invalid Choice, Please Try Again\n") ;fflush(stdin);}  
    else break;
}

if(Choice == 3 ) {printf("You Choosed Exit, GoodBye.\n") ; return 0 ;  }


char rcvbuff[100] ; 
char sendbuff[100] ; 
int MySock ; 


if ( (hp = gethostbyname(argv[1])) == NULL) {
if (inet_aton(argv[1], &inetaddr) == 0) {
printf("hostname error for %s\n", argv[1]) ; 
hstrerror(h_errno);
} else {
inetaddrp[0] = &inetaddr;
inetaddrp[1] = NULL;
pptr = inetaddrp;
}
} else {
pptr = (struct in_addr **) hp->h_addr_list;
}

if ( (sp = getservbyname(argv[2], "tcp")) == NULL)
{printf("getservbyname error for %s", argv[2]); return -1 ; } 

memset(&ServerAddrStructure, 0 , sizeof(ServerAddrStructure)) ;
ServerAddrStructure.sin_family= AF_INET ;
ServerAddrStructure.sin_port = sp->s_port;
memcpy(&ServerAddrStructure.sin_addr, *pptr, sizeof(struct in_addr));

const int on = 1;

bool flag = false ; 

if (Choice == 1)
{
//TCP Client Code: 

/*

if ( (hp = gethostbyname(argv[1])) == NULL) {
if (inet_aton(argv[1], &inetaddr) == 0) {
printf("hostname error for %s\n", argv[1]) ; 
hstrerror(h_errno);
} else {
inetaddrp[0] = &inetaddr;
inetaddrp[1] = NULL;
pptr = inetaddrp;
}
} else {
pptr = (struct in_addr **) hp->h_addr_list;
}

if ( (sp = getservbyname(argv[2], "tcp")) == NULL)
{printf("getservbyname error for %s", argv[2]); return -1 ; } 
*/

for ( ; *pptr != NULL; pptr++) {

MySock = socket( AF_INET,  SOCK_STREAM, 0); 
if(MySock<0)
{perror("Error While Creating Client Socket%d"); return -1 ;}
printf("Socket Created Successfully, SocketNum= (%d)\n",MySock); 
setsockopt(MySock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

/*
memset(&ServerAddrStructure, 0 , sizeof(ServerAddrStructure)) ;
ServerAddrStructure.sin_family= AF_INET ;
ServerAddrStructure.sin_port = sp->s_port;
memcpy(&ServerAddrStructure.sin_addr, *pptr, sizeof(struct in_addr));


*/


printf ("Built Server Address: (%s)\n", inet_ntoa(ServerAddrStructure.sin_addr)) ; 


if (connect(MySock,(struct sockaddr*) &ServerAddrStructure, sizeof(ServerAddrStructure)) == 0)
break;
{perror("Error While Connecting to the Server: "); return -1 ; }



// Connecting To the Server: 
/*
if ( connect(MySock, (struct sockaddr*) &ServerAddrStructure, sizeof(ServerAddrStructure)) < 0)
{perror("Error While Connecting to the Server: "); return -1 ; }
*/

printf("Connection Succes\n") ; 
}


do{

    
memset(sendbuff, 0 , sizeof(sendbuff)) ; 
memset(rcvbuff, 0 , sizeof(rcvbuff)) ; 

printf("Please Enter Your Plaintext Message or Enter EXIT to Quit. \n") ; 
printf("Lowercase English Alph' only.. \n") ; 

fgets( sendbuff , sizeof(sendbuff), stdin ); 
printf("%s", sendbuff) ; 

sendbuff[strlen(sendbuff)-1] = '\0' ; 
if (strcmp("exit", sendbuff) == 0 ) 

{printf("You Choosed exit, exiting .. \n"); flag = true ; }
printf("(%s)", sendbuff) ; 

if (!flag) 
{cipher(sendbuff, sendbuff, 0) ;} 

printf("Sending--> (%s) to the server \n", sendbuff) ; 


if (write (MySock, sendbuff, strlen(sendbuff)) == -1)
printf ("Error while Writing to the socket.\n") ;

if (!flag) 

if (read (MySock, rcvbuff, sizeof(rcvbuff)) == -1)
{perror("Error while reading socket:") ; return -1 ; } 

printf("(%s, %d)", inet_ntoa(ServerAddrStructure.sin_addr) ,(int)ntohs(ServerAddrStructure.sin_port) ) ; 
printf("  %s\n", rcvbuff) ; 
} while (!flag  ) ;


close(MySock) ;  

} 
else 
{
//UDP Client Code: 
int MySock_UDP ; 
MySock_UDP = socket(AF_INET, SOCK_DGRAM, 0);
setsockopt(MySock_UDP, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



do{

    
memset(sendbuff, 0 , sizeof(sendbuff)) ; 
memset(rcvbuff, 0 , sizeof(rcvbuff)) ; 

printf("Please Enter Your Plaintext Message or Enter EXIT to Quit. %lu \n", sizeof(sendbuff)) ; 
fgets( sendbuff , sizeof(sendbuff), stdin ); 
printf("%s", sendbuff) ; 

sendbuff[strlen(sendbuff)-1] = '\0' ; 
if (strcmp("exit", sendbuff) == 0 ) 

{printf("You Choosed exit, exiting .. \n"); flag = true ; }
printf("(%s)", sendbuff) ; 

if (!flag) 
{cipher(sendbuff, sendbuff, 1) ;} 

printf("Sending--> (%s) to the server \n", sendbuff) ; 


if ( sendto(MySock_UDP, sendbuff, strlen(sendbuff), 0, (struct sockaddr*) &ServerAddrStructure, sizeof(ServerAddrStructure)) == -1)
{perror("While Sending Datagram:"); return -1 ; }


if (!flag) 



if (recvfrom(MySock_UDP , rcvbuff , sizeof(rcvbuff) , 0, (struct sockaddr*) &ServerAddrStructure, &addrlen ) ==-1)
{perror("Error while reading socket:") ; return -1 ; } 


printf("(%s, %d)", inet_ntoa(ServerAddrStructure.sin_addr) ,(int)ntohs(ServerAddrStructure.sin_port) ) ; 
printf("  %s\n", rcvbuff) ; 
} while (!flag  ) ;

close(MySock_UDP) ; 



}

return 0   ; } 



