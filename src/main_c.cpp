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
* File:  main_c.cpp
* Author:  SkibbleBip
* Procedures:
* newPing       -Calls the C++ library default constructor
* createPing    -Calls the C++ library overloaded constructor
* copyPing      -Calls the C++ library copy constructor
* destroyPing   -Calls the C++ library destructor
* ping_connectMC        -Calls the C++ library connection function
* ping_getError -Calls the C++ library error handle and returns the error code
* ping_getResponse      -Calls the C++ library response function, returns the string of data returned from the server
* ping_getPing  -Calls the C++ library latency function and returns the milliseconds for processing
* ping_SRV_Lookup       -Calls the SRV Lookup function in the C++ library and checks the DNS cache for the domain
* ping_getDNSerror      -Calls the C++ library DNS error handle and returns the DNS error code
* ping_ping_free        -Calls the C++ library data freeing function
***************************************************************************/


#include "MinecraftPing.h"

extern "C" {
/*These are all implementations of the C frontend of the library.
*If the user wants to link this library to a C program, the user
*must use these functions, they call the "backend" library functions
*that are written in C++. This is done for C compatability
*/

        Ping* newPing(void)
        {
                return new Ping();
        }

        Ping* createPing(const char* address, int p)
        {
                return new Ping(address, p);
        }

        Ping* copyPing(const Ping obj)
        {
                return new Ping(obj);
        }

        void destroyPing(Ping* p)
        {
                p->~Ping();
        }

        int ping_connectMC(Ping* p)
        {
                return p->connectMC();
        }

        pingError ping_getError(Ping* p)
        {
                return p->getError();
        }

        char* ping_getResponse(Ping* p)
        {
                return p->getResponse();
        }

        long ping_getPing(Ping* p)
        {
                return p->getPing();
        }

        void ping_SRV_Lookup(char* domain, DNS_Response* dnsr)
        {
                Ping::SRV_Lookup(domain, dnsr);
        }

        DNS_ERROR ping_getDNSerror(Ping* p)
        {
                return p->getDNSerror();
        }

        void ping_ping_free(Ping* p)
        {
                p->ping_free();
        }



}
