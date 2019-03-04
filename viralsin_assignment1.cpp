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
#include <vector>
#include <sstream>
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
void commonPrintFunctionForSuccess(char *, const char *);
void commonPrintFunctionForError(char *, const char *);
void getPortforServer(char *,char *);
void loginToServer(char *);
void startServer();
void sendMessage(int);
string ltrim(const string);
string rtrim(const string);
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
	//commonPrintFunction("IP",buf);
}

void printAuthor(char *authorName)
{
	char *myString = "I, Viral Sinha, have read and understood the course academic integrity policy.";
	//commonPrintFunction("AUTHOR",myString);
}

void commonPrintFunctionForSuccess(char *command,const char *output)
{
	cse4589_print_and_log("[%s:SUCCESS]\n",command);
	cse4589_print_and_log("%s:%s\n",command,output);
	cse4589_print_and_log("[%s:END]\n",command);
}

void commonPrintFunctionForError(char *command)
{
	cse4589_print_and_log("[%s:Error]\n",command);
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
	FD_SET(0,&master);
	fdMax = STDIN_FILENO;
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

	FD_SET(sockFd,&master);	

	if(sockFd > fdMax)
		fdMax = sockFd;
	
	if(connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen)==-1)
		perror("Error");
	
	cout<<"connection"<<endl;

	while(1)
	{
		write_fds = master;
		select(fdMax+1,&write_fds,NULL,NULL,0);
		if(FD_ISSET(0,&write_fds))
		{
			vector<string> tokens;
			string line;
			getline(cin,line);
			string intr;

			if(line.find("SEND")!= string::npos)
			{
				line.insert(4,"^");
				stringstream check1(line);
				while(getline(check1,intr, '^'))
				{
					tokens.push_back(rtrim(ltrim(intr)));
					//cout<<line<<endl;
				}
			}

			if(line.find("BROADCAST")!= string::npos)
			{
				line.insert(9,"^");
				stringstream check1(line);
				// stringstream check1(line);
				// cout<<line<<endl;
				while(getline(check1,intr,'^'))
				{
					tokens.push_back(rtrim(ltrim(intr)));
					//cout<<line<<endl;
				}
			}

			
			
			//cin.clear();
			/*char bufferString[1000];
			memset(bufferString,'\0',sizeof(bufferString));
			int readLen = read(0,bufferString,sizeof(bufferString));*/
			if(line.length()>0)
			{
				//cout<<line<<endl;	
				//cout<<tokens[1]<<endl;	
				if(tokens[0].compare("SEND")==0)
				{
					//cout<<"inside send"<<endl;
					if(send(sockFd,tokens[1].c_str(),tokens[1].length(),0) == -1)
					perror("send");
					//memset(bufferString,'\0',sizeof(bufferString));
				}
				if(tokens[0].compare("BROADCAST")==0)
				{
					//cout
					if(send(sockFd,line.c_str(),line.length(),0) == -1){
						commonPrintFunctionForError("BROADCAST");
					//perror("send");
					}
					else
					{
						commonPrintFunctionForSuccess("BROADCAST",tokens[1].c_str());
					}
				}
			}
		}
		else
		{

			if(FD_ISSET(sockFd,&write_fds))
			{
				char buff[1000];
				memset(buff,'\0',sizeof buff);
				int recvLen = recv(sockFd,buff,sizeof(buff),0);
				if(recvLen >0)
				{
					buff[recvLen]='\0';
					cse4589_print_and_log("msg from:\n[msg]:%s\n",buff);
					memset(buff,'\0',sizeof buff);

				}
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
					cout<<"Message  : "<<buff<<endl;
					cout<<"compare result : "<<strncmp(buff,"BROADCAST",9)<<endl;
					if(lenOfData==-1){
						perror("recv");
					}
					else
					{
						if(strncmp(buff,"BROADCAST",9)!=0){
						cout<<"Send section "<<endl;		
						struct sockaddr_in *connectedIp;
						socklen_t len = sizeof connectedIp;	
						buff[lenOfData] = '\0';
							for(int j=0;j<=fdMax;j++)
							{
								if((getpeername(j,(struct sockaddr *)&connectedIp,&len)!=-1))
								{
									char ipBuf[INET_ADDRSTRLEN];
									struct sockaddr_in *ipv4 = (struct sockaddr_in *)&connectedIp;
									void *addr = &ipv4->sin_addr; 
									inet_ntop(AF_INET,addr,ipBuf, sizeof ipBuf);
								//	cout<<"IP : "<<ipBuf<<endl;	
									if(strcmp(ipBuf,buff)==0)
									{
										if(send(j,"buff",strlen(buff),0)==-1)
											perror("SEND");
										else
											break;
									}
								}		
							}
						}	
						else
						{
							int error = 0;
							string line = buff;
							stringstream check1(line);
							string intr;
							vector<string> tokens;
							while(getline(check1,intr, '^'))
							{
								//size_t end = s.find_last_not_of(WHITESPACE);
								
								tokens.push_back(rtrim(ltrim(intr)));
								//cout<<line<<endl;
							}	

							for(int j=0;j<=fdMax;j++)
							{
								if(FD_ISSET(j,&master))
								{
									if(j!=i && j!=sockFd)
									{
										if(send(j,tokens[1].c_str(),tokens[1].length(),0)==-1)
										{	
											commonPrintFunctionForError("BROADCAST");
											error=1;
										}
										else
										{
											cse4589_print_and_log("[BROADCAST:SUCCESS]\n");
											cse4589_print_and_log("msg from:, to:255.255.255.255 \n[msg]:%s\n", tokens[1].c_str());
											cse4589_print_and_log("[BROADCAST:END]\n");
										}
									}
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
				
/*char* getPresentableIp(short int ipFamily, struct in_addr addr)
{
	char ip4[INET_ADDRSTRLEN];
	if(inet_ntop(ipFamily,addr,ip4,sizeof ip4)==-1)
		perror("ntop");
	return ip4;
}*/ 

string ltrim(string s)
{
	size_t start = s.find_first_not_of(" ");
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(string s)
{
	size_t end = s.find_last_not_of(" ");
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}
