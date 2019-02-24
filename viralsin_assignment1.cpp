/**
 * @viralsin_assignment1
 * @author  Viral Sinha <viralsin@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "../include/global.h"
#include "../include/logger.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

void printAuthor(char *);
void getIP(char *);
void commonPrintFunction(char *, char *);
void getPortforServer(char *,char *);

int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);
	char *tempChar = argv[1];
	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));
	/*Start Here*/
	string cmd;
	while(true)
	{
		cout << "\n#chat# : ";
		cin >> cmd;
		if(cmd == "AUTHOR")
			printAuthor("AUTHOR");

		if(cmd == "EXIT")
			break;

		if(cmd == "IP")
			getIP("IP");
		
		if(cmd == "PORT")
		{
			commonPrintFunction("PORT",argv[2]);
		}
	}
	return 0;
}

// void getPortforServer(char *cmd, char *port)
// {
// 	int sockFd, addressStatus;
// 	char ipstr[INET_ADDRSTRLEN];
// 	struct  addrinfo hints;
// 	struct  addrinfo *addressInfo;
// 	memset(&hints,0,sizeof hints);
// 	hints.ai_family = AF_INET;
// 	hints.ai_socktype = SOCK_DGRAM;
// 	hints.ai_flags = AI_PASSIVE;
// 	if(addressStatus = getaddrinfo(NULL,port,&hints,&addressInfo)){
// 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addressStatus));
// 	}

// 	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
// 	{
// 		perror("listener: socket");
// 	}

// 	//bind(sockFd, addressInfo->ai_addr, addressInfo->ai_addrlen);
// 	connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen);
// 	struct sockaddr_in name ;
// 	socklen_t namelen = sizeof(name);
// 	getsockname(sockFd,(struct sockaddr *)&name,&namelen);	
// 	cout<<ntohs(name.sin_port);
// }

void getIP(char *cmd)
{
	int sockFd, addressStatus;
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if(addressStatus = getaddrinfo("8.8.8.8","3453",&hints,&addressInfo)){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addressStatus));
	}

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
	{
		perror("listener: socket");
	}
	connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen);
	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	getsockname(sockFd,(struct sockaddr *)&name,&namelen);
	char buf[100];
	inet_ntop(addressInfo->ai_family, &name.sin_addr, buf, sizeof buf);
	commonPrintFunction("IP",buf);
}

void printAuthor(char *authorName)
{
	char *myString = "I, Viral Sinha, have read and understood the course academic integrity policy.";
	commonPrintFunction("AUTHOR",myString);
}

void commonPrintFunction(char *command, char *output)
{
	cse4589_print_and_log("[%s:SUCCESS]\n",command);
	cse4589_print_and_log("%s:%s\n",command,output);
	cse4589_print_and_log("[%s:END]\n",command);
}

