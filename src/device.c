/**
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
#include <sys/ioctl.h>

// MARK: device info
#if defined(__linux__)

static int macaddrs_lua(lua_State *L)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd) {
        char buf[INET6_ADDRSTRLEN];
        struct ifreq ifrbuf[16];
        struct ifconf ifc = {.ifc_len = sizeof(ifrbuf), .ifc_req = ifrbuf};

        // get if-conf
        if (ioctl(fd, SIOCGIFCONF, &ifc) != -1) {
            struct ifreq *it   = ifc.ifc_req;
            struct ifreq *end  = it + (ifc.ifc_len / sizeof(struct ifreq));
            unsigned char *mac = NULL;

            lua_newtable(L);
            for (; it != end; it++) {
                // inet only
                if (it->ifr_addr.sa_family == AF_INET &&
                    // running only and ignore loopback
                    ioctl(fd, SIOCGIFFLAGS, it) != -1 &&
                    it->ifr_flags & IFF_RUNNING &&
                    !(it->ifr_flags & IFF_LOOPBACK) &&
                    // get flags and hardware address
                    ioctl(fd, SIOCGIFHWADDR, it) != -1) {
                    mac = (unsigned char *)it->ifr_hwaddr.sa_data;

                    snprintf(buf, INET6_ADDRSTRLEN,
                             "%02x:%02x:%02x:%02x:%02x:%02x", *mac, mac[1],
                             mac[2], mac[3], mac[4], mac[5]);
                    lauxh_pushstr2tbl(L, it->ifr_name, buf);
                }
            }

            close(fd);
            return 1;
        }

        close(fd);
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

#else

static int macaddrs_lua(lua_State *L)
{
    struct ifaddrs *ifa;

    if (getifaddrs(&ifa) == 0) {
        lua_newtable(L);

        if (ifa) {
            struct ifaddrs *ptr = ifa;
            char buf[INET6_ADDRSTRLEN];
            unsigned char *mac = NULL;
            struct sockaddr_dl *sd;

            do {
                if (ptr->ifa_addr->sa_family == AF_LINK) {
                    sd = (struct sockaddr_dl *)ptr->ifa_addr;
                    switch (sd->sdl_alen) {
                    case 6:
                        mac = (unsigned char *)LLADDR(sd);
                        snprintf(buf, INET6_ADDRSTRLEN,
                                 "%02x:%02x:%02x:%02x:%02x:%02x", *mac, mac[1],
                                 mac[2], mac[3], mac[4], mac[5]);
                        lauxh_pushstr2tbl(L, ptr->ifa_name, buf);
                        break;
                    case 8:
                        mac = (unsigned char *)LLADDR(sd);
                        snprintf(buf, INET6_ADDRSTRLEN,
                                 "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                                 *mac, mac[1], mac[2], mac[3], mac[4], mac[5],
                                 mac[6], mac[7]);
                        lauxh_pushstr2tbl(L, ptr->ifa_name, buf);
                        break;
                    }
                }

                ptr = ptr->ifa_next;
            } while (ptr);
        }

        freeifaddrs(ifa);
        return 1;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}
#endif

static inline int gettable(lua_State *L, int tblidx, const char *name)
{
    lua_pushstring(L, name);
    lua_rawget(L, tblidx);
    if (!lauxh_istable(L, -1)) {
        lua_pop(L, 1);
        lua_pushstring(L, name);
        lua_newtable(L);
        lua_rawset(L, tblidx);
        lua_pushstring(L, name);
        lua_rawget(L, tblidx);
        return 1;
    }
    return 0;
}

static inline void add_ifa_flags(lua_State *L, unsigned int ifa_flags)
{
    if (ifa_flags & IFF_UP) {
        lauxh_pushbool2tbl(L, "up", 1);
    }
    if (ifa_flags & IFF_BROADCAST) {
        lauxh_pushbool2tbl(L, "broadcast", 1);
    }
    if (ifa_flags & IFF_DEBUG) {
        lauxh_pushbool2tbl(L, "debug", 1);
    }
    if (ifa_flags & IFF_LOOPBACK) {
        lauxh_pushbool2tbl(L, "loopback", 1);
    }
    if (ifa_flags & IFF_POINTOPOINT) {
        lauxh_pushbool2tbl(L, "pointtopoint", 1);
    }
    if (ifa_flags & IFF_NOTRAILERS) {
        lauxh_pushbool2tbl(L, "notrailers", 1);
    }
    if (ifa_flags & IFF_RUNNING) {
        lauxh_pushbool2tbl(L, "running", 1);
    }
    if (ifa_flags & IFF_NOARP) {
        lauxh_pushbool2tbl(L, "noarp", 1);
    }
    if (ifa_flags & IFF_PROMISC) {
        lauxh_pushbool2tbl(L, "promisc", 1);
    }
    if (ifa_flags & IFF_ALLMULTI) {
        lauxh_pushbool2tbl(L, "allmulti", 1);
    }
    if (ifa_flags & IFF_MULTICAST) {
        lauxh_pushbool2tbl(L, "multicast", 1);
    }

#if defined(IFF_OACTIVE)
    if (ifa_flags & IFF_OACTIVE) {
        lauxh_pushbool2tbl(L, "oactive", 1);
    }
#endif

#if defined(IFF_SIMPLEX)
    if (ifa_flags & IFF_SIMPLEX) {
        lauxh_pushbool2tbl(L, "simplex", 1);
    }
#endif

#if defined(IFF_MASTER)
    if (ifa_flags & IFF_MASTER) {
        lauxh_pushbool2tbl(L, "master", 1);
    }
#endif

#if defined(IFF_SLAVE)
    if (ifa_flags & IFF_SLAVE) {
        lauxh_pushbool2tbl(L, "slave", 1);
    }
#endif

#if defined(IFF_PORTSEL)
    if (ifa_flags & IFF_PORTSEL) {
        lauxh_pushbool2tbl(L, "portsel", 1);
    }
#endif

#if defined(IFF_AUTOMEDIA)
    if (ifa_flags & IFF_AUTOMEDIA) {
        lauxh_pushbool2tbl(L, "automedia", 1);
    }
#endif

#if defined(IFF_DYNAMIC)
    if (ifa_flags & IFF_DYNAMIC) {
        lauxh_pushbool2tbl(L, "dynamic", 1);
    }
#endif

#if defined(IFF_LOWER_UP)
    if (ifa_flags & IFF_LOWER_UP) {
        lauxh_pushbool2tbl(L, "lower_up", 1);
    }
#endif

#if defined(IFF_DORMANT)
    if (ifa_flags & IFF_DORMANT) {
        lauxh_pushbool2tbl(L, "dormant", 1);
    }
#endif

#if defined(IFF_ECHO)
    if (ifa_flags & IFF_ECHO) {
        lauxh_pushbool2tbl(L, "echo", 1);
    }
#endif
}

static inline void add_ifa_index(lua_State *L, char *ifa_name)
{
    unsigned int idx = if_nametoindex(ifa_name);

    if (idx != 0) {
        lauxh_pushint2tbl(L, "index", idx);
    }
}

static inline void add_ifa_mtu(lua_State *L, char *ifa_name)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd != -1) {
        struct ifreq ifr;

        strncpy(ifr.ifr_name, ifa_name, IFNAMSIZ - 1);
        if (ioctl(fd, SIOCGIFMTU, &ifr) == 0) {
            lauxh_pushint2tbl(L, "mtu", ifr.ifr_mtu);
        }
        close(fd);
    }
}

static int getifaddrs_lua(lua_State *L)
{
    struct ifaddrs *list = NULL;

    if (getifaddrs(&list) == 0) {
        struct ifaddrs *ifa   = list;
        const int tblidx      = lua_gettop(L) + 1;
        char host[NI_MAXHOST] = {0};
        socklen_t addrlen     = 0;

        lua_newtable(L);
        for (; ifa; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) {
                continue;
            }

            // create interface table if it returns 1
            if (gettable(L, tblidx, ifa->ifa_name) == 1) {
                add_ifa_flags(L, ifa->ifa_flags);
                add_ifa_index(L, ifa->ifa_name);
                add_ifa_mtu(L, ifa->ifa_name);
            }

            switch (ifa->ifa_addr->sa_family) {
            case AF_INET: {
                gettable(L, tblidx + 1, "inet");
                addrlen = sizeof(struct sockaddr_in);
            } break;

            case AF_INET6: {
                gettable(L, tblidx + 1, "inet6");
                addrlen = sizeof(struct sockaddr_in6);
            } break;

#if defined(__linux__)
            case AF_PACKET: {
                struct sockaddr_ll *addr = (struct sockaddr_ll *)ifa->ifa_addr;
                if (addr->sll_halen) {
                    snprintf(host, NI_MAXHOST, "%02x:%02x:%02x:%02x:%02x:%02x",
                             addr->sll_addr[0], addr->sll_addr[1],
                             addr->sll_addr[2], addr->sll_addr[3],
                             addr->sll_addr[4], addr->sll_addr[5]);
                    lauxh_pushstr2tbl(L, "ether", host);
                }
            }
#else
            case AF_LINK: {
                addrlen = ((struct sockaddr_dl *)ifa->ifa_addr)->sdl_alen;
                // set address
                if (addrlen > 0 &&
                    getnameinfo(ifa->ifa_addr, addrlen, host, NI_MAXHOST, NULL,
                                0, NI_NUMERICHOST) == 0) {
                    lauxh_pushstr2tbl(L, "ether", host);
                }
            }
#endif
            default:
                lua_pop(L, 1);
                continue;
            }

            // set address
            lua_createtable(L, 0, 2);
            if (getnameinfo(ifa->ifa_addr, addrlen, host, NI_MAXHOST, NULL, 0,
                            NI_NUMERICHOST) == 0) {
                lauxh_pushstr2tbl(L, "address", host);
            }
            if (ifa->ifa_netmask &&
                getnameinfo(ifa->ifa_netmask, addrlen, host, NI_MAXHOST, NULL,
                            0, NI_NUMERICHOST) == 0) {
                lauxh_pushstr2tbl(L, "netmask", host);
            }
            if (ifa->ifa_flags & IFF_BROADCAST &&
                getnameinfo(ifa->ifa_broadaddr, addrlen, host, NI_MAXHOST, NULL,
                            0, NI_NUMERICHOST) == 0) {
                lauxh_pushstr2tbl(L, "broadcast", host);
            }
            if (ifa->ifa_flags & IFF_POINTOPOINT &&
                getnameinfo(ifa->ifa_dstaddr, addrlen, host, NI_MAXHOST, NULL,
                            0, NI_NUMERICHOST) == 0) {
                lauxh_pushstr2tbl(L, "point2point", host);
            }
            lua_rawseti(L, -2, lauxh_rawlen(L, -2) + 1);
            lua_pop(L, 1);
            lua_pop(L, 1);
        }

        freeifaddrs(list);
        return 1;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

LUALIB_API int luaopen_llsocket_device(lua_State *L)
{
    struct luaL_Reg method[] = {
        {"macaddrs",   macaddrs_lua  },
        {"getifaddrs", getifaddrs_lua},
        {NULL,         NULL          }
    };
    struct luaL_Reg *ptr = method;

    lua_newtable(L);
    do {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
        ptr++;
    } while (ptr->name);

    return 1;
}
