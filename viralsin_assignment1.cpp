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

//used external libraries
//used vector;stdlib;algorithm(sort);msgpack(for serialization/deserialation of data)
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include<msgpack.hpp>
using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

void printAuthor();
void getIP(char *,int);
void commonPrintFunctionForSuccess(char *, const char *);
void commonPrintFunctionForError(char *);
void getPortforServer(char *,char *);
void loginToServer(string,string,string);
void startServer(string);
void receiveAndRelay(string,int,int,int,fd_set);
void unicastMessage(string,string,int);
void broadcastMessage(string,string,int,int,int,fd_set);
void displayListOfBlockedClients(string,int);

string getOriginalMessage(string);
string getSendersIp(string);
string getIpfromSocket(int);
string ltrim(const string);
string rtrim(const string);
string skipFirstWord(string);
string skipTwoWords(string);

int isValidPort(string);
int isValidIp(string);
int sendMessage(string,int);
int checkBlockList(string,string);
int getSocketFdFromIp(int,string);
int getPortFromIpForBlockedList(string,int);
int unblockIp(string,string);
int validateIpAndPort(string,string);
int checkLocalList(string);
int validateIP(string,string);
int validateIForSending(string);
int checkBlockedListInServer(string);
int checkForValidPort(string);

//Reference : https://stackoverflow.com/questions/40896477/using-msgpack-to-encode-a-user-defined-structure
//MSGPACK requires this method MSGAPACK to be defined for it to be considered a MSGPACK type
struct loggedInDetails{
		string name;
		string ip;
		int port;
		MSGPACK_DEFINE(name,ip,port);
};

vector<loggedInDetails> getDetailsOfConnectedClients(int,int,fd_set);

struct blockedListstruct{
		string name;
		string ip;
		int port;
};

//Inserting into structure on the basis of port number
bool comparePorts(loggedInDetails first,loggedInDetails second)
{
    return first.port < second.port;
}

//Inserting into structure on the basis of port number
bool comparePortsForBlockedListDisplay(blockedListstruct first,blockedListstruct second)
{
    return first.port < second.port;
}

vector<loggedInDetails> clientList;
vector< pair<string,string> > blockList;
vector<blockedListstruct> blockListByClient;

fd_set globalMasterSet;
char systemIp[INET_ADDRSTRLEN];
int clientLoggedIn = 0;


int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);
    fclose(fopen(LOGFILE, "w"));
	/*Start Here*/
   
    if(strcmp(argv[1],"s")== 0)
    {
    	  startServer(argv[2]);
    }

    else
    {
    	while(true)
		{
	    	string intr,input;
	    	vector<string> commandTokens;
	    	//Reference:https://en.cppreference.com/w/cpp/string/basic_string/getline
	    	getline(cin,input);
	    	stringstream ss(input);
	    	while(getline(ss,intr,' '))
    		{
    			commandTokens.push_back(intr);
    		}
		
			if(commandTokens[0] == "AUTHOR")
				printAuthor();

			if(commandTokens[0] == "EXIT")
				break;

			if(commandTokens[0] == "IP")
				getIP("IP",1);
			
			if(commandTokens[0] == "PORT")
			{
				commonPrintFunctionForSuccess("PORT",argv[2]);
			}

			if(commandTokens[0] == "LOGIN")
			{
				int isValid = validateIpAndPort(commandTokens[1],commandTokens[2]);
				if(isValid == 1)
				{
					cse4589_print_and_log("[LOGIN:SUCCESS]\n");
					cse4589_print_and_log("[LOGIN:END]\n");
					loginToServer(commandTokens[1],commandTokens[2],argv[2]);
				}
				else
				{
					cse4589_print_and_log("[LOGIN:ERROR]\n");
					cse4589_print_and_log("[LOGIN:END]\n");
				}	
			}	
		}
	}
	return 0;
}

