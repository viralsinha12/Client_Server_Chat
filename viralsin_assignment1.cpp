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
#include <stdlib.h>
using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

void printAuthor(char *);
void getIP(char *,int);
void commonPrintFunctionForSuccess(char *, const char *);
void commonPrintFunctionForError(char *, const char *);
void getPortforServer(char *,char *);
void loginToServer(char *);
void startServer();
int sendMessage(string,int);
void receiveAndRelay(string,int,int,int,fd_set);
void unicastMessage(string,string,int);
void broadcastMessage(string,string,int,int,int,fd_set);
string getOriginalMessage(string);
string getSendersIp(string);
string ltrim(const string);
string rtrim(const string);

char systemIp[INET_ADDRSTRLEN];

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

void getIP(char *cmd,int print)
{
	int sockFd, addressStatus;
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof(hints));
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
	memset(systemIp,'\0',sizeof(systemIp));
	inet_ntop(addressInfo->ai_family, &name.sin_addr, systemIp, sizeof(systemIp));
	if(print==1)
		commonPrintFunctionForSuccess("IP",systemIp);
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
	memset(&hints,0,sizeof(hints));
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
	
	//cout<<"connection on : "<<sockFd<<endl;

	while(1)
	{
		write_fds = master;
		select(fdMax+1,&write_fds,NULL,NULL,0);
		if(FD_ISSET(0,&write_fds))
		{
			vector<string> tokens;
			string input,tmp,rest,intr;
			getline(cin,input);
			stringstream ss(input);
			int i=0;
			while(getline(ss,intr,' '))
			{
				if(i<1)
					tokens.push_back(intr);
				i++;
			}
			
			if(tokens[0]=="SEND")
			{
				int retValue = sendMessage(input,sockFd);
				if(retValue == -1)
				{
					cse4589_print_and_log("[SEND:ERROR]\n");
					cse4589_print_and_log("[SEND:END]\n");
				}
				else
				{
					cse4589_print_and_log("[SEND:SUCCESS]\n");
					cse4589_print_and_log("[SEND:END]\n");
				}
			}
			if(tokens[0]=="BROADCAST")
			{
				int retValue = sendMessage(input,sockFd);
				if(retValue == -1)
				{
					cse4589_print_and_log("[BROADCAST:ERROR]\n");
					cse4589_print_and_log("[BROADCAST:END]\n");
				}
				else
				{
					cse4589_print_and_log("[BROADCAST:SUCCESS]\n");
					cse4589_print_and_log("[BROADCAST:END]\n");
				}
			}
		}
		else
		{

			if(FD_ISSET(sockFd,&write_fds))
			{
				char buff[1000];
				memset(buff,'\0',sizeof(buff));
				int recvLen = recv(sockFd,buff,sizeof(buff),0);

				if(recvLen >0)
				{
					buff[recvLen]='\0';
					string tempString(buff);
    				string originalMessage = getOriginalMessage(tempString);
    				string sendersIp = getSendersIp(tempString);
					getIP("IP",0);
					string ip(systemIp);
					cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
					cse4589_print_and_log("msg from:%s\n[msg]:%s\n",sendersIp.c_str(),originalMessage.c_str());
					cse4589_print_and_log("[RECEIVED:END]\n");
					memset(buff,'\0',sizeof(buff));

				}
			}
		}	
	}
}

void startServer()
{
	fd_set master;fd_set read_fds;fd_set write_fds;
	int sockFd{}, fdMax{},newSocketFd, addressStatus;
	FD_ZERO(&master);FD_ZERO(&read_fds);FD_ZERO(&write_fds);
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof(hints));
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
        				//cout<<"client connection on : "<<newSocketFd<<endl;
        				//send(newSocketFd,newSocketFd)
				}	
				else
				{
					char buff[1000];
					memset(buff,'\0',sizeof(buff));
					int lenOfData = recv(i,buff,sizeof(buff),0);
					if(lenOfData==-1)
						perror("recv");
					else
					{
						//buff[lenOfData] = '\0';
						string stringMessage(buff);
						receiveAndRelay(stringMessage,i,sockFd,fdMax,master);
					}
				}
			}
		}	
	}	
}
int sendMessage(string message, int socket)
{
	getIP("IP",0);
	string iP(systemIp);
	message = message + "^"+systemIp;
	if(send(socket,message.c_str(),message.length(),0) == -1){
		return -1;	
	}
	else
	{
		return 0;
	}

}					

