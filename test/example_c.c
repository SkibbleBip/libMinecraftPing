
/**
    Minecraft Server List Protocol API.
    Copyright (C) 2021  SkibbleBip
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
**/

#include <stdlib.h>
#include <stdio.h>
#include "MinecraftPing.h"


char* getDNSresponse(enum DNS_ERROR z);
char* getAPIerror(enum pingError z);
//translates the error codes into human readable content



int main(int argc, char* args[])
{


	if(argc != 3){
        	printf("Usage: %s [server] [port]", args[0]);
        	return 0;
    	}

	char* ip = args[1];
	unsigned short port = atoi(args[2]);


	Ping* ping = createPing(ip, port);

	int q = ping_connectMC(ping);

	if(q < 0){
		printf("There was an error attempting to communicate with the Minecraft server: %d \n", 
			ping_getError(ping)
			);
		printf("\tDNS Error Response: %s \n", getDNSresponse(ping_getDNSerror(ping)));
		printf("\tMinecraft Error Code: %s\n", getAPIerror(ping_getError(ping)));
		return -1;
	}

	else if(q == 0){
		printf("The server was not found: \n");
		struct DNS_Response d;
		ping_SRV_Lookup(ip, &d);
		printf("\tBackend resolved address: %s\n", d.url);
		printf("\tDNS Error Response: %s\n", getDNSresponse(ping_getDNSerror(ping)));
		printf("\tMinecraft Error Code: %s\n", getAPIerror(ping_getError(ping)));
		return 0;
	}

	printf("%s\n", ping_getResponse(ping));
	printf("\n\n\tPing: %ld ms\n", ping_getPing(ping));
	struct DNS_Response d;
        ping_SRV_Lookup(ip, &d);
	printf("\tBackend resolved address: %s\n", d.url);
        printf("\tDNS Error Response: %s\n", getDNSresponse(ping_getDNSerror(ping)));
        printf("\tMinecraft Error Code: %s\n", getAPIerror(ping_getError(ping)));

	return 1;
}


char* getDNSresponse(enum DNS_ERROR z){
    switch(z){
        case NOERROR_STATUS:         return (char*)"NOERROR_STATUS";
        case FORMERR_STATUS:         return (char*)"FORMERR_STATUS";
        case SERVFAIL_STATUS:        return (char*)"SERVFAIL_STATUS";
        case NXDOMAIN_STATUS:        return (char*)"NXDOMAIN_STATUS";
        case NOTIMP_STATUS:          return (char*)"NOTIMP_STATUS";
        case REFUSED_STATUS:         return (char*)"REFUSED_STATUS";
        case YXDOMAIN_STATUS:        return (char*)"YXDOMAIN_STATUS";
        case XRRSET_STATUS:          return (char*)"XRRSET_STATUS";
        case NOTAUTH_STATUS:         return (char*)"NOTAUTH_STATUS";
        case NOTZONE_STATUS:         return (char*)"NOTZONE_STATUS";
        case SEND_REQUEST_FAILURE:   return (char*)"SEND_REQUEST_FAILURE";
        case RECV_REQUEST_FAILURE:   return (char*)"RECV_REQUEST_FAILURE";
        case WSA_INITIALIZE_FAILURE: return (char*)"WSA_INITIALIZE_FAILURE";
        default:                     return (char*)"N/A";

    }
}


char* getAPIerror(enum pingError z){
    switch(z){
        case OK:                                return (char*)"OK";
        case SOCKET_INITIALIZATION_FAILURE:     return (char*)"SOCKET_INITIALIZATION_FAILURE";
        case SOCKET_OPEN_FAILURE:               return (char*)"SOCKET_OPEN_FAILURE";
        case RECEIVE_FAILURE:                   return (char*)"RECEIVE_FAILURE";
        case MALFORMED_VARINT_PACKET:           return (char*)"MALFORMED_VARINT_PACKET";
        case INITIALIZATION_FAILURE:            return (char*)"INITIALIZATION_FAILURE";
        case SEND_FAILURE:                      return (char*)"SEND_FAILURE";
        case CONNECT_FAILURE:                   return (char*)"SEND_FAILURE";
        case PING_FAILURE:                      return (char*)"PING_FAILURE";
        case SRV_FAILURE:                       return (char*)"SRV_FAILURE";
        default:                                return (char*)"N/A";

    }

}
