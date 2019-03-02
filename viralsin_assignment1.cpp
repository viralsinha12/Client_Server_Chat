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
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include "../include/global.h"
#include "../include/logger.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>

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
void loginToServer(char *);
void startServer();
void sendMessage(int);

int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);
    fclose(fopen(LOGFILE, "w"));
	/*Start Here*/
   
    if(strcmp(argv[1],"s")== 0)
    {
    	  startServer();
    }

    else
    {
    		/*string cmd;
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

			if(cmd == "LOGIN" && (strcmp(argv[1],"c")==0))
			{
				loginToServer();
				//cout<<"connected";
			}
		}*/
		
                                loginToServer(argv[1]);
	}
	return 0;
}

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
	if(addressStatus = getaddrinfo("8.8.8.8","3453",&hints,&addressInfo))
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addressStatus));

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
		perror("listener: socket");
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

void loginToServer(char *argv1)
{
	int fdMax;
	int sockFd, addressStatus;

	fd_set master;
	fd_set write_fds;
	FD_ZERO(&master);
	FD_ZERO(&write_fds);
	FD_SET(sockFd,&master);	
	FD_SET(STDIN_FILENO,&master);
//	FD_SET(STDOUT_FILENO,&master);
	fdMax = STDIN_FILENO;
	//int sockFd, addressStatus;
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(addressStatus = getaddrinfo("128.205.36.46","3453",&hints,&addressInfo))
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addressStatus));

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
		perror("listener: socket");

	
	if(connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen)==-1)
		perror("Error");
	


	while(1)
	{
		write_fds = master;
		int selValue = select(fdMax+1,&write_fds,NULL,NULL,0);
		if(FD_ISSET(STDIN_FILENO,&write_fds))
		{
			char bufferString[1000];
			memset(bufferString,'\0',sizeof(bufferString));
			int readLen = read(STDIN_FILENO,bufferString,sizeof(bufferString));
			
			if(readLen>0)
			{		
				if(strncmp(bufferString,"SEND",4)==0)
				{
					//cout<<sockFd<<endl;
					//sendMessage(sockFd);
					if(send(sockFd,"hello clients",20,0)==-1)
					perror("send");
					memset(bufferString,'\0',sizeof(bufferString));

				}
			}
		}
		else
		{
			cout<<"receving data";
			if(FD_ISSET(sockFd,&write_fds))
			{
				cout<<"receiving data";
				char buff[1000];
				memset(buff,'\0',sizeof buff);
				int recvLen = recv(sockFd,buff,sizeof(buff),0);
				if(recvLen >0)
				{
					buff[recvLen]='\0';
					cout<<buff<<endl;
					memset(buff,'\0',sizeof buff);
				}
			}
			else
			{
				cout<<"which socket"<<endl;
			}
		}	
	}
}

void startServer()
{
	fd_set master;fd_set read_fds;fd_set write_fds;
	int sockFd{}, fdMax ,newSocketFd, addressStatus;
	FD_ZERO(&master);FD_ZERO(&read_fds);FD_ZERO(&write_fds);
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(addressStatus = getaddrinfo(NULL,"3453",&hints,&addressInfo))
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addressStatus));

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
		perror("listener: socket");
	
	bind(sockFd, addressInfo->ai_addr, addressInfo->ai_addrlen);
	//int recvSock{};
	FD_SET(sockFd, &master);
	fdMax = sockFd;
	listen(sockFd, 5);

	while(1)
	{
		read_fds = master;	
		select(fdMax+1,&read_fds,NULL,NULL,NULL);
		for(int i=0;i<=fdMax;i++)
		{
			if(FD_ISSET(i,&read_fds))
			{
				if(i == sockFd)
				{
					socklen_t namelen = sizeof(addressInfo);
					newSocketFd = accept(sockFd, (struct sockaddr *)&addressInfo,&namelen);
			       		 if(newSocketFd == -1)
        			  		perror("accept");
					FD_SET(newSocketFd, &master);
		        		if(newSocketFd > fdMax)
        					fdMax = newSocketFd;
				}	
				else
				{
					char buff[1000];
					memset(buff,'\0',sizeof buff);
					int lenOfData = recv(i,buff,sizeof(buff),0);
					if(lenOfData==-1)
						perror("recv");
					else
					{
						buff[lenOfData] = '\0';
						//cout<<"fdMax : "<<fdMax<<endl;
						for(int j=0;j<=fdMax;j++)
						{
							//cout<<"J : "<<j<<endl;
							if(FD_ISSET(j,&master))
							{
								//cout<<"second";
								if(j!=i && j!=sockFd)
								{
									//cout<<"third";
									send(j,buff,strlen(buff),0);		
								}
							}
						}						
					}
				}
			}
		
		}
	}	
}	

void sendMessage(int sockFd)
{
	send(sockFd,"hello clients",20,0);	
	cout<<"Message sent";
}					
				




