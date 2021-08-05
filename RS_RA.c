#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 2152

#define SERVADDR "192.168.1.146"
#define CLIENTADDR "192.168.1.145" 
int main(void)
{
  int sock;
  socklen_t clilen;
  struct sockaddr_in server_addr, client_addr;
  char buffer[1024];
  
  char buf[1024] = {0x30, 0xff, 0x00, 0x30, 0x02, 0x00, 0x00, 0x20,/* GTP header */ 
                    0x60, 0x00, 0x00, 0x00, 0x00, 0x08, 0x3a, 0xff,/* IP HEader*/
                    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
                    0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, /* UDP header */
                    0x85, 0x00, 0x7d, 0x34, 0x00, 0x00, 0x00, 0x00};

char addrbuf[INET_ADDRSTRLEN];

  /* create a DGRAM (UDP) socket in the INET6 (IPv6) protocol */
  sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (sock < 0) {
    perror("creating socket");
    exit(1);
  }




  memset(&server_addr, 0, sizeof(server_addr));
  client_addr.sin_family = AF_INET;
  inet_pton(AF_INET, CLIENTADDR, &client_addr.sin_addr);
  client_addr.sin_port = htons(PORT);
  /* it is an INET address */
  server_addr.sin_family = AF_INET;

  /* the server IP address, in network byte order */
  inet_pton(AF_INET, SERVADDR, &server_addr.sin_addr);

  /* the port we are going to send to, in network byte order */
  server_addr.sin_port = htons(PORT);

  /* now send a datagram */
  if (sendto(sock, buf, 56, 0,
             (struct sockaddr *)&server_addr,
	     sizeof(server_addr)) < 0) {
      perror("sendto failed");
      exit(4);
  }

  printf("waiting for a reply...\n");
  clilen = sizeof(client_addr);
  if (recvfrom(sock, buffer, 1024, 0,
	       (struct sockaddr *)&client_addr,
               &clilen) < 0) {
      perror("recvfrom failed");
      exit(4);
  }

  printf("got '%s' from %s\n", buffer,
	 inet_ntop(AF_INET, &client_addr.sin_addr, addrbuf,
		   INET_ADDRSTRLEN));

  /* close socket */
  close(sock);

  return 0;
}
