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

/***************************************************************************
* File:  main.cpp
* Author:  SkibbleBip
* Procedures:
* connectMC     -Attempts to initialize a connection to the minecraft server
* Ping(X, Y)    -Overloaded constructor
* Ping(&X)      -Copy constructor
* Ping()        -Default constructor
* initializeSocket      -(Windows Only) Sets up the WSA OS features
* buildHandshake        -Function that creates the handshake packet
* readVarInt    -Reads in data from a varint from the socket
* checkIfIP     -Checks an inputted string if it is a domain or IP
* SRV_Lookup    -Performs an SRV DNS record lookup
* ~Ping()       -Destructor
* ping_free     -Frees any dynamic data
* getError      -returns the ping error code
* getResponse   -returns the string response of the ping process
* getPing       -returns the ping latency of the connection
* getDNSerror   -Returns the DNS error occured while searching for the IP the
*                       domain points to
***************************************************************************/



#ifdef _WIN32
#define _WIN32_WINNT 0x501
#define CLOSE(X)            closesocket(X)
#endif // _WIN32
#ifdef __linux__
#define CLOSE(X)            shutdown(X, 1)
#endif // __linux__


#include "MinecraftPing.h"
#include <endian.h>



/***************************************************************************
* int Ping::connectMC(void)
* Author: SkibbleBip
* Date: Unknown, 2020   v1 Initial
* Date: 09/09/2021      v2 Cleaned up, optimized, and added safety checking
* Description: Function that attempts to initialize a connection to the
*                       minecraft server and query it's status
*
* Parameters:
*        connectMC      O/P     int     return status, returns 1 if successful,
*                                                0 if server is offline, or
*                                                negative if an error occured
**************************************************************************/
int Ping::connectMC(void)
{
        /*attempt to connect to the Minecraft Server. Returns a positive number
        *if successfully communicated with the server, returns 0 when the server
        *could not be found (offline, bad url, etc), and negative if there was
        *a network error
        */
        error = OK;

        unsigned short _port = this->port;

        //free(pingResponse);
        pingResponse = "";




#ifdef _WIN32
        if(Ping::initializeSocket()){
        //initialize the socket
                error = INITIALIZATION_FAILURE;
                milliseconds = -1;
                return -1;
                //if failed to initialize the windows socket, then return -1

        }
#endif // windows requires you to initialize the socket before opening


        const char* addressToProcess;
        /*the IP address the server's domain points to*/
        char* backAddress;
        /*the backend url to the server, as SRV records could have a
        *re-direct the minecraft server requires
        */
        struct in_addr addr;
        /*struct to contain the IP address info*/

        if(checkIfIP(frontAddress)){
                addressToProcess = frontAddress;
                /*the url is an IP, it is assumed it can directly
                *connect to the IP
                */
        }
        else{
                struct hostent* _host;  //struct to contain the host info

                DNS_Response dnsr;
                SRV_Lookup((char*)frontAddress, &dnsr);
                dnsError = dnsr.dns_error;
                /*attempt SRV record lookup, set the error code from the
                *record's response
                */

                if(dnsError == NOERROR_STATUS){
                        _host       = gethostbyname(dnsr.url);
                        backAddress = dnsr.url;
                        /*the SRV Record was found, get the backend address
                        *and get the _host object that contains the properties
                        *of the server domain
                        */

                }
                else if(dnsError == NXDOMAIN_STATUS){
                        _host       = gethostbyname(frontAddress);
                        backAddress = (char*)frontAddress;
                        /*SRV record was not found, attempt to check A name
                        *through gethostbyname, assume the backend address is
                        *the same as the frontend address
                        */

                }
                else{
                        error        = SRV_FAILURE;
                        //free(pingResponse);
                        //pingResponse = nullptr;
                        pingResponse = "";
                        milliseconds = -1;
                        return -1;
                /*the SRV record failed to successfully request a lookup,
                *something went wrong set the error code and return -1 to let
                *user know there was a failure
                */

                }

                if(_host != nullptr){
                        addr.s_addr      = *(u_long*)_host->h_addr_list[0];
                        /*get first hostname from the list*/
                        addressToProcess = inet_ntoa(addr);
                        /*get the IP octal values of the IP address*/
                        dnsError         = NOERROR_STATUS;
                        /*overwrite SRV_Lookup's response code, as
                        *gethostbyname was able to resolve the location
                        *of the url
                        */

                }
                else{
                        error        = OK;
                        //pingResponse = nullptr;
                        pingResponse = "";
                        milliseconds = -1;
                        return 0;
                        /*if _host was nullptr, that's ok, it just means
                        *the DNS server could not find the domain, it does
                        not exist. return 0 to let user know the server was
                        *not found
                        */
                }



        }
        /*some non-notchian servers (specifically those that are protected by
        * DDOS Protection Services such as Cloudflare or TCPShield)
        * do not allow handshaking from direct IPs, they prefer that they
        * connect through DNS-recorded domains, henceforth they only allow the
        * URL of the server be included in the handshake, not the IP. This part
        * checks if the inputted Minecraft server location is an IP or URL.
        * This assumes you are aware whether or not the destination accepts IPs
        * or URLs. Attempting to connect from an invalid URL will result
        * in a DNS_FAILURE error and a return 0.
        */


        server.sin_family      = AF_INET;
        server.sin_port        = htons(_port);
        server.sin_addr.s_addr =  inet_addr(addressToProcess);
        /*initialize the server connection configuration as an INET /24 IP,
        * and initialize the port.
        * set the IP address of the server
        */



        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        /*create the socket as /24 IP, TCP stream*/

        if(sock < 0){
        /*if the socket is negative, then it failed, return out*/
                error = SOCKET_OPEN_FAILURE;
                milliseconds = -1;
                //free(pingResponse);
                //pingResponse = nullptr;
                pingResponse = "";

                return -1;
        }
        /*open the socket*/





#ifdef _WIN32
        unsigned long mode = 0;
        /* 0 is blocking, != is non-blocking  */
        ioctlsocket(sock, FIONBIO, &mode);
        /*in *nix, sockets are blocking by default*/
#endif // _WIN32
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        /*set socket options as blocking, set the timeout for the connection*/


        int connectR = connect(sock, (struct sockaddr*)&server, sizeof(server));
        /*client-connect
        *connect to the socket
        */

        if(connectR<0){
        /*if response is negative, then it failed, possibly the
        * server is down, so return 0
        */

                error = CONNECT_FAILURE;
                //pingResponse = nullptr;
                pingResponse = "";
                milliseconds = -1;
                return 0;

        }

        uint8_t handshakePacket[HANDSHAKE_MAX_SIZE];
        size_t packetSize = buildHandshake(handshakePacket, backAddress);
        /*build the handshake packet*/

        if(packetSize < 0){
        /*if the handshake packet building failed, it's possible a domain
        *that was too long was given to it, therefor we should error out
        */
                error = BAD_DOMAIN;
                dnsError = INVALID_DOMAIN;
                //pingResponse = nullptr;
                pingResponse = "";
                milliseconds = -1;
                return 0;
        }


        int sendVal = send(sock, handshakePacket, packetSize, 0);
        /*send the handshake packet*/

        if(sendVal <0){
        /*if send response is negative, then it failed to send*/
                error = SEND_FAILURE;
                //pingResponse = nullptr;
                pingResponse = "";
                milliseconds = -1;
                return -1;
        }
        sendVal = send(sock, request, 2, 0);
        /*follow up immediatly with a request packet*/
        if(sendVal <0){
        /*if response negative, then it failed*/
                error = SEND_FAILURE;
                //pingResponse = nullptr;
                pingResponse = "";
                milliseconds = -1;
                return -1;
        }




        readVarInt(sock);
        /*eat the first varInt, this variable isnt needed for anything*/

        char id;
        int size = recv(sock, &id, 1, 0);
        /*attempt to get the ID of the transmission.
        * Minecraft's packet ID is 0x0
        */
        if(size <= 0){
                /*if reply size is negative, it means it failed*/
                error = RECEIVE_FAILURE;
                //pingResponse = nullptr;
                pingResponse = "";
                milliseconds = -1;
                return -1;
        }
        if(id != 0){
                /*if the ID is not 0, then it is not a regular reply-could
                * be a reject packet or trash
                */
                milliseconds = -1;
                //pingResponse = nullptr;
                pingResponse = "";
                return 0;
        }


        int json_length = readVarInt(sock);
        if(json_length < 0){
                milliseconds = -1;
                //pingResponse = nullptr;
                pingResponse = "";
                return -1;
        }
        /*if the response from readVarInt() is negative, then it failed to
        *read, return -1
        */


        //pingResponse = (char*)malloc(json_length*sizeof(char)+1);
        /**pingResponse becomes dynamically allocated**/
        /*memory to hold the ping response, +1 for the null terminator*/
        char buffer[BUFFER_SIZE];
        int read  = 999;
        int total = json_length;
        //int s = 0;

        while(json_length >0){
        /*
        *while the receiver still has data in queue, append that value to the
        * pingResponse which is the char pointer to the server's JSON reply.
        * allocate new space into the pingResponse using realloc and the size
        * that was returned in the buffer
        */
                read = recv(sock, buffer, BUFFER_SIZE, 0);
                if(read <0){
                        /*if recv replies with negative, then it failed*/
                        error = RECEIVE_FAILURE;
                        //free(pingResponse);
                        //pingResponse = nullptr;
                        pingResponse = "";
                        milliseconds = -1;
                        return -1;
                }


                //memcpy(pingResponse+s, buffer, read);
                pingResponse.append(buffer, read);

                /*add the buffer to the pingResponse*/

                //s+=read;
                json_length = json_length - read;
            /*subtract the amount currently processed in the buffer
            *from the total
            */
        }//end while
        //pingResponse[total] = '\0';

        /**PING PACKET
                        ID: 0X1     LONG: 8 BYTES

        **/
        /*timing start and stops for calculating the ping duration time*/
        struct timeval _stop, _start;


        gettimeofday(&_start, NULL);
        /*get start time*/

        uint64_t start = _start.tv_sec * 1000 + (double)_start.tv_usec * 1.0/(1000.0);
        /*calculate time in milliseconds (for ping)*/

        uint8_t pingPacket[10];
        pingPacket[0] = 9;
        /*packet length*/
        pingPacket[1] = 0x1;
        /*ping ID*/

        std::memcpy(pingPacket+2, &start, 8);

/*Packet example: [0x9] [0x1] [0x0] [0x1] [0x2] [0x3] [0x4] [0x5] [0x6] [0x7]
                    ^size  ^ID                ^Long Data

*this packet contains a random number to use as a checksum when sending and
*receiving the ping-pong packets
*/

        sendVal = send(sock, pingPacket, 10, 0);
        /*send the packet*/
        if(sendVal < 0){
                error = SEND_FAILURE;
                milliseconds = -1;

        }
        else{
                uint8_t pingReply[10];
                int total = 0;
                read = 0;
                do{
                        read = recv(sock, pingReply+read, 10, 0);
                        total +=read;

                }while(total <10);
                /*certain servers transmit packets in chunks, so we must
                *piece them all back together
                */


                if(read < 0){
                        /*if read fail, set the error code to RECEIVE_FAILURE,
                        *set ping to negative. this is a soft error
                        */
                        error = RECEIVE_FAILURE;
                        milliseconds = -1;
                }
                else{
                        if(pingReply[1] != 0x1 || pingReply[0] != 9){
                        /*if the ping ID and size dont match, throw error.
                        *this is a soft error
                        */
                        error = PING_FAILURE;
                        milliseconds = -1;
                        }
                        else{

                                if(memcmp(pingPacket, pingReply, 10)){
                                /*check the incoming packet for any
                                *descrepancies in content. If it found a
                                *descrepancy, throw an ping error, this is a
                                *soft error
                                */
                                        error = PING_FAILURE;
                                        milliseconds = -1;
                                }
                                else{
                                        gettimeofday(&_stop, NULL);
                                        /*get the stop time*/
                                        uint64_t stop =
                                                _stop.tv_sec * 1000 +
                                                (double)_stop.tv_usec *
                                                1.0/(1000.0);
                                        milliseconds = stop - start;
                                        /*get duration in milliseconds.
                                        *this is the ping duration
                                        */

                                }
                        }
                }
        }


       CLOSE(sock);
#ifdef _WIN32
       WSACleanup();
#endif // _WIN32
       /*cleanup any windows related stuff*/


        return 1;
    /*return 1, we successfully connected*/
}

