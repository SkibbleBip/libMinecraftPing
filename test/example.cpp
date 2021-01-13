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

#include "MinecraftPing.h"
#include <iostream>


char* getDNSresponse(DNS_ERROR z);
char* getAPIerror(pingError z);
//translates the error codes into human readable content


using namespace std;


/**This is the Test example program that demonstrates the usage of the libMinecraftPing API**/
int main(int argc, char* args[]){


    if(argc != 3){
        cout<<"Usage: mcping [server] [port]"<<endl;
        return 0;
    }


    char* ip = args[1];
    unsigned short port = atoi(args[2]);


    Ping p = Ping(ip, port);

    int x = p.connectMC();

    if(x <0){
        cout<<"There was an error attempting to communicate with the Minecraft server: "<<p.getError()<<endl;
        cout<<"\tDNS Error Response: "<< getDNSresponse(p.getDNSerror())<<endl;
        cout<<"\tMinecraft Error Code: "<< getAPIerror(p.getError())<<endl;
        return -1;
    }
    else if(x == 0){
        cout<<"The server was not found: " <<endl;
        DNS_Response d;
        p.SRV_Lookup(ip, &d);
        cout<<"\tBackend resolved address: "<<d.url<<endl;
        cout<<"\tDNS Error Response: "<< getDNSresponse(p.getDNSerror())<<endl;
        cout<<"\tMinecraft Error Code: "<< getAPIerror(p.getError())<<endl;
        return 0;
    }
    else{
        cout<<p.getResponse()<<endl;
        cout<<"\n\n\tPing: "<<p.getPing()<<" ms."<<endl;
        DNS_Response d;
        p.SRV_Lookup(ip, &d);
        cout<<"\tBackend resolved address: "<<d.url<<endl;
        cout<<"\tDNS Error Response: "<< getDNSresponse(p.getDNSerror())<<endl;
        cout<<"\tMinecraft Error Code: "<< getAPIerror(p.getError())<<endl;
    }


return 1;

}

char* getDNSresponse(DNS_ERROR z){
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


char* getAPIerror(pingError z){
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