void receiveAndRelay(string message,int listeningSocket,int connectionSocket,int maximumSocket,fd_set masterlist)
{
	stringstream check1(message);
	string intr;
	while(getline(check1,intr,' '))
	{
		if(intr=="SEND")
		{
			string tmp,originalMessage,recpIp,intr;
			stringstream ssMessage(message);
			stringstream ssIp(message);
			ssMessage>>tmp>>tmp;
			getline(ssMessage,originalMessage);
			int i=0;
			while(getline(ssIp,intr,' '))
			{
				if(i==1){
					recpIp=intr;
					break;
				}
				i++;
			}
			unicastMessage(originalMessage,recpIp,maximumSocket);
			break;
		}
		if(intr=="BROADCAST")
		{
			string tmp,originalMessage,recpIp,intr;
			stringstream ssMessage(message);
			stringstream ssIp(message);
			ssMessage>>tmp;
			getline(ssMessage,originalMessage);
			broadcastMessage(originalMessage,"255.255.255.255",listeningSocket,connectionSocket,maximumSocket,masterlist);
			break;
		}
	}
}

void broadcastMessage(string message,string ipRecp,int listeningSocket,int connectionSocket,int maximumSocket,fd_set master)
{
	int i=0;
	for(int j=0;j<=maximumSocket;j++)
	{
		if(FD_ISSET(j,&master))
		{
			if(j!=listeningSocket && j!=connectionSocket)
			{
				if(send(j,message.c_str(),message.length(),0)==-1)
				{	
					commonPrintFunctionForError("BROADCAST");
				}
				else
				{
					if(i==0)
					{
						string tempString = getOriginalMessage(message);
	    				string sendersIp = getSendersIp(message);
						cse4589_print_and_log("[RELAYED:SUCCESS]\n");
						cse4589_print_and_log("msg from:%s, to:255.255.255.255 \n[msg]:%s\n",sendersIp.c_str(),tempString.c_str());
						cse4589_print_and_log("[RELAYED:END]\n");
						i++;
					}
				}
			}	
		}
	}
}

void unicastMessage(string message,string recpIp,int maximumSocket)
{
	getIP("IP",0);
	string ip(systemIp);
	string originalMessage = rtrim(ltrim(message));
	string recpiIp = rtrim(ltrim(recpIp));		
	
	for(int j=0;j<=maximumSocket;j++)
	{
		struct sockaddr_in *connectedIp;
		socklen_t len = sizeof(connectedIp);				
	 	if((getpeername(j,(struct sockaddr *)&connectedIp,&len)!=-1))
	 	{
			char ipBuf[INET_ADDRSTRLEN];
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)&connectedIp;
			void *addr = &ipv4->sin_addr; 
			inet_ntop(AF_INET,addr,ipBuf, sizeof(ipBuf));
			string receviersip(ipBuf);
			if(receviersip.compare(recpiIp)==0)
			{
				if(send(j,originalMessage.c_str(),originalMessage.length(),0)==-1)
					perror("SEND");
				else
				{
    				string tempString = getOriginalMessage(originalMessage);
    				string last_element = getSendersIp(originalMessage);
    				cse4589_print_and_log("[RELAYED:SUCCESS]\n");
					cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",last_element.c_str(),receviersip.c_str(),tempString.c_str());
					cse4589_print_and_log("[RELAYED:END]\n");
					break;
				}
			}
		}
	 	//else
	 		//perror("peernameissue");
	 }
}

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

string getOriginalMessage(string message)
{
	string tempString(message);
	string last_element(tempString.substr(tempString.rfind("^") + 1));
	size_t found = tempString.rfind("^");
  	if (found!=std::string::npos)
    tempString.replace(found,tempString.length(),"");
    tempString = rtrim(ltrim(tempString));
    return tempString;
}

string getSendersIp(string message)
{
	string tempString(message);
	string last_element(tempString.substr(tempString.rfind("^") + 1));
	return last_element;
}