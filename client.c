#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
// --------------------------------
int isNum(const char*, int);
void freeDataMalloced(char *request,char *path, char *host, char **rParameter, char *pContent);
//---------------------------------
int main(int argc, char *argv[]){
    if (argc <= 1){//check  entered  input length
        printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
        return 1;
    }
    int fd;//file descriptor table  "socket door"
    char *host = NULL;//the host name
    char *path = NULL;//the path
    char *pContent = NULL;//the content that come after the  parameter p
    int pContentLength = 0;//the length of  p content
    char *pContentLengthStr = NULL;//the length of  p content in chars in order to add to the request
    char **rParameter = NULL;//the r parameter content  rParameter[0]="parameter=value".....
    int rParaLength = 0;// the number of the parameter that come after r
    int port = 80;
    char *portStr = NULL;
    //sending part
    int LengthApproximation_Message = 0;//this number bigger then the actual length of the request  I use it to do malloc
    char *request = NULL;//the http request
    //receiving part
    int receiveMessage_length = 0;//the number of read  chars into buffer
    for ( int i = 1 ; i < argc; i++){//run over the argc array element
        if (strcmp(argv[i], "-r") == 0){
            if(isNum(argv[ i + 1 ], (int)strlen(argv[ i + 1 ])) == 0 ){//number after r
                printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                exit(1);
            }
            rParaLength= atoi(argv[ i + 1 ]);//length of r parameter
            rParameter = (char**) malloc(sizeof(char*) * rParaLength);//start the parameter
            if (rParameter == NULL){
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }

            for ( int j = 0; j < rParaLength; ++j) {//take the parameter and check the format
                if(strstr(argv[ i + 2 + j],"=") == NULL){
                    printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                    freeDataMalloced(request,path,host,rParameter,pContent);
                    exit(1);
                }
                rParameter[j] = argv[ i + 2 + j];
                LengthApproximation_Message += (int)strlen(argv[ i + 2 + j]);
            }

            if ((i + 2 + rParaLength) < argc){//if the next parameter that come after the r parameter with format name=value
                if(strstr(argv[i + 2 + rParaLength],"=") != NULL){
                    printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                    freeDataMalloced(request,path,host,rParameter,pContent);
                    exit(1);
                }
            }

            i += rParaLength + 1;//jump
        }
        else if (strcmp(argv[i], "-p") == 0){
            if(isNum(argv[ i + 1 ], (int)strlen(argv[ i + 1 ])) == 0 ){
                printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            pContentLengthStr = argv[ i + 1 ];
            pContentLength = atoi(argv[ i + 1 ]);
            if(pContentLength == 0){
                printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            if(pContentLength > strlen(argv[ i + 2 ])){
                printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            pContent = (char*) malloc(sizeof(char) * (pContentLength + 1));
            if (pContent == NULL){
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            for (int j = 0; j < pContentLength; ++j) {
                pContent[j] = argv[ i + 2 ][j];
            }
            pContent[pContentLength]='\0';
            LengthApproximation_Message += pContentLength;
            i += 2;//jump
        }else {
            for (int j = 0; j < 7; ++j) {//check the start
                if (argv[i][j] != "http://"[j]) {
                    freeDataMalloced(request,path,host,rParameter,pContent);
                    printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
                    exit(1);
                }
            }
            host = (char *) malloc(sizeof(char) * strlen(argv[i]));//malloc with max size can be for host
            if (host == NULL){
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            path = (char *) malloc(sizeof(char) * strlen(argv[i]));//malloc with max size can be for path
            if (path == NULL){
                freeDataMalloced(request,path,host,rParameter,pContent);
                exit(1);
            }
            strcpy(host, &argv[i][7]);//copy the chars after the http://
            strcpy(path, "/");// the default value of the path
            if(strchr(host, '/') !=NULL){
                strcpy(path, strchr(host, '/'));//copy the path from the host - host contain host / path
            }
            char * freeHost= host;
            host = strtok(host, "/");// now host contain just the host
            if (host == NULL){
                freeDataMalloced(request,path,freeHost,rParameter,pContent);
                exit(1);
            }
            portStr = strchr(host, ':');//find the port number
            if (portStr != NULL) {
                port = atoi(&portStr[1]);
                if (port > 65536 || port < 0)// check the port number range
                    printf("port number bigger than 2^16 or less than 0 \n");
                host = strtok(host, ":");// now host contain just the host without the port

            }
            LengthApproximation_Message += (int)strlen(host) + (int)strlen(path);
        }
    }
    if (host == NULL){
        printf("Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL> \n");
        freeDataMalloced(request,path,host,rParameter,pContent);
        exit(1);
    }
    LengthApproximation_Message += pContentLength + 77; //calculate the approximation of the length request 77 is random number
    request = (char*)malloc(LengthApproximation_Message * sizeof(char));//start the request
    if (request == NULL){
        freeDataMalloced(request,path,host,rParameter,pContent);
        exit(1);
    }
    request[0]='\0';
    if(pContent != NULL)//if the p content was started before this mean --> post request
        strcat(request,"POST ");
    else
        strcat(request,"GET ");
    if(path != NULL)
        strcat(request,path);
    if(rParaLength !=0) {//append  r parameter
        strcat(request,"?");
        for (int i = 0; i < rParaLength; ++i) {
            strcat(request,rParameter[i]);
            if(i != rParaLength -1)
                strcat(request,"&");
        }
    }
    strcat(request," HTTP/1.0\r\nHost: ");
    if(host != NULL)
        strcat(request,host);
    if(pContent != NULL) {//if the request of type post
        strcat(request, "\r\nContent-length:");
        strcat(request, pContentLengthStr);
        strcat(request, "\r\n\r\n");
        strcat(request, pContent);
    }
    else{//if the request of type get
        strcat(request, "\r\n\r\n");
    }
    //TCP
    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0){
        freeDataMalloced(request,path,host,rParameter,pContent);
        perror("socket");
        exit(1);
    }
    struct sockaddr_in *socketAddress = (struct sockaddr_in*)malloc( sizeof( struct sockaddr_in ) );
    if (socketAddress == NULL){
        close(fd);
        freeDataMalloced(request,path,host,rParameter,pContent);
        exit(1);
    }
    struct hostent *idByName = gethostbyname(host);
    if (idByName == NULL){
        close(fd);
        free(socketAddress);
        freeDataMalloced(request,path,host,rParameter,pContent);
        herror("id");
        exit(1);
    }
    struct in_addr *pAdrr= (struct in_addr*) (idByName -> h_addr);
    socketAddress -> sin_addr.s_addr = pAdrr -> s_addr;
    socketAddress -> sin_family = AF_INET;
    socketAddress -> sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)socketAddress, sizeof(*socketAddress)) < 0){
        close(fd);
        freeDataMalloced(request,path,host,rParameter,pContent);
        free(socketAddress);
        perror("connect");
        exit(1);
    }
    printf("HTTP request =\n%s\nLEN = %d\n", request, (int)strlen(request));
    if (write(fd, request, (int)strlen(request)) < 0){
        close(fd);
        freeDataMalloced(request,path,host,rParameter,pContent);
        free(socketAddress);
        perror("write");
        exit(1);
    }



    int return_read_val ;
    unsigned char c = '\0'; // char to read
    while ( 1 )
    {
        if ( ( return_read_val = (int) read( fd, & c, 1 ) ) == -1)
        {
            close(fd);
            freeDataMalloced(request,path,host,rParameter,pContent);
            free(socketAddress);
            perror("read");
            exit(1);
        }
        if ( return_read_val == 0 )
            break;
        printf("%c", c);
        receiveMessage_length += 1;
    }


    printf("\n Total received response bytes: %d\n",receiveMessage_length);
    freeDataMalloced(request,path,host,rParameter,pContent);
    free(socketAddress);

    return 0;
}
void freeDataMalloced(char *request, char *path, char *host, char **rParameter, char *pContent) {
    if(path != NULL)
        free(path);
    if(host != NULL)
        free(host);
    if(rParameter != NULL){
        free(rParameter);}
    if(pContent != NULL)
        free(pContent);
    if(request != NULL)
        free(request);
}


int isNum(const char* str , int len) {
    int j = 0;
    while(j < len ){
        if(str[j] > 57 || str[j] < 48){
            return 0;
        }
        j++;
    }
    return 1;
}




