/**
    Minecraft Server List Protocol API.
    Copyright (C) 2020  SkibbleBip

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

#ifndef MINECRAFTPING_H_INCLUDED
#define MINECRAFTPING_H_INCLUDED


#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
//#include <ws2tcpip.h>
#endif // _WIN32
#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <resolv.h>
#endif // __linux__

#include <cstring>
#include <stdlib.h>
#include <sys/time.h>
//#include <time.h>

#define TIMEOUT 5

#define ID 0x00
#define VERSION -1      /**definitions reserved for later use**/
#define NEXT_STATE 1
#define BUFFER_SIZE 1024

#ifndef nullptr
#define nullptr NULL    /*if nullptr has not been declared by the compiler, then declare it*/
#endif // nullptr


typedef signed int minecraft_socket;
//just a regular socket, there is already a definition for a socket in a few libraries but for these implimentations I am simlifying it



    enum pingError {OK, SOCKET_INITIALIZATION_FAILURE, SOCKET_OPEN_FAILURE,
                    RECEIVE_FAILURE, MALFORMED_VARINT_PACKET, INITIALIZATION_FAILURE,
                    SEND_FAILURE, CONNECT_FAILURE, PING_FAILURE, SRV_FAILURE
                    };
            //ping attempt error codes

/**
                    DNS HEADER
ID: 16 bits | QR: 1 bit | OPCODE: 4 bit | AUTHORITIVE ANSWER: 1 bit | TRUNCATE: 1 bit | RECURSION DESIRED: 1 bit | RECURSION AVAILABLE: 1 bit | Z: 3 bits(000) |
RESPONSE CODE: 4 bits (0-5) | QUESTION COUNT: 16 bits | ANSWER COUNT: 16 bits | NAME RESOURCE COUNTS: 16 bits | ADDITIONAL RESOURCE COUNTS: 16 bits
**/


struct Minecraft_DNS_Question{
    char* QNAME;
    char QTYPE[2];
    char QCLASS[2];

};

/**                     DNS QUESTION
    QNAME: length octet + that number of octets + 0 length octet
    QTYPE: 2 octet code, 16 bits, d1 for A record, d5 for CNAME
    QCLASS: 2 octet, 16 bits
**/


enum DNS_ERROR{NOERROR_STATUS = 0, FORMERR_STATUS = 1, SERVFAIL_STATUS = 2, NXDOMAIN_STATUS = 3, NOTIMP_STATUS = 4, REFUSED_STATUS = 5,
                YXDOMAIN_STATUS = 6, XRRSET_STATUS = 7, NOTAUTH_STATUS = 8, NOTZONE_STATUS = 9, SEND_REQUEST_FAILURE = 16,
                RECV_REQUEST_FAILURE = 17, WSA_INITIALIZE_FAILURE = 18
};
            //SRV DNS server response codes, values 10 thru 15 are reserved

struct DNS_Response{
    char* url;           /*The alias URL of the SRV record*/
    DNS_ERROR dns_error; /*DNS error response code*/
    signed short port;   /*redirected port response from the record*/

};


class Ping{


private:
    const char request[2] = {0x1, 0x0};
    const unsigned char version[5] = {0xff,0xff,0xff,0xff, 0x0f};// -1 in varInt
    //constant packet values

    minecraft_socket sock;
    struct sockaddr_in server;
    struct timeval timeout;
    char* pingResponse;
    const char* frontAddress;
    const char* actualAddress;
    short port;
    long milliseconds;
    pingError error;
    DNS_ERROR dnsError;
    //variables

    bool initializeSocket(void);
    size_t buildHandshake(char* buffer,   char* host);
    int readVarInt(minecraft_socket s);
    bool checkIfIP(const char* in);
    //private functions

public:
    int connectMC();
    Ping( const char* address, int p);
    Ping(){}
    ~Ping();
    Ping(const Ping &obj);
    pingError getError(){return error;}
    char* getResponse(){return pingResponse;}
    long getPing(){return milliseconds;}
    DNS_Response SRV_Lookup(char* url);
    DNS_ERROR getDNSerror(){return dnsError;}






};



#endif // MINECRAFTPING_H_INCLUDED