/***************************************************************************
* Ping::Ping( const char* address, unsigned short p)
* Author: SkibbleBip
* Date: 09/10/2021
* Description: Overloaded constructor
*
* Parameters:
*        address        I/P     const char*     domain of the minecraft server
*        p              I/P     unsigned short  port of the minecraft server
**************************************************************************/
Ping::Ping( const char* address, unsigned short p)
{
        port = p;
        strncpy(frontAddress, address, DOMAIN_MAX_SIZE);
        frontAddress[DOMAIN_MAX_SIZE-1] = '\000';

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        //pingResponse = (char*)malloc(0);
        error = OK;
        dnsError = NOERROR_STATUS;
        milliseconds = 0;


}

/***************************************************************************
* Ping::Ping(const Ping &obj)
* Author: SkibbleBip
* Date: 09/10/2021
* Description: Copy constructor
*
* Parameters:
*        obj    I/P     const Ping&     object to copy
**************************************************************************/
Ping::Ping(const Ping &obj)
{
        port = obj.port;
        strncpy(frontAddress, obj.frontAddress, DOMAIN_MAX_SIZE);
        frontAddress[DOMAIN_MAX_SIZE] = '\000';
        timeout = obj.timeout;
        pingResponse = obj.pingResponse;
        error = obj.error;
        dnsError = obj.dnsError;
        milliseconds = obj.milliseconds;
}

