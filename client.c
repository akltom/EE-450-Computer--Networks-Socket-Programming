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

int LinkCost[4][4];
char str[4][256];
char* name="ABCD";
int count = 0;

void printMinCostEdges(int* nodes, int size){
      int i,cost = 0 ;
      
      for(i = 1; i < size; i++)
      {
            cost += LinkCost[i][nodes[i]]; 
      }
      printf("The Client has calculated a tree. The tree cost is %d\n", cost);
	  printf("Edge------Cost\n");
      for(i = 1; i < size; i++)
      {
            printf("%c%c      %d\n", name[nodes[i]], name[i], LinkCost[i][nodes[i]]);
      }

}

void Prim(int vn)
{
      int i, j, k, x, y;
      int reached[vn];	// Reach/unreach nodes
      int predNode[vn];		// Remember min cost edge
      int minEdgeCost = 1000;
      
      reached[0] = 1;


      for ( i = 1; i < vn; i++ )
      {
            reached[i] = 0;
      } 

      predNode[0] = 0;  
      
      for ( k = 1; k < vn; k++) // Loop vn-1 times (UnReachSet = empty) 
      {
            
            x = y = 0;
            for ( i = 0; i < vn; i++ ){
                  
                  for ( j = i; j < vn; j++ )
                  {
                        if( LinkCost[i][j] == 0 )
                              LinkCost[i][j] = 1000;

                  		if ( reached[i] && ! reached[j] && 
                                       LinkCost[i][j] < LinkCost[x][y] )
                  		{
                                     x = i;     // Link (i,j) has lower cost !!!
                                     y = j;
                  		}
                  }
            }
            predNode[y] = x;
            reached[y] = 1;
      }

      printMinCostEdges( predNode, vn );    // Print the min, cost spanning tree

}

int TCP_cli_socket(int cliport){
	
	int sockfd, newsockfd, servport, servlen;
	char buffer[256];
	struct sockaddr_in cli_addr, serv_addr;
	int  i,j,n,cnt,idx;
	char ipstr[16];

	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
			perror("ERROR opening socket");
			exit(1);
	}

	/* Initialize socket structure */
	bzero((char *) &cli_addr, sizeof(cli_addr));

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = INADDR_ANY;
	cli_addr.sin_port = htons(cliport);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
			perror("ERROR on binding");
			exit(1);
	}
	
	inet_ntop(AF_INET, &cli_addr.sin_addr, ipstr, sizeof(ipstr));
	printf("The client has TCP port number %d and IP address %s \n", cliport, ipstr);

	/* Now start listening for the server, here process will
	* go in sleep mode and will wait for the incoming connection
	*/
	listen(sockfd,5);
	
	cnt = 0;
	char *ep,*bp;
	for(i=0; i<4; i++) {
		bzero(str[i], 256);
	}
	
	while(cnt<4){
			
			servlen = sizeof(serv_addr);
			/* Accept actual connection from the server */
			
			newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr, &servlen);
			if (newsockfd < 0) {
					perror("ERROR on accept");
					exit(1);
			}

			inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));
			servport = ntohs(serv_addr.sin_port);

			printf("The Client receivers neighbor information from the Server %c with TCP port number %d and IP address %s as follows:\n", name[cnt], servport, ipstr);
			printf("Neighbor------Cost\n");
			
			/*char ipstr[16];
			int port;	
			inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof ipstr);
			printf("Peer IP address: %s\n", ipstr);
			port = ntohs(serv_addr.sin_port);
			printf("Peer port      : %d\n", port);*/

			bzero(buffer, 256); // initialize buffer to zero
			n = read(newsockfd, buffer, 255);
			if (n < 0) {
					perror("ERROR reading from socket");
					exit(1);
			}
			
			bp = buffer+1;
			switch(buffer[0]) {
				case 'A': idx = 0; break; 
				case 'B': idx = 1; break;
				case 'C': idx = 2; break;
				case 'D': idx = 3; break;
			}

			strncpy(str[idx], bp, 255);

			for(i = 0; ep = strchr(bp, ' '); i++) {
					*ep = '\0';
					ep ++;
					LinkCost[idx][i] = atoi(bp);
					bp = ep;
			}
			LinkCost[idx][i] = atoi(bp);
			for(i = 0; i < 4; i++) {
					if(LinkCost[idx][i]) {
							printf("%c%c      %d\n", name[idx], name[i], LinkCost[idx][i]);
					}
			}
			inet_ntop(AF_INET, &cli_addr.sin_addr, ipstr, sizeof(ipstr)); // Get client address in a string form
			printf("For this connection with Server %c, The Client has TCP portnumber %d and IP address %s.\n", name[idx], cliport, ipstr);
			cnt++;
			close(newsockfd);
	}
	return 0;
}

int UDP_cli_socket(int servport) {

		int sockfd, newsockfd, servlen;
		char buffer[256];
		struct sockaddr_in cli_addr,serv_addr;
		int  recvlen, sendlen;
		struct hostent *server;
		char ipstr[16];
		int i, j, cnt;

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("cannot create socket\n");
                return 0;
        }

        server = gethostbyname("localhost"); //nunki.usc.edu

		if (server == NULL) {
				perror("ERROR no such host");
				exit(0);
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); // Copy the information to these three items
		serv_addr.sin_port = htons(servport);

        servlen = sizeof(serv_addr);
        cnt = 0;
        sleep(3);
    	do {

    		//sendto is not a blocking call, so the receiver may miss some messages
    		sendlen = sendto(sockfd, str[cnt], 256, 0, 
    								(struct sockaddr *)&serv_addr, servlen);
    		sleep(1);
    		cnt++;
    	}
    	while(sendlen >=0 && cnt<4);

    	if ( cnt != 4 ) {
	        	perror("send failed");
	        	exit(-1);
		}

    	inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr)); // Convert to string for printing purpose
    	printf("The Client has sent the network topology to the network topology to the Server %c with UDP port number %d and IP address %s as follows:\n", name[count++], servport, ipstr);
    	printf("Edge------Cost\n");
    	
    	for(i=0; i<4; i++) {
				for(j=i+1; j<4; j++) {
						if(LinkCost[i][j])
							printf("%c%c      %d\n", name[i], name[j], LinkCost[i][j]);
				}
		}

        close(sockfd);
        return 0;

}

int main ()
{		
		
		TCP_cli_socket(25029);
		UDP_cli_socket(21029);
		UDP_cli_socket(22029);
		UDP_cli_socket(23029);
		UDP_cli_socket(24029);
		
		Prim(4); // for generating min spaning tree purpose

		return 0;
	
}


   