void getIP(char *cmd,int print)
{
	int sockFd;
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo("8.8.8.8","3453",&hints,&addressInfo) == -1)
	{
		commonPrintFunctionForError("IP");
	}
				

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
	{
		commonPrintFunctionForError("IP");
	}

	if(connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen)==-1)
	{
		commonPrintFunctionForError("IP");
	}

	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	socklen_t len = sizeof(sa);
	//Reference : https://support.sas.com/documentation/onlinedoc/sasc/doc700/html/lr2/zockname.htm//
	getsockname(sockFd,(struct sockaddr *)&sa,&	len);
	memset(systemIp,'\0',sizeof(systemIp));
	//beej//
	inet_ntop(addressInfo->ai_family, &sa.sin_addr, systemIp, sizeof(systemIp));
	if(print==1)
		commonPrintFunctionForSuccess("IP",systemIp);
}

void printAuthor()
{
	cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
	cse4589_print_and_log("I, viralsin, have read and understood the course academic integrity policy.\n");
	cse4589_print_and_log("[AUTHOR:END]\n");
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

void loginToServer(string ip,string port,string lport)
{
	int fdMax;
	int sockFd, addressStatus;
	fd_set master;
	fd_set write_fds;
	FD_ZERO(&master);
	FD_ZERO(&write_fds);
	//adding input fd to the main set - 0 = STDIN
	FD_SET(0,&master);
	fdMax = 0;
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	struct  addrinfo *bindaddressInfo;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo(ip.c_str(),port.c_str(),&hints,&addressInfo)==-1)
	{
		cse4589_print_and_log("[LOGIN:ERROR]\n");
		cse4589_print_and_log("[LOGIN:END]\n");
	}

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
		perror("listener: socket");

	FD_SET(sockFd,&master);	

	if(sockFd > fdMax)
		fdMax = sockFd;

	getaddrinfo(NULL,lport.c_str(),&hints,&bindaddressInfo);
    bind(sockFd, bindaddressInfo->ai_addr, bindaddressInfo->ai_addrlen);

	if(connect(sockFd,addressInfo->ai_addr,addressInfo->ai_addrlen)==-1)
	{
		clientLoggedIn=0;
	}
	else
	{
		clientLoggedIn=1;
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
				
				if(tokens[0]=="SEND" && clientLoggedIn == 1)
				{
					int isValidSend = validateIForSending(input);
					if(isValidSend == 1)
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
					else
					{
							cse4589_print_and_log("[SEND:ERROR]\n");
							cse4589_print_and_log("[SEND:END]\n");
					}
				}
				if(tokens[0]=="BROADCAST" && clientLoggedIn==1)
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
				if(tokens[0]=="AUTHOR")
					printAuthor();
				if(tokens[0]=="IP")
					getIP("IP",1);
				if(tokens[0] == "PORT")
				{
					commonPrintFunctionForSuccess("PORT",lport.c_str());
				}
				if(tokens[0]=="LOGOUT" && clientLoggedIn==1){
					clientLoggedIn=0;
					string removeSocketString = "REMOVE";
					send(sockFd,removeSocketString.c_str(),removeSocketString.length(),0);
					cse4589_print_and_log("[LOGOUT:SUCCESS]\n");
					cse4589_print_and_log("[LOGOUT:END]\n");
					break;
				}
				if(tokens[0]=="EXIT"){
					clientLoggedIn=0;
					exit(0);
				}
				if(tokens[0]=="LIST")
				{
					for(int i =0;i<clientList.size();i++)
					{
						if(i==0)
							cse4589_print_and_log("[LIST:SUCCESS]\n");
						cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i+1, clientList[i].name.c_str(), clientList[i].ip.c_str(),clientList[i].port);
						if(i==clientList.size()-1)
							cse4589_print_and_log("[LIST:END]\n");
					}
				}
				if(tokens[0]=="REFRESH" && clientLoggedIn == 1)
				{
					string inputMsg = "REFRESH";

					int retValue = send(sockFd,inputMsg.c_str(),inputMsg.length(),0);
					if(retValue == -1)
					{
						cse4589_print_and_log("[REFRESH:ERROR]\n");
						cse4589_print_and_log("[REFRESH:END]\n");
					}
					else
					{
						cse4589_print_and_log("[REFRESH:SUCCESS]\n");
						cse4589_print_and_log("[REFRESH:END]\n");
					}
				}
				if(tokens[0]=="BLOCK" && clientLoggedIn == 1)
				{
					if(validateIP(input,"")==1)
					{
						send(sockFd,input.c_str(),input.length(),0);
					}
					else
					{
						cse4589_print_and_log("[BLOCK:ERROR]\n");
						cse4589_print_and_log("[BLOCK:END]\n");
					}
				}
				if(tokens[0]=="UNBLOCK" && clientLoggedIn == 1)
				{
					if(validateIP(input,"")==1)
					{
						send(sockFd,input.c_str(),input.length(),0);
					}
					else
					{
						cse4589_print_and_log("[UNBLOCK:ERROR]\n");
						cse4589_print_and_log("[UNBLOCK:END]\n");	
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
						//Reference : http://www.thomaswhitton.com/blog/2013/07/03/binary-message-format-c-plus-plus-examples/
						try{
							msgpack::object_handle oh = msgpack::unpack(buff,sizeof(buff));
								msgpack::object obj = oh.get();
							vector<loggedInDetails> rvec;
	        				obj.convert(rvec);
	        				clientList = rvec;
						}
						catch(...)
						{
							buff[recvLen]='\0';
							string tempString(buff);
							if(tempString=="BLOCKSUCCESS")
							{
								cse4589_print_and_log("[BLOCK:SUCCESS]\n");
								cse4589_print_and_log("[BLOCK:END]\n");
							}
							else
							{
								if(tempString=="BLOCKFAILURE")
								{
									cse4589_print_and_log("[BLOCK:ERROR]\n");
									cse4589_print_and_log("[BLOCK:END]\n");
								}
								else
								{
									if(tempString == "UNBLOCKSUCCESS")
									{
										cse4589_print_and_log("[UNBLOCK:SUCCESS]\n");
										cse4589_print_and_log("[UNBLOCK:END]\n");			
									}
									else
									{
										if(tempString == "UNBLOCKFAILURE")
										{
											cse4589_print_and_log("[UNBLOCK:ERROR]\n");
											cse4589_print_and_log("[UNBLOCK:END]\n");	
										}
										else
										{
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
					}
				}
			}	
		}
	}
}

void startServer(string serverPort)
{
	fd_set master;fd_set read_fds;fd_set write_fds;
	int sockFd{}, fdMax{},newSocketFd, addressStatus;
	FD_ZERO(&globalMasterSet);
	FD_ZERO(&master);FD_ZERO(&read_fds);FD_ZERO(&write_fds);
	FD_SET(0,&master);
	fdMax = 0;
	char ipstr[INET_ADDRSTRLEN];
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL,serverPort.c_str(),&hints,&addressInfo);

	if((sockFd = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol))== -1)
		perror("listener: socket");
	
	bind(sockFd, addressInfo->ai_addr, addressInfo->ai_addrlen);
	FD_SET(sockFd, &master);
	
	if(sockFd>fdMax)
		fdMax=sockFd;

	listen(sockFd, 4);
	
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
        			globalMasterSet = master;		

        			clientList = getDetailsOfConnectedClients(fdMax,sockFd,globalMasterSet);

        			//Reference : http://www.thomaswhitton.com/blog/2013/07/03/binary-message-format-c-plus-plus-examples/
        			msgpack::sbuffer sbuf;
        			msgpack::packer<msgpack::sbuffer> pk(&sbuf);
        			pk.pack(clientList);
        			send(newSocketFd,sbuf.data(),sbuf.size(),0);
        			
				}	
				else
				{
					if(i == 0)
					{
							string cmd;
							vector<string> tokens;
							string intr;
							getline(cin,cmd);
							stringstream ss(cmd);
							int i=0;
							while(getline(ss,intr,' '))
							{
								if(i<1)
								tokens.push_back(intr);
								i++;
							}
							if(tokens[0]=="IP")
							{
								getIP("IP",1);
							}
							if(tokens[0]=="AUTHOR")
								printAuthor();
							if(tokens[0]=="PORT")
								commonPrintFunctionForSuccess("PORT",serverPort.c_str());
							if(tokens[0]=="LIST")
							{
								for(int i =0;i<clientList.size();i++)
								{
										if(i==0)
											cse4589_print_and_log("[LIST:SUCCESS]\n");
										cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i+1, clientList[i].name.c_str(), clientList[i].ip.c_str(),clientList[i].port);
										if(i==clientList.size()-1)
											cse4589_print_and_log("[LIST:END]\n");
								}
							}
							if(tokens[0]=="BLOCKED")
							{
								string targetIp = rtrim(ltrim(skipFirstWord(cmd)));
								if(checkBlockedListInServer(targetIp)==1)
									displayListOfBlockedClients(targetIp,fdMax);	
								else
								{
									cse4589_print_and_log("[BLOCKED:ERROR]\n");
									cse4589_print_and_log("[BLOCKED:END]\n");
								}
							}		
						}
					else
					{
						char buff[1000];
						memset(buff,'\0',sizeof(buff));
						int lenOfData = recv(i,buff,sizeof(buff),0);
						if(lenOfData<=0){
							perror("recv");
						}
						else
						{
							buff[lenOfData]='\0';
							string stringMessage(buff);
							if(stringMessage=="REFRESH")
							{
								clientList=getDetailsOfConnectedClients(fdMax,sockFd,globalMasterSet);
								msgpack::sbuffer sbuf;
        						msgpack::packer<msgpack::sbuffer> pk(&sbuf);
        						pk.pack(clientList);
        						send(i,sbuf.data(),sbuf.size(),0);
							}
							else
							{
								if(stringMessage=="REMOVE")
								{
									FD_CLR(i,&globalMasterSet);
									clientList = getDetailsOfConnectedClients(fdMax,sockFd,globalMasterSet);
								}
								else
								{
									string input = stringMessage;
									string firstWord = input.substr(0, input.find(" "));
									if(firstWord == "BLOCK")
									{	
										string iptoblock = skipFirstWord(stringMessage);
										if(checkBlockList(getIpfromSocket(i),rtrim(ltrim(iptoblock))) == 0){
											blockList.push_back(make_pair(getIpfromSocket(i),rtrim(ltrim(iptoblock))));
											string blockSuccess = "BLOCKSUCCESS";
											send(i,blockSuccess.c_str(),blockSuccess.length(),0);
										}
										else
										{
											string blockFailure = "BLOCKFAILURE";
											send(i,blockFailure.c_str(),blockFailure.length(),0);
										}
									}
									else
									{
										if(firstWord == "UNBLOCK")
										{
											string iptounblock = skipFirstWord(stringMessage);
											int unblockSuccess = unblockIp(getIpfromSocket(i),rtrim(ltrim(iptounblock)));
											if(unblockSuccess==1)
											{
												string unblockSuccessString = "UNBLOCKSUCCESS";
												send(i,unblockSuccessString.c_str(),unblockSuccessString.length(),0);
											}
											else
											{
												string unblockFailureString = "UNBLOCKFAILURE";
												send(i,unblockFailureString.c_str(),unblockFailureString.length(),0);
											}
										}
										else
										{
											receiveAndRelay(stringMessage,i,sockFd,fdMax,master);
											memset(buff,'\0',sizeof(buff));
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
			string originalMessage = skipTwoWords(message);
			string recpIp;
			stringstream ssIp(message);
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
			string originalMessage = skipFirstWord(message);
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

			if(j!=listeningSocket && j!=connectionSocket &&j!=0)
			{
				string sendersIp = getSendersIp(message);
				if((checkBlockList(getIpfromSocket(j),sendersIp))==0)
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
							cse4589_print_and_log("[RELAYED:SUCCESS]\n");
							cse4589_print_and_log("msg from:%s, to:255.255.255.255\n[msg]:%s\n",sendersIp.c_str(),tempString.c_str());
							cse4589_print_and_log("[RELAYED:END]\n");
							i++;
						}
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
			if(receviersip.compare(recpIp)==0)
			{
				string last_element = getSendersIp(originalMessage);
				int isRecvIpBlocked = checkBlockList(receviersip,last_element);
				if(isRecvIpBlocked == 0)
				{
					if(send(j,originalMessage.c_str(),originalMessage.length(),0) !=-1)
					{	
						string tempString = getOriginalMessage(originalMessage);
			    		cse4589_print_and_log("[RELAYED:SUCCESS]\n");
						cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",last_element.c_str(),receviersip.c_str(),tempString.c_str());
						cse4589_print_and_log("[RELAYED:END]\n");
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
}

void displayListOfBlockedClients(string targetIp,int maximumSocket)
{
	blockListByClient.clear();
	for(int i=0;i<blockList.size();i++)
	{
		if(blockList[i].first == targetIp)
		{
			blockedListstruct b;
			struct in_addr ipv4addr;
			memset(&ipv4addr,0,sizeof(ipv4addr));
			inet_pton(AF_INET,blockList[i].second.c_str(), &ipv4addr);
			struct hostent *he;
			he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
			b.name = he->h_name;
			b.ip = blockList[i].second;
			b.port = getPortFromIpForBlockedList(blockList[i].second,maximumSocket);
			blockListByClient.push_back(b);
		}
	}
	sort(blockListByClient.begin(),blockListByClient.end(),comparePortsForBlockedListDisplay);

	for(int i =0;i<blockListByClient.size();i++)
	{
		if(i==0)
			cse4589_print_and_log("[BLOCKED:SUCCESS]\n");
		cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i+1, blockListByClient[i].name.c_str(), blockListByClient[i].ip.c_str(),blockListByClient[i].port);
		if(i==blockListByClient.size()-1)
			cse4589_print_and_log("[BLOCKED:END]\n");
	}
}

int getPortFromIpForBlockedList(string ipForPort,int maximumSocket)
{
	int port{};
	for(int j=0;j<=maximumSocket;j++)
	{
		if(FD_ISSET(j,&globalMasterSet))
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
				if(ipBuf==ipForPort)
				{
					port = ntohs(ipv4->sin_port);
				}
			}
		}
	}
	return port;
}

vector<loggedInDetails> getDetailsOfConnectedClients(int maximumSocket,int serverSocket,fd_set globalmaster)
{
	clientList.clear();
	vector<loggedInDetails> loggedinvector;
	for(int j=0;j<=maximumSocket;j++)
	{
		if(FD_ISSET(j,&globalmaster))
		{
			struct sockaddr_in *connectedIp;
			socklen_t len = sizeof(connectedIp);				
		 	if((getpeername(j,(struct sockaddr *)&connectedIp,&len)!=-1) && j!=serverSocket)
		 	{
				char ipBuf[INET_ADDRSTRLEN];
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)&connectedIp;
				void *addr = &ipv4->sin_addr; 
				int port = ntohs(ipv4->sin_port);
				inet_ntop(AF_INET,addr,ipBuf, sizeof(ipBuf));
				string receviersip(ipBuf);
				struct hostent *he;
				struct in_addr ipv4addr;
				https://beej.us/guide/bgnet/html/single/bgnet.html#inet_ntopman
				inet_pton(AF_INET,receviersip.c_str(), &ipv4addr);
				//Reference : https://beej.us/guide/bgnet/html/single/bgnet.html#gethostbynameman
				he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
				loggedInDetails l;
				l.name=he->h_name;
				l.ip=receviersip;
				l.port=port;
				loggedinvector.push_back(l);
			}
		}
	}
	sort(loggedinvector.begin(),loggedinvector.end(),comparePorts);
	return loggedinvector;
}


//Reference : http://www.cplusplus.com/reference/string/string/find_first_not_of/
//https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
string ltrim(string s)
{
	size_t start = s.find_first_not_of(" ");
	return (start == string::npos) ? "" : s.substr(start);
}

//Reference  : http://www.cplusplus.com/reference/string/string/find_last_not_of/
//https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
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

string getIpfromSocket(int socket)
{
	struct sockaddr_in *connectedIp;
	socklen_t len = sizeof(connectedIp);				
	getpeername(socket,(struct sockaddr *)&connectedIp,&len);
	char ipBuf[INET_ADDRSTRLEN];
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)&connectedIp;
	void *addr = &ipv4->sin_addr; 
	int port = ntohs(ipv4->sin_port);
	inet_ntop(AF_INET,addr,ipBuf, sizeof(ipBuf));
	string receviersip(ipBuf);
	return receviersip;
}

int checkBlockList(string recvrsIp,string sendersIp)
{
	int returnValue = 0;
	pair<string,string> p = std::make_pair(recvrsIp, sendersIp);
	if(std::find(blockList.begin(), blockList.end(), p) != blockList.end())
		returnValue = 1;
	return returnValue;
}

int unblockIp(string requestFromIp,string requestedIp)
{
	int returnValue = 0;
	for(int i = 0;i<blockList.size();i++)
	{
		if(blockList[i].first == requestFromIp && blockList[i].second == requestedIp)
		{
			blockList.erase(blockList.begin()+i);
			returnValue = 1;
			break;
		}
	}
	return returnValue;
}

//Reference for extracting string from inpu
//http://www.cplusplus.com/reference/istream/istream/operator%3E%3E/
string skipFirstWord(string inputString)
{
	string returnString,tmp;
	stringstream ssMessage(inputString);
	ssMessage>>tmp;
	getline(ssMessage,returnString);
	return returnString;
}


//Reference for extracting string from inpu
//http://www.cplusplus.com/reference/istream/istream/operator%3E%3E/
string skipTwoWords(string inputString)
{
	string returnString,tmp;
	stringstream ssMessage(inputString);
	ssMessage>>tmp>>tmp;
	getline(ssMessage,returnString);
	return returnString;
}

int validateIP(string inputString,string port)
{
	int isValidReceiver = 1;
	string receiversIp = skipFirstWord(inputString);
	receiversIp = rtrim(ltrim(receiversIp));
	int validIporNot = validateIpAndPort(receiversIp,port);
	if(validIporNot == 0){
		isValidReceiver=0;
	}

	int isInLocalList = checkLocalList(receiversIp);
	if(isInLocalList== 0)
		isValidReceiver=0;

	return isValidReceiver;
}

int validateIpAndPort(string ip,string port)
{
	int isIpValid = 1;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    ip = rtrim(ltrim(ip));
    //Reference from beej to check for proper IP
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    if (result!=1){
    	isIpValid = 0;
    }
    if(!port.empty())
    {
    	if(checkForValidPort(rtrim(ltrim(port)))==0){
    		isIpValid = 0;
    	}
	}
    return isIpValid;
}

int checkForValidPort(string port)
{
	int isValidPort = 1;
	for(int i=0;i<port.length();i++)
	{
		if(port[i]=='0' || port[i]=='1' || port[i]=='2'|| port[i]=='3'|| port[i]=='4'|| port[i]=='5'|| port[i]=='6'|| port[i]=='7' || port[i]=='8' || port[i]=='9')
		{
			isValidPort=1;
		}
		else
		{
			isValidPort=0;
			break;
		}
	}
	if(atoi(port.c_str())<0 || atoi(port.c_str())>65535)
		isValidPort=0;

	return isValidPort;
}

int checkLocalList(string inputIp)
{
	int isInList = 0;
	for(int i=0;i<clientList.size();i++)
	{
		if(clientList[i].ip == inputIp)
		{
			isInList = 1;
		}
	}
	return isInList;
}

int validateIForSending(string inputString)
{
	int isValidReceiver = 1;
	stringstream ss(inputString);
	string intr;
	string receiversIp;
	int i=0;
	while(getline(ss,intr,' '))
	{
		if(i==1)
		{
			receiversIp=intr;
			break;
		}
		i++;
	}
	receiversIp = rtrim(ltrim(receiversIp));
	int validIporNot = validateIpAndPort(receiversIp,"");
	if(validIporNot == 0){
		isValidReceiver=0;
	}

	int isInLocalList = checkLocalList(receiversIp);
	if(isInLocalList== 0)
		isValidReceiver=0;

	return isValidReceiver;		
}

int checkBlockedListInServer(string inputString)
{
	int isValidIp = 1;
	int validIporNot = validateIpAndPort(inputString,"");
	if(validIporNot == 0){
		isValidIp=0;
		return isValidIp;
	}

	for(int i=0;i<blockListByClient.size();i++)
	{
		if(blockListByClient[i].ip == inputString)
		{
			isValidIp = 1;
		}
	}
	return isValidIp;
}