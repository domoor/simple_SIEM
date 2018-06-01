#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
using std::string;

#define BUF_SIZ		1024
//#define FILE_PATH	"/root/zaproxy/"
//#define FILE_NAME	"test_0000.xml"
#define FILE_PATH	"/opt/splunk/test/"
#define FILE_NAME	"test_0000.xml"

int vul_result(int sock){
    int res, len;

    res = recv(sock, (char*)&len, 10, 0);
    if(res < 0) {
        perror("recv1 failed !");
	return -1;
    }
    char *buf = (char*)malloc(len+1);

    res = recv(sock, buf, len, 0);
    if(res < 0) {
        perror("recv2 failed !");
	return -1;
    }
    buf[len] = '\n';

    string file = FILE_PATH;
    file += FILE_NAME;
    FILE * wfp = fopen(file.c_str(), "w");
    fwrite(buf, 1, len, wfp);
    fclose(wfp);
    free(buf);

    puts("검사가 완료 되었습니다.");
}

int command_srv(int sock){
    int res;
    int start;

    while(1) {
	puts("1\t:start");
	puts("default\t: exit");
	printf("Input : ");
        scanf("%d",&start);
        switch(start) {
	case 1:
	    puts("취약점 검사를 시작합니다.");
	    res = send(sock,"start\n",6,0);
	    if(res < 0) {
        	perror("send failed !");
		return -1;
	    }
	    if(res = vul_result(sock) < 0) return res;
	    break;
	default:
	    puts("프로그램을 종료합니다.");
	    return 0;
 	}
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in servaddr, cliaddr;
    int sockfd; // socket descripter
    int acc_sock;   // accept_socket
    socklen_t addrlen = sizeof(cliaddr);

    if(argc != 2)
    {
        printf("Usage: %s <port> \n", argv[0]);
        return -1;
    }

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed !");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed !");
        return -1;
    }

    if(listen(sockfd, 1) < 0)
    {
	    perror("listen failed !");
	    return -1;
    }
    puts("Listening.....");

    acc_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen);
    if(acc_sock < 0)
    {
        perror("accept failed !");
        return -1;
    }
    puts("Connected client");

    int res;
    if(res = command_srv(acc_sock) < 0) return res;

    close(sockfd);
    return 0;
}
