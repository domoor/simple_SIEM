#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sstream>
#include <malloc.h>

using std::string;

#define BUF_SIZ		1024
#define SH_PATH		"/usr/share/zaproxy/"
#define TARGET		"http://127.0.0.1:8000"
#define FILE_PATH	"/root/zaproxy/"
#define FILE_NAME	"test_0000.xml"

int file_create(int sock){
    int res;
    puts("취약점 점검을 3초 뒤에 실행합니다.");
    sleep(3);

    string command;
    command = SH_PATH;
    command += "zap.sh -cmd -quickurl \\ ";
    command += TARGET;
    command += " -quickprogress -quickout ";
    command += FILE_PATH;
    command += FILE_NAME;
    puts(command.c_str());
    system(command.c_str());
  
    string file = FILE_PATH;
    file += FILE_NAME;
    FILE *rfp = fopen(file.c_str(), "r");
    if(rfp == NULL) {
        perror("file read failed !");
	return -1;
    }
    fseek(rfp, 0,SEEK_END);
    int len = ftell(rfp);
    fseek(rfp, 0, SEEK_SET);

    char *buf = (char*)malloc(len);
    fread(buf,1,len,rfp);
    fclose(rfp);
    
    char length[10];
    sprintf(length, "%d", len);

    res = send(sock, (char*)&len, strlen(length), 0);
    if(res < 0) {
	perror("send1 failed !");
	return -1;
    }
    printf("size = %d\n", res);
    puts("2초간 대기합니다.");
    sleep(2);

    res = send(sock, buf, len, 0);
    if(res < 0) {
	perror("send1 failed !");
	return -1;
    }

    puts("점검 내역이 서버로 전송되었습니다.");
    free(buf);
}

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in servaddr;

    if(argc != 3)
    {
        printf("Usage: %s <ip_address> <port>\n", argv[0]);
        return -1;
    }

    if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("socket failed !");
        return -1;
    }
    
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));

    // 연결 요청,,
    if(connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Connection failed !");
        return -1;
    }
    puts("connect success !");

    while(1) {
        char buf[BUF_SIZ];
    	int res = recv(sock, buf, BUF_SIZ, 0);
    	if(strncmp(buf, "start\n", res)) {
	    perror("size error !");
	    return -1;
    	}
	file_create(sock);
    }

    close(sock);
    return 0;
}