/***************************************************************************
* Ping::Ping()
* Author: SkibbleBip
* Date: 09/10/2021
* Description: Default constructor
*
* Parameters:
**************************************************************************/
Ping::Ping()
{
        port = 0;
        frontAddress[0] = '\000';
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        //pingResponse = nullptr;
        pingResponse = "";
        error = OK;
        dnsError = NOERROR_STATUS;
        milliseconds = 0;
}

#ifdef _WIN32
/***************************************************************************
* bool Ping::initializeSocket(void)
* Author: SkibbleBip
* Date: Unknown, 2020
* Description: (Windows Only) Initializes the WSA socket OS features
*
* Parameters:
*        initializeSocket       O/P     bool    boolean status return value
**************************************************************************/
bool Ping::initializeSocket(void)
{
        /*windows network initializer*/
        WSADATA data;

        if(!WSAStartup(MAKEWORD(2, 10), &data))
                return false;
        else
                return true;

}//end initialize
#endif // _WIN32

/***************************************************************************
* size_t Ping::buildHandshake(uint8_t* buffer, char* host)
* Author: SkibbleBip
* Date: Unknown, 2020   v1 Initial
* Date: 09/09/2021      v2 Optimized code to make it faster, added safety
*                               checking
* Description: Function that creates the handshake packet
*
* Parameters:
*        buffer I/O     uint8_t*        buffer to contain the packet data
*        host   I/P     char*   domain of the host being queried
*        buildHandshake O/P     size_t  size of the packet generated
**************************************************************************/
size_t Ping::buildHandshake(uint8_t* buffer, char* host)
{
        ///inspired by https://github.com/theodik/mcping/blob/master/mcping.c

        /**                 PROTOCOL VERSION: VARINT (-1)
        PACKET ID: 0X00     SERVER ADDRESS: STRING
                            SERVER PORT: UNSIGNED SHORT
                            NEXT STATE: VARINT (1)
        **/

        size_t hostLength = strnlen(host, DOMAIN_MAX_SIZE+1);
        if(hostLength > DOMAIN_MAX_SIZE){
                return -1;
        }

        size_t length = 1 /*packet ID*/ + 5; /*protocol version*/
        length = length + hostLength +1;
        length = length + 2; /*port*/
        length = length +1; /*state*/


        int i = 0;
        buffer[i++] = length;
        buffer[i++] = ID;
        memcpy(buffer+i, version, 5);
        i = i+5;
        buffer[i++] = hostLength;
        memcpy(buffer + i, host, hostLength);
        i = i+hostLength;
        buffer[i++] = (port >> 8) & 0xFF; /* MSB */
        buffer[i++] = port & 0xFF;  /*LSB*/
        buffer[i] = NEXT_STATE;

        length = length+1;

        return length;
}


