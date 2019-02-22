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

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

int printAuthor(char *);
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);
	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));
	/*Start Here*/
	string cmd;
	while(true){
	cout << "#chat# : ";
	cin >> cmd;

	if(cmd == "AUTHOR")
		printAuthor("AUTHOR");

	if(cmd == "EXIT")
		break;

	if(cmd == "IP")
		getPort();
	}
	return 0;
}

int getIP()
{
	int sockFd;
	struct  addrinfo hints;
	struct  addrinfo *addressInfo;
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	//if()
	
}

int printAuthor(char *authorName)
{
	
	char *myString = "I, Viral Sinha, have read and understood the course academic integrity policy.\n";
	cse4589_print_and_log("[%s:SUCCESS]\n",authorName);
	cse4589_print_and_log("AUTHOR:%s\n",myString);
	cse4589_print_and_log("[%s:END]\n",authorName);
	return 0;
}