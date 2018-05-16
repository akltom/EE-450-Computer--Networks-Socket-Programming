#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>

char str[4][256];
int LinkCost[4][4];

char servName;

int TCP_serv_socket(int cliport, char *data, int size){
	
		int sockfd, n, servport;
		struct sockaddr_in cli_addr, serv_addr;
		struct hostent *client;
		char ipstr[16];
		char buffer[256];
		char *name = "ABCD";

		/* Create a socket point */
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		if (sockfd < 0) {
				perror("ERROR opening socket");
				exit(1);
		}
		client = gethostbyname("localhost");

		if (client == NULL) {
				perror("ERROR no such host");
				exit(0);
		}

		bzero((char *) &cli_addr, sizeof(cli_addr));
		cli_addr.sin_family = AF_INET;
		bcopy((char *)client->h_addr, (char *)&cli_addr.sin_addr.s_addr, client->h_length);
		cli_addr.sin_port = htons(cliport);

		/* Now connect to the client */
		if (connect(sockfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0) {
				perror("ERROR connecting");
				exit(1);
		}
		sleep(1);

		/* Write message to the client */
		n = write(sockfd, data, size);

		if (n < 0) {
				perror("ERROR writing to socket");
				exit(1);
		}
		
		inet_ntop(AF_INET, &cli_addr.sin_addr, ipstr, sizeof(ipstr));
		printf("The Server %c finishes sending its neighbor information to the Client with TCP port number %d and IP address %s.\n", servName, cliport, ipstr);
		
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));
		servport = ntohs(serv_addr.sin_port);
		printf("For this connection with the Client, the Server %c has TCP port number %d and IP address %s.\n", servName, servport, ipstr );

		close(sockfd);
		return 0;
}

int UDP_serv_socket(int servport){

		int sockfd, clilen, cliport;
		struct sockaddr_in cli_addr, serv_addr;
		int recvlen, sendlen;
		char buffer[256];
		char *ep,*bp;
		char ipstr[16];
		int i,j;
		int cnt = 0;
		char *name = "ABCD";
		if(servport == 21029)
			servName = 'A';
		if(servport == 22029)
			servName = 'B';
		if(servport == 23029)
			servName = 'C';
		if(servport == 24029)
			servName = 'D';

		/* First call to socket() function */
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sockfd < 0) {
				perror("ERROR opening socket");
				exit(1);
		}

		memset((char *)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(servport);

        /* bind the socket to any valid IP address and a specific port */
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                perror("bind failed");
                return 0;
        }
        printf("Bind success!\n");
		
		clilen = sizeof(cli_addr);
		
		bzero(buffer, 256);

		// listening to receive messages

		do{
				recvlen = recvfrom(sockfd, buffer, 255, 0, 
									(struct sockaddr*)&cli_addr, &clilen);
				// printf("Messages are %s\n", buffer);
				if( recvlen > 0) {	
						bp = buffer;
						for(i = 0; ep = strchr(bp, ' '); i++) {
								*ep = '\0';
								ep ++;
								LinkCost[cnt][i] = atoi(bp);
								bp = ep;
						}
						LinkCost[cnt][i] = atoi(bp);
				}
				else{
					perror("can't receive.");
					exit(-1);
				}
		}
		while(recvlen >= 0 && ++cnt<4);

		if ( cnt != 4 ) {
	        	perror("Not reveived enough messages!");
	        	exit(-1);
		}

		cliport = ntohs(serv_addr.sin_port);
		inet_ntop(AF_INET, &cli_addr.sin_addr, ipstr, sizeof(ipstr));
		printf("The server %c has received the network topology from the Client with UDP port number %d and IP address %s as follows:\n", servName, cliport, ipstr);
		printf("Edge------Cost\n");
		
		for(i=0; i<4; i++) {
				for(j=i+1; j<4; j++) {
						if(LinkCost[i][j])
							printf("%c%c      %d\n", name[i], name[j], LinkCost[i][j]);
				}
		}

		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));
		printf("For this connection with Client, The Server %c has UDP port number %d \
			and IP address %s.\n", servName, servport, ipstr);
		close(sockfd);
		
		return 0;

}

int read_store(char* name, char** line) {
        
        char* buffer = malloc(256 * sizeof(char));
        char dist[4][256];
        char *bp,*ep;
        int i,j,idx;

        FILE *fp = fopen(name, "r");
        if(fp == NULL){
                perror("Error opening file");
                return -1;
        }
        servName = name[6];
        // printf("%s created!\n", name);
        printf("The Server %c is up and running.\n", servName);
        printf("The Server %c has the following neighbor information:\n", servName);
        printf("Neighbor------Cost\n");

        for(i=0; i<4; i++){
                bzero(dist[i], 256);
        }
        bzero(buffer, 256);
        while (fgets(buffer, 20, fp) != NULL)
        {
                bp = strchr(buffer, ' ');
                bp++;
                ep = strrchr(buffer, '\n');
                if(strncmp(buffer, "serverA",7) == 0){
                    strncpy(dist[0], bp, ep-bp);
                    dist[0][ep-bp] = '\0';
                }   
                else if(strncmp(buffer, "serverB",7) == 0){
                    strncpy(dist[1], bp, ep-bp);
                    dist[1][ep-bp] = '\0';
                }
                else if(strncmp(buffer, "serverC",7) == 0){
                    strncpy(dist[2], bp, ep-bp);
                    dist[2][ep-bp] = '\0';
                }
                else if(strncmp(buffer, "serverD",7) == 0){
                    strncpy(dist[3], bp, ep-bp);
                    dist[3][ep-bp] = '\0';
                }
                printf("%s", buffer);
        }
        fclose(fp);
        
       	bzero(buffer, 256);
       	idx = 0;
       	buffer[idx++] = servName;
        for( i = 0; i < 4; i++ ) { 

                for( j = 0; dist[i][j] != '\0'; j++ )
                		// printf("buffer[%d] == %c\n", idx, buffer[idx]);
                        buffer[idx++] = dist[i][j];
                    	// printf("buffer[%d] == %c\n", idx-1, buffer[idx-1]);
			                
                if( j == 0 ) {
                		// printf("buffer[%d] == %c\n", idx, buffer[idx]);
                		buffer[idx++] = '0';
                		// printf("buffer[%d] == %c\n", idx-1, buffer[idx-1]);
                }
                // printf("buffer[%d] == %c\n", idx, buffer[idx]);
                buffer[idx++] = 32;
                // printf("buffer[%d] == %c\n", idx-1, buffer[idx-1]);
        }
        buffer[idx-1] = '\0';
        for(i = 0 ; i < idx; i++) {
        		// printf("buffer[%d] = %c\n", i, buffer[i]);
        }
        *line = buffer;

        return idx;
}