/***************************************************************************
* int Ping::readVarInt(int s)
* Author: SkibbleBip
* Date: Unknown, 2020
* Description: Reads from a socket data and processes it to read the variable
*       integer from the packet
*
* Parameters:
*        s      I/P     int     inputted socket
*        readVarInt     O/P     int     value of the varint read
**************************************************************************/
int Ping::readVarInt(int s)
{
        char size;
        int number = 0;
        int result = 0;
        do{
                int b = recv(s, &size, 1, 0);
                if(b<0){
                        /*receive the varInt buffer character. if recv'ing
                        *responds with a negative, then it failed
                        */
                        error = RECEIVE_FAILURE;
                        return -1;

                }
                result |= (size&0x7F)<<(7*number);
                number++;
                /*process the varint and turn it back into a regular integer*/
                if(number>5){
                /*if more than 5 bytes are found in the varInt, then it's a
                *bad varint
                */
                        error = MALFORMED_VARINT_PACKET;
                        return -1;

                }
        }while((size&0x80) !=0);


        return result;
    /*return the integer value of the varInt*/
}

/***************************************************************************
* bool Ping::checkIfIP(const char* in)
* Author: SkibbleBip
* Date: Unknown, 2020
* Description: Checks if a string contains an IP or a domain url
*
* Parameters:
*        in     O/P     const char*     inputted string
*        checkIfIP      O/P     bool    boolean response
**************************************************************************/
bool Ping::checkIfIP(const char* in)
{
        int c = 0;

        for(unsigned int i=0; i < strlen(in); i++){
                if(in[i] > 57 || in[i] < 48)
                        return false;
        /*if the character is not numeric, then it is definately not an IP*/

        if(in[i] == '.')
                c++;
                /*add 1 to the decimal count*/
        }

        if(c == 3)
                return true;
        /*if there are 3 decimals, then its an IP*/

        return false;
        /*else return false*/
}

