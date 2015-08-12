/*
 *  Copyright 2015 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *
 *  device.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 15/08/12.
 *
 */

#include "llsocket.h"
#include <ifaddrs.h>
#include <net/if.h>

// MARK: device info
#if defined(__linux__)
#include <sys/ioctl.h>

static int macaddrs_lua( lua_State *L )
{
    int fd = socket( AF_INET, SOCK_DGRAM, 0 );

    if( fd )
    {
        char buf[INET6_ADDRSTRLEN];
        struct ifreq ifrbuf[16];
        struct ifconf ifc = {
            .ifc_len = sizeof( ifrbuf ),
            .ifc_req = ifrbuf
        };

        // get if-conf
        if( ioctl( fd, SIOCGIFCONF, &ifc ) != -1 )
        {
            struct ifreq* it = ifc.ifc_req;
            struct ifreq* end = it + ( ifc.ifc_len / sizeof( struct ifreq ) );
            unsigned char *mac = NULL;

            lua_newtable( L );
            for(; it != end; it++ )
            {
                    // inet only
                if( it->ifr_addr.sa_family == AF_INET &&
                    // running only and ignore loopback
                    ioctl( fd, SIOCGIFFLAGS, it ) != -1 &&
                    it->ifr_flags & IFF_RUNNING &&
                    !( it->ifr_flags & IFF_LOOPBACK ) &&
                    // get flags and hardware address
                    ioctl( fd, SIOCGIFHWADDR, it ) != -1 ){
                    mac = (unsigned char*)it->ifr_hwaddr.sa_data;

                    snprintf( buf, INET6_ADDRSTRLEN,
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        *mac, mac[1], mac[2], mac[3], mac[4], mac[5]
                    );
                    lstate_str2tbl( L, it->ifr_name, buf );
                }
            }

            close( fd );
            return 1;

        }

        close( fd );
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


#else

#include <net/if_dl.h>

static int macaddrs_lua( lua_State *L )
{
    struct ifaddrs *ifa;
    
    if( getifaddrs( &ifa ) == 0 )
    {
        lua_newtable( L );
        
        if( ifa )
        {
            struct ifaddrs *ptr = ifa;
            char buf[INET6_ADDRSTRLEN];
            unsigned char *mac = NULL;
            struct sockaddr_dl *sd;
            
            do
            {
                if( ptr->ifa_addr->sa_family == AF_LINK )
                {
                    sd = (struct sockaddr_dl*)ptr->ifa_addr;
                    switch( sd->sdl_alen ){
                        case 6:
                            mac = (unsigned char*)LLADDR( sd );
                            snprintf( 
                                buf, INET6_ADDRSTRLEN, 
                                "%02x:%02x:%02x:%02x:%02x:%02x",
                                *mac, mac[1], mac[2], mac[3], mac[4], mac[5] 
                            );
                            lstate_str2tbl( L, ptr->ifa_name, buf );
                        break;
                        case 8:
                            mac = (unsigned char*)LLADDR( sd );
                            snprintf( 
                                buf, INET6_ADDRSTRLEN, 
                                "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                                *mac, mac[1], mac[2], mac[3], mac[4], mac[5], 
                                mac[6], mac[7]
                            );
                            lstate_str2tbl( L, ptr->ifa_name, buf );
                        break;
                    }
                }
                
                ptr = ptr->ifa_next;
            } while( ptr );
        }
        
        freeifaddrs( ifa );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}
#endif


LUALIB_API int luaopen_llsocket_device( lua_State *L )
{
    struct luaL_Reg method[] = {
        // device info
        { "macaddrs", macaddrs_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = method;
    
    lua_newtable( L );
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    
    return 1;
}