/***************************************************************************
* void Ping::SRV_Lookup(char* domain, DNS_Response* dnsr)
* Author: SkibbleBip
* Date: Unknown, 2020   v1: Initial
* Date: 09/05/2021      v2: Fixed dynamic allocation and replaced it with array
*                               memory
* Description: Performs SRV lookup of the minecraft server
*
* Parameters:
*        domain I/P     char*   name of the domain being searched for, can only
*                                       be 253 characters long
*        dnsr   I/O     DNS_Response*   DNS data response struct that holds
*                                               any error codes, responses, etc
**************************************************************************/
void Ping::SRV_Lookup(char* domain, DNS_Response* dnsr)
{
        struct timeval random;
        /*random number generator*/
        char label[63];
        /*chunks of labels*/
        char tmpDomain[DOMAIN_MAX_SIZE+16] = "_minecraft._tcp.";

        if(strnlen(domain, DOMAIN_MAX_SIZE+1) > DOMAIN_MAX_SIZE){
        /*if the domain submitted is too long, then return an error*/
                dnsr->dns_error = INVALID_DOMAIN;
                dnsr->url[0] = '\000';
                return;
        }

        if(strncmp(domain, tmpDomain, 16)){
        //if url doesnt start with _minecraft._tcp., then add it
                strncat(tmpDomain, domain, DOMAIN_MAX_SIZE);
        }
        else{
                strncpy(tmpDomain, domain, DOMAIN_MAX_SIZE);
        }



#ifdef _WIN32
        if(Ping::initializeSocket()){
        //initialize the socket
            error = INITIALIZATION_FAILURE;
            //milliseconds = -1;
            //DNS_Response dnsr;
            dnsr->dns_error = WSA_INITIALIZE_FAILURE;
            dnsr->url[0] = '\0';
            return;
            //if failed to initialize the windows socket, then return -1
        }
#endif // windows requires you to initialize the socket before opening


/**
                    DNS HEADER
ID: 16 bits | QR: 1 bit | OPCODE: 4 bit | AUTHORITIVE ANSWER: 1 bit |
TRUNCATE: 1 bit | RECURSION DESIRED: 1 bit | RECURSION AVAILABLE: 1 bit |
 Z: 3 bits(000) | RESPONSE CODE: 4 bits (0-5) | QUESTION COUNT: 16 bits |
ANSWER COUNT: 16 bits | NAME RESOURCE COUNTS: 16 bits |
ADDITIONAL RESOURCE COUNTS: 16 bits
**/
/**/
/**/
/**/    gettimeofday(&random, NULL);
/**/    uint32_t time = htole32(random.tv_sec);  /*Long is 4 bytes*/
/**/
        uint8_t mcHeader[12] = {0x0, 0x0, 0x1, 0x0, 0x0, 0x1,
                                0x0, 0x0, 0x0, 0x0, 0x0, 0x0
                                };

        memcpy(mcHeader, (void*)(&time), 2);
        /*copy the payload random number to the first 2 bytes*/

/**                     DNS QUESTION
    QNAME: length octet + that number of octets + null octet.
    Example: \002my\006server\003org\000 == my.server.org
    QTYPE: 2 octet code, 16 bits, d1 for A record, d5 for CNAME,
        d33 for SRV Lookup
    QCLASS: 2 octet, 16 bits
**/

        Minecraft_DNS_Question mcQuestion;
        /*the question section of the DNS request*/

        uint8_t j = 0;
        uint8_t i;

        for(i = 0; i<strnlen(tmpDomain, DOMAIN_MAX_SIZE+16); i++){
                if(j >= 63){
                /*if the label size is greater than 63, then the domain is
                *invalid
                */
                        dnsr->dns_error = INVALID_DOMAIN;
                        dnsr->url[0] = '\000';
                        return;
                }


                if(tmpDomain[i] != '.'){
                        label[j] = tmpDomain[i];
                        j++;
                /*add new char to the temp label and increase the temp counter*/
                }
                else{
                /*copy the label to the output domain string*/

                        label[j] = '\000';
                        mcQuestion.QNAME[i] = '\000';
                        mcQuestion.QNAME[i-j] = j;
                        memcpy(mcQuestion.QNAME +i-j+1, label, j);
                        j = 0;


                }
        }

        mcQuestion.QNAME[i]   = '\0';
        mcQuestion.QNAME[i-j] =  j;
        memcpy(mcQuestion.QNAME +i-j+1, label, j);
        /*add the final length, label, and null terminator*/


        mcQuestion.QTYPE  = htobe16(0x0021);
        /*qtype = 0x0021 = 33, SRV record*/
        mcQuestion.QCLASS = htobe16(0x0001);
        /*qclass = 0x001, Internet address*/
/******************************************************************************/
                /**End of setting up question**/

        int z = i  +1 + 4;
        /*size of the domain name +null terminator + the type and class bytes*/

        uint8_t toSend[512];
        /*max size of a DNS packet is 512 bytes*/

        memcpy(toSend, &mcHeader, sizeof(mcHeader));
        memcpy(toSend+sizeof(mcHeader), mcQuestion.QNAME, i+1);
        toSend[sizeof(mcHeader)+i+1] = '\0';
        /*copy the header and question to the send buffer*/
        memcpy(toSend+sizeof(mcHeader)+z-3, &mcQuestion.QTYPE, 2);
        memcpy(toSend+sizeof(mcHeader)+z-1, &mcQuestion.QCLASS, 2);

        int s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        sockaddr_in dest;
        dest.sin_family=AF_INET;        /*open the socket on the dns port 53*/
        dest.sin_port=htons(53);
#ifdef _WIN32
        FIXED_INFO pfi;
        unsigned long ulOutBufLen = sizeof(FIXED_INFO);
        GetNetworkParams(&pfi, &ulOutBufLen);
        dest.sin_addr.s_addr=inet_addr(pfi.DnsServerList.IpAddress.String);
#endif // _WIN32
#ifdef __linux__
        res_init();
        dest = _res.nsaddr_list[0];
#endif // __linux__

/*perform the lookup of the DNS server on either windows or *nix
*(usually the router)
*/



        int val = sendto(s,
                        toSend,
                        sizeof(mcHeader)+z+1,
                        0,
                        (sockaddr*)&dest,
                        sizeof(dest)
                        );
        /*send the DNS packet to the gateway*/


        if(val<0){
            /*if sending failed, return an error*/
                dnsr->dns_error = SEND_REQUEST_FAILURE;
                dnsr->url[0]       = '\0';
                return;

        }


        uint8_t incoming[512];
        /*Max size of a DNS packet is 512 bytes*/
#ifdef _WIN32
        int x = sizeof(dest);
#else
        unsigned int x = sizeof(dest);
        /*windows is stupid for not agreeing with everyone else*/
#endif

        uint16_t _id;
        uint16_t _answers;
        uint16_t _port;
        uint8_t  _error;
        //DNS packet answer properties

        do
        {
                val = recvfrom(s, incoming, 512, 0, (sockaddr*)&dest, &x);
    /**         DNS Answer
        NAME (QNAME FORMAT) | TYPE: 16 bits | CLASS: 16 bits | TTL: 32 bits |
        RDLENGTH: 16 bits
        RDATA:      depends, look up the format for SRV
    **/

                if(val<0){
                        /*if failed to receive, return an error*/
                        dnsr->dns_error = RECV_REQUEST_FAILURE;
                        dnsr->url[0]       = '\0';
                        dnsr->port      = 0;
                        return;
                }

                _id      = *((uint16_t*)(incoming));
                _answers = *((uint16_t*)(incoming+6));
                _answers = htons(_answers);
                _error   = incoming[3]&0x0f;
                /*get ID of the packet, error code and the number of answers*/

        }while(_id != *((uint16_t*)mcHeader));
        //}while(memcmp(&_id, mcHeader, 2));
        /*as the machine may receive multiple DNS packets from various unrelated
        *services, we need to make sure the nabbed dns packet actually belongs
        *to our application. To do this, we compare our sent packet to the
        *received packet. if the IDs dont match, attempt to receive a new packet
        */

        if(_answers < 1){
                /*if there were no answers, then return out to let
                *the user know
                */
                dnsr->url[0]        = '\0';
                dnsr->dns_error  = (DNS_ERROR)_error;
                dnsr->port       = 0;
                //_dnsr = &dnsr;
                return;
        }


        signed int size = incoming[12];
        unsigned int next_stop = 0;
        int v = 1;
        while(size != 0){
                next_stop +=size;
                size = incoming[next_stop + 12 + v++];
        }
        /*get location of the last part of the srv record query
        *string in the packet
        */

        /*Past this point it is assumed that the packet transmission
        *and receival was successful
        */

        _port = *((uint16_t*)(incoming+12 + next_stop + 20 + v));
        _port = ntohs(_port);
        /*get the backend port of the server*/

        next_stop = next_stop + 12/*size of header*/
        + v/*size of question string*/ + 22 /*location of the answer*/;
        size = incoming[next_stop];
        uint32_t next_stop2 = 0;


        while(size != 0){
                memcpy(dnsr->url+next_stop2, incoming+next_stop+1, size);
                next_stop += size+1;
                next_stop2+=size+1;
                dnsr->url[next_stop2-1] = '.';

                size = incoming[next_stop];
        }

        dnsr->url[next_stop2-1] = '\0';
        dnsr->dns_error = (DNS_ERROR)_error;
        dnsr->port = _port;
#ifdef _WIN32
        WSACleanup();
#endif



/**                     DNS ANSWER
    NAME: same value as QNAME
    TYPE: 2 octets of type code, specifies the meaning of RDATA
        (0x0001 A, 0x0005 CNAME, 0x0021 SRV)
    CLASS: 2 octets specify the class of RDATA
    TTL: time to live
    RDLENGTH: length of RDATA
    RDATA: response
**/


}

/***************************************************************************
* Ping::~Ping(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: Destructor of the Ping class
*
* Parameters:
**************************************************************************/
Ping::~Ping(void)
{

        //free(this->pingResponse);

        //free the response

        //exit
}


/***************************************************************************
* void Ping::ping_free(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: Frees the dynamic response generated by the class
*
* Parameters:
**************************************************************************/
/*void Ping::ping_free(void)
{
        free(this->pingResponse);
        this->pingResponse = nullptr;
}*/

/***************************************************************************
* pingError Ping::getError(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: returns the ping error code
*
* Parameters:
*        getError       O/P     pingError       The returned error code
**************************************************************************/
pingError Ping::getError(void)
{
        return this->error;
}

/***************************************************************************
* std::string Ping::getResponse(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: returns the string response of the ping process
*
* Parameters:
*        getResponse    O/P     std::string   The returned string
**************************************************************************/
/*char**/std::string Ping::getResponse(void)
{
        return this->pingResponse;
}

/***************************************************************************
* long Ping::getPing(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: returns the ping latency of the connection
*
* Parameters:
*        getPing        O/P     long    The returned latency value
**************************************************************************/
long Ping::getPing(void)
{
        return this->milliseconds;
}

/***************************************************************************
* DNS_ERROR Ping::getDNSerror(void)
* Author: SkibbleBip
* Date: 09/09/2021
* Description: Returns the DNS error occured while searching for the IP the
*       domain points to
*
* Parameters:
*        getDNSserver   O/P     DNS_ERROR       Returned DNS error code
**************************************************************************/
DNS_ERROR Ping::getDNSerror(void)
{
        return this->dnsError;
}


