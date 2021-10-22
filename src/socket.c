/**
 *  Copyright (C) 2015-2021 Masatoshi Fukunaga
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
 *  socket.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 15/12/17.
 */

#include "llsocket.h"

#define DEFAULT_RECVSIZE 4096

static const char *const SA_STORAGE_BUF[sizeof(struct sockaddr_storage)] = {0};
#define SOCKADDR_STORAGE_INITIALIZER                                           \
 *((struct sockaddr_storage *)SA_STORAGE_BUF)

typedef struct {
    int fd;
    int family;
    int socktype;
    int protocol;
    socklen_t addrlen;
    struct sockaddr_storage addr;
} lls_socket_t;

// MARK: fd option
static int cloexec_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    return lls_fcntl_lua(L, s->fd, F_GETFD, F_SETFD, FD_CLOEXEC);
}

static int nonblock_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    return lls_fcntl_lua(L, s->fd, F_GETFL, F_SETFL, O_NONBLOCK);
}

// MARK: socket option
static inline int sockopt_int_lua(lua_State *L, int level, int optname,
                                  int type)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    return lls_sockopt_int_lua(L, s->fd, level, optname, type);
}

// multicast

static int mcastloop_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        switch (s->family) {
        case AF_INET:
            return lls_sockopt_int_lua(L, s->fd, IPPROTO_IP, IP_MULTICAST_LOOP,
                                       LUA_TBOOLEAN);

        case AF_INET6:
            return lls_sockopt_int_lua(L, s->fd, IPPROTO_IPV6,
                                       IPV6_MULTICAST_LOOP, LUA_TBOOLEAN);
        }

    default:
        lua_pushnil(L);
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 2;
    }
}

static int mcastttl_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        switch (s->family) {
        case AF_INET:
            return lls_sockopt_int_lua(L, s->fd, IPPROTO_IP, IP_MULTICAST_TTL,
                                       LUA_TNUMBER);

        case AF_INET6:
            return lls_sockopt_int_lua(L, s->fd, IPPROTO_IPV6,
                                       IPV6_MULTICAST_HOPS, LUA_TNUMBER);
        }

    default:
        lua_pushnil(L);
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 2;
    }
}

static int mcastif_lua(lua_State *L)
{
    lls_socket_t *s     = lauxh_checkudata(L, 1, SOCKET_MT);
    unsigned int ifidx  = 0;
    socklen_t len       = sizeof(struct in_addr);
    struct in_addr addr = {0};

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        switch (s->family) {
        case AF_INET:
            if (lua_isnoneornil(L, 2)) {
                len = sizeof(addr);
                if (getsockopt(s->fd, IPPROTO_IP, IP_MULTICAST_IF,
                               (void *)&addr, &len) == 0) {
                    struct ifaddrs *list = NULL;

                    if (getifaddrs(&list) == 0) {
                        struct ifaddrs *ptr          = list;
                        struct sockaddr_in *ifa_addr = NULL;

                        for (ptr = list; ptr; ptr = ptr->ifa_next) {
                            ifa_addr = (struct sockaddr_in *)ptr->ifa_addr;
                            if (ptr->ifa_addr->sa_family == AF_INET &&
                                addr.s_addr == ifa_addr->sin_addr.s_addr) {
                                lua_pushstring(L, ptr->ifa_name);
                                freeifaddrs(list);
                                return 1;
                            }
                        }

                        freeifaddrs(list);
                    }
                }
            } else {
                const char *ifname = lauxh_checkstring(L, 2);
                struct ifreq ifr;

                strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
                // get interface address
                if (ioctl(s->fd, SIOCGIFADDR, &ifr) == 0) {
                    addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
                    // set address to multicast_if
                    if (setsockopt(s->fd, IPPROTO_IP, IP_MULTICAST_IF,
                                   (void *)&addr, sizeof(addr)) == 0) {
                        lua_settop(L, 2);
                        return 1;
                    }
                }
            }
            break;

        case AF_INET6:
            if (lua_isnoneornil(L, 2)) {
                len = sizeof(ifidx);

                if (getsockopt(s->fd, IPPROTO_IPV6, IPV6_MULTICAST_IF,
                               (void *)&ifidx, &len) == 0) {
                    char ifname[IFNAMSIZ] = {0};

                    if (if_indextoname(ifidx, ifname)) {
                        lua_pushstring(L, ifname);
                        return 1;
                    }
                }
            } else {
                const char *ifname = lauxh_checkstring(L, 2);

                if ((ifidx = if_nametoindex(ifname)) != 0 &&
                    setsockopt(s->fd, IPPROTO_IPV6, IPV6_MULTICAST_IF,
                               (void *)&ifidx, sizeof(ifidx)) == 0) {
                    lua_settop(L, 2);
                    return 1;
                }
            }
            break;
        }

    default:
        errno = EOPNOTSUPP;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static inline int mcastgroup_lua(lua_State *L, lls_socket_t *s, int family,
                                 int proto, int opt)
{
    struct group_req gr;
    int rc = lls_checksockaddr(L, 2, family, s->socktype, &gr.gr_group);
    const char *ifname = NULL;

    // check arguments
    if (rc != 0) {
        lua_pushstring(L, gai_strerror(rc));
        return 1;
    }

    gr.gr_interface = 0;
    ifname          = lauxh_optstring(L, 3, NULL);
    if ((!ifname || (gr.gr_interface = if_nametoindex(ifname)) != 0) &&
        setsockopt(s->fd, proto, opt, (void *)&gr, sizeof(struct group_req)) ==
            0) {
        return 0;
    }

    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static inline int mcast4group_lua(lua_State *L, lls_socket_t *s, int opt)
{
    struct ip_mreq mr  = {.imr_multiaddr = {INADDR_ANY}, .imr_interface = {0}};
    int rc             = lls_check4inaddr(L, 2, s->socktype, &mr.imr_multiaddr);
    const char *ifname = NULL;

    if (rc != 0) {
        lua_pushstring(L, gai_strerror(rc));
        return 1;
    }

    ifname = lauxh_optstring(L, 3, NULL);
    if (ifname) {
        struct ifreq ifr;

        strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
        // get interface address
        if (ioctl(s->fd, SIOCGIFADDR, &ifr) != 0) {
            goto FAILED;
        }
        // set in_addr
        mr.imr_interface = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
    } else {
        mr.imr_interface.s_addr = INADDR_ANY;
    }

    if (setsockopt(s->fd, IPPROTO_IP, opt, (void *)&mr,
                   sizeof(struct ip_mreq)) == 0) {
        return 0;
    }

FAILED:
    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static inline int mcast6group_lua(lua_State *L, lls_socket_t *s, int opt)
{
    struct ipv6_mreq mr = {.ipv6mr_multiaddr = IN6ADDR_ANY_INIT,
                           .ipv6mr_interface = 0};
    int rc = lls_check6inaddr(L, 2, s->socktype, &mr.ipv6mr_multiaddr);
    const char *ifname = NULL;

    if (rc != 0) {
        lua_pushstring(L, gai_strerror(rc));
        return 1;
    }

    ifname = lauxh_optstring(L, 3, NULL);
    if ((!ifname || (mr.ipv6mr_interface = if_nametoindex(ifname)) != 0) &&
        setsockopt(s->fd, IPPROTO_IPV6, opt, (void *)&mr,
                   sizeof(struct ipv6_mreq)) == 0) {
        return 0;
    }

    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static int mcastjoin_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4group_lua(L, s, IP_ADD_MEMBERSHIP);

        case AF_INET6:
            return mcast6group_lua(L, s, IPV6_JOIN_GROUP);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

static int mcastleave_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4group_lua(L, s, IP_DROP_MEMBERSHIP);

        case AF_INET6:
            return mcast6group_lua(L, s, IPV6_LEAVE_GROUP);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

static inline int mcast4srcgroup_lua(lua_State *L, lls_socket_t *s, int opt)
{
    struct ip_mreq_source mr = {
        .imr_multiaddr = {0}, .imr_sourceaddr = {0}, .imr_interface = {0}};
    int rc             = lls_check4inaddr(L, 2, s->socktype, &mr.imr_multiaddr);
    const char *ifname = NULL;

    // check arguments
    if (rc != 0) {
        lua_pushstring(L, gai_strerror(rc));
        return 1;
    }

    rc = lls_check4inaddr(L, 3, s->socktype, &mr.imr_sourceaddr);
    if (rc != 0) {
        lua_pushstring(L, gai_strerror(rc));
        return 1;
    }

    ifname = lauxh_optstring(L, 4, NULL);
    if (ifname) {
        struct ifreq ifr;

        strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
        // get interface address
        if (ioctl(s->fd, SIOCGIFADDR, &ifr) != 0) {
            goto FAILED;
        }
        // set in_addr
        mr.imr_interface = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
    } else {
        mr.imr_interface.s_addr = INADDR_ANY;
    }

    if (setsockopt(s->fd, IPPROTO_IP, opt, (void *)&mr,
                   sizeof(struct ip_mreq_source)) == 0) {
        return 0;
    }

FAILED:
    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static int mcastjoinsrc_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4srcgroup_lua(L, s, IP_ADD_SOURCE_MEMBERSHIP);

        case AF_INET6:
            return mcastgroup_lua(L, s, AF_INET6, IPPROTO_IPV6,
                                  MCAST_JOIN_SOURCE_GROUP);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

static int mcastleavesrc_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4srcgroup_lua(L, s, IP_DROP_SOURCE_MEMBERSHIP);

        case AF_INET6:
            return mcastgroup_lua(L, s, AF_INET6, IPPROTO_IPV6,
                                  MCAST_LEAVE_SOURCE_GROUP);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

static int mcastblocksrc_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4srcgroup_lua(L, s, IP_BLOCK_SOURCE);

        case AF_INET6:
            return mcastgroup_lua(L, s, AF_INET6, IPPROTO_IPV6,
                                  MCAST_BLOCK_SOURCE);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

static int mcastunblocksrc_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    switch (s->socktype) {
    case SOCK_RAW:
    case SOCK_DGRAM:
        // check socket family
        switch (s->family) {
        case AF_INET:
            return mcast4srcgroup_lua(L, s, IP_UNBLOCK_SOURCE);

        case AF_INET6:
            return mcastgroup_lua(L, s, AF_INET6, IPPROTO_IPV6,
                                  MCAST_UNBLOCK_SOURCE);
        }

    default:
        lua_pushstring(L, strerror(EOPNOTSUPP));
        return 1;
    }
}

// readonly

static int error_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_ERROR, LUA_TNUMBER);
}

static int acceptconn_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_ACCEPTCONN, LUA_TBOOLEAN);
}

// writable
static int tcpnodelay_lua(lua_State *L)
{
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_NODELAY, LUA_TBOOLEAN);
}

static int tcpkeepintvl_lua(lua_State *L)
{
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_KEEPINTVL, LUA_TNUMBER);
}

static int tcpkeepcnt_lua(lua_State *L)
{
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_KEEPCNT, LUA_TNUMBER);
}

static int tcpkeepalive_lua(lua_State *L)
{
#if defined(TCP_KEEPALIVE)
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_KEEPALIVE, LUA_TNUMBER);

#elif defined(TCP_KEEPIDLE)
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_KEEPIDLE, LUA_TNUMBER);

#else
    // tcpkeepalive does not implemented in this platform
    lua_pushnil(L);
    lua_pushstring(L, strerror(ENOPROTOOPT));
    return 2;

#endif
}

static int tcpcork_lua(lua_State *L)
{
#if defined(TCP_CORK)
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_CORK, LUA_TBOOLEAN);

#elif defined(TCP_NOPUSH)
    return sockopt_int_lua(L, IPPROTO_TCP, TCP_NOPUSH, LUA_TBOOLEAN);

#else
    // tcpcork does not implmeneted in this platform
    lua_pushnil(L);
    lua_pushstring(L, strerror(ENOPROTOOPT));
    return 2;

#endif
}

static int reuseport_lua(lua_State *L)
{
#if defined(SO_REUSEPORT)
    return sockopt_int_lua(L, SOL_SOCKET, SO_REUSEPORT, LUA_TBOOLEAN);

#else
    // reuseport does not implmeneted in this platform
    lua_pushnil(L);
    lua_pushstring(L, strerror(ENOPROTOOPT));
    return 2;

#endif
}

static int reuseaddr_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_REUSEADDR, LUA_TBOOLEAN);
}

static int broadcast_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_BROADCAST, LUA_TBOOLEAN);
}

static int debug_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_DEBUG, LUA_TBOOLEAN);
}

static int keepalive_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_KEEPALIVE, LUA_TBOOLEAN);
}

static int oobinline_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_OOBINLINE, LUA_TBOOLEAN);
}

static int dontroute_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_DONTROUTE, LUA_TBOOLEAN);
}

static int timestamp_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_TIMESTAMP, LUA_TBOOLEAN);
}

static int rcvbuf_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_RCVBUF, LUA_TNUMBER);
}

static int rcvlowat_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_RCVLOWAT, LUA_TNUMBER);
}

static int sndbuf_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_SNDBUF, LUA_TNUMBER);
}

static int sndlowat_lua(lua_State *L)
{
    return sockopt_int_lua(L, SOL_SOCKET, SO_SNDLOWAT, LUA_TNUMBER);
}

static inline int sockopt_timeval_lua(lua_State *L, int level, int opt)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    return lls_sockopt_timeval_lua(L, s->fd, level, opt);
}

static int rcvtimeo_lua(lua_State *L)
{
    return sockopt_timeval_lua(L, SOL_SOCKET, SO_RCVTIMEO);
}

static int sndtimeo_lua(lua_State *L)
{
    return sockopt_timeval_lua(L, SOL_SOCKET, SO_SNDTIMEO);
}

static int linger_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    struct linger l = {0, 0};
    socklen_t len   = sizeof(struct linger);
    int rc          = 0;
#if defined(SO_LINGER_SEC)
    int opt = SO_LINGER_SEC;
#else
    int opt = SO_LINGER;
#endif

    // set linger option
    if (!lua_isnoneornil(L, 2)) {
        l.l_linger = lauxh_checkinteger(L, 2);
        l.l_onoff  = l.l_linger >= 0;
        rc         = setsockopt(s->fd, SOL_SOCKET, opt, (void *)&l, len);
    }
    // get linger option
    else {
        rc = getsockopt(s->fd, SOL_SOCKET, opt, (void *)&l, &len);
    }

    // got error
    if (rc) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    } else if (l.l_onoff) {
        lua_pushinteger(L, l.l_linger);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// MARK: state

static int atmark_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int rc          = sockatmark(s->fd);

    if (rc != -1) {
        lua_pushboolean(L, rc);
        return 1;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

// MARK: address info

static int getsockname_lua(lua_State *L)
{
    lls_socket_t *s      = lauxh_checkudata(L, 1, SOCKET_MT);
    struct addrinfo wrap = {.ai_flags     = 0,
                            .ai_family    = s->family,
                            .ai_socktype  = s->socktype,
                            .ai_protocol  = s->protocol,
                            .ai_addrlen   = s->addrlen,
                            .ai_addr      = (struct sockaddr *)&s->addr,
                            .ai_canonname = NULL,
                            .ai_next      = NULL};

    // push llsocket.addr udata
    lls_addrinfo_alloc(L, &wrap);

    return 1;
}

static int getpeername_lua(lua_State *L)
{
    lls_socket_t *s              = lauxh_checkudata(L, 1, SOCKET_MT);
    socklen_t len                = sizeof(struct sockaddr_storage);
    struct sockaddr_storage addr = SOCKADDR_STORAGE_INITIALIZER;

    if (getpeername(s->fd, (struct sockaddr *)&addr, &len) == 0) {
        struct addrinfo wrap = {.ai_flags     = 0,
                                .ai_family    = s->family,
                                .ai_socktype  = s->socktype,
                                .ai_protocol  = s->protocol,
                                .ai_addrlen   = len,
                                .ai_addr      = (struct sockaddr *)&addr,
                                .ai_canonname = NULL,
                                .ai_next      = NULL};
        lls_addrinfo_alloc(L, &wrap);
        return 1;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

// MARK: method

static inline int shutdownfd(lua_State *L, int fd, int how)
{
    if (shutdown(fd, how) == 0) {
        return 0;
    }

    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static int shutdown_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int how         = (int)lauxh_optinteger(L, 2, SHUT_RDWR);

    return shutdownfd(L, s->fd, how);
}

static inline int closefd(lua_State *L, int fd)
{
    int how = (int)lauxh_optinteger(L, 2, -1);
    int rc  = 0;

    switch (how) {
    case SHUT_RD:
    case SHUT_WR:
    case SHUT_RDWR:
        rc = shutdown(fd, how);

    default:
        rc += close(fd);
    }

    // got error
    if (rc) {
        lua_pushstring(L, strerror(errno));
        return 1;
    }

    return 0;
}

static int close_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = s->fd;

    if (fd != -1) {
        s->fd = -1;
        return closefd(L, fd);
    }

    return 0;
}

static int listen_lua(lua_State *L)
{
    lls_socket_t *s     = lauxh_checkudata(L, 1, SOCKET_MT);
    // default backlog size
    lua_Integer backlog = lauxh_optinteger(L, 2, SOMAXCONN);

    // check args
    if (backlog < 1 || backlog > INT_MAX) {
        return luaL_error(L, "backlog range must be 1 to %d", INT_MAX);
    }
    // listen
    else if (listen(s->fd, (int)backlog) != 0) {
        // got error
        lua_pushstring(L, strerror(errno));
        return 1;
    }

    return 0;
}

static inline int acceptfd(int sfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int flg = fcntl(sfd, F_GETFL);

    if (flg != -1) {
#if defined(HAVE_ACCEPT4)
        flg = SOCK_CLOEXEC | ((flg & O_NONBLOCK) ? SOCK_NONBLOCK : 0);
        return accept4(sfd, addr, addrlen, flg);

#else
        int fd = accept(sfd, addr, addrlen);

        if (fd != -1) {
            if (fcntl(fd, F_SETFD, FD_CLOEXEC) == 0 &&
                fcntl(fd, F_SETFL, flg) == 0) {
                return fd;
            }
            close(fd);
        }
#endif
    }

    return -1;
}

static int acceptfd_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = acceptfd(s->fd, NULL, NULL);

    if (fd != -1) {
        lua_pushinteger(L, fd);
        return 1;
    }

    // check errno
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ||
        errno == ECONNABORTED) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int accept_lua(lua_State *L)
{
    lls_socket_t *s              = lauxh_checkudata(L, 1, SOCKET_MT);
    struct sockaddr_storage addr = SOCKADDR_STORAGE_INITIALIZER;
    socklen_t addrlen            = sizeof(struct sockaddr_storage);
    int fd = acceptfd(s->fd, (struct sockaddr *)&addr, &addrlen);

    if (fd != -1) {
        lls_socket_t *cs = lua_newuserdata(L, sizeof(lls_socket_t));

        cs->fd       = fd;
        cs->family   = s->family;
        cs->socktype = s->socktype;
        cs->protocol = s->protocol;
        cs->addrlen  = addrlen;
        // copy sockaddr
        memcpy((void *)&cs->addr, (void *)&addr, addrlen);
        lauxh_setmetatable(L, SOCKET_MT);

        return 1;
    }

    // check errno
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ||
        errno == ECONNABORTED) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int send_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    size_t len      = 0;
    const char *buf = lauxh_checklstring(L, 2, &len);
    int flg         = lauxh_optflags(L, 3);
    ssize_t rv      = 0;

    // invalid length
    if (!len) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 2;
    }

    rv = send(s->fd, buf, len, flg);
    switch (rv) {
    // got error
    case -1:
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // closed by peer
        else if (errno == EPIPE || errno == ECONNRESET) {
            return 0;
        }
        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, rv);
        lua_pushnil(L);
        lua_pushboolean(L, len - (size_t)rv);
        return 3;
    }
}

static int sendto_lua(lua_State *L)
{
    lls_socket_t *s      = lauxh_checkudata(L, 1, SOCKET_MT);
    size_t len           = 0;
    const char *buf      = lauxh_checklstring(L, 2, &len);
    lls_addrinfo_t *info = lauxh_checkudata(L, 3, ADDRINFO_MT);
    int flg              = lauxh_optflags(L, 4);
    ssize_t rv           = 0;

    // invalid length
    if (!len) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 2;
    }

    rv = sendto(s->fd, buf, len, flg, (const struct sockaddr *)info->ai.ai_addr,
                info->ai.ai_addrlen);
    switch (rv) {
    // got error
    case -1:
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // closed by peer
        else if (errno == EPIPE || errno == ECONNRESET) {
            return 0;
        }
        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, rv);
        lua_pushnil(L);
        lua_pushboolean(L, len - (size_t)rv);
        return 3;
    }
}

static int sendfd_lua(lua_State *L)
{
    lls_socket_t *s        = lauxh_checkudata(L, 1, SOCKET_MT);
    lua_Integer fd         = lauxh_checkinteger(L, 2);
    lls_addrinfo_t *info   = lauxh_optudata(L, 3, ADDRINFO_MT, NULL);
    int flg                = lauxh_optflags(L, 4);
    // init data
    struct iovec empty_iov = {.iov_base = NULL, .iov_len = 0};
    union {
        unsigned char buf[CMSG_SPACE(sizeof(int))];
        struct cmsghdr data;
    } ctrl             = {.data.cmsg_len   = CMSG_LEN(sizeof(int)),
                          .data.cmsg_level = SOL_SOCKET,
                          .data.cmsg_type  = SCM_RIGHTS};
    struct msghdr data = {.msg_name       = NULL,
                          .msg_namelen    = 0,
                          .msg_iov        = &empty_iov,
                          .msg_iovlen     = 1,
                          .msg_control    = &ctrl.data,
                          .msg_controllen = ctrl.data.cmsg_len,
                          .msg_flags      = 0};

    // set fd
    *(int *)CMSG_DATA(&ctrl.data) = fd;

    // set msg_name
    if (info) {
        data.msg_name    = (void *)&(info->ai.ai_addr);
        data.msg_namelen = info->ai.ai_addrlen;
    }

    switch (sendmsg(s->fd, &data, flg)) {
    // got error
    case -1:
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ||
            errno == EMSGSIZE) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // closed by peer
        else if (errno == EPIPE || errno == ECONNRESET) {
            return 0;
        }
        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, 0);
        return 1;
    }
}

static int sendmsg_lua(lua_State *L)
{
    lls_socket_t *s           = lauxh_checkudata(L, 1, SOCKET_MT);
    lls_msghdr_t *lmsg        = lauxh_checkudata(L, 2, MSGHDR_MT);
    int flg                   = lauxh_optflags(L, 3);
    // init data
    struct iovec iov[IOV_MAX] = {0};
    struct msghdr data        = {.msg_name       = NULL,
                                 .msg_namelen    = 0,
                                 .msg_iov        = iov,
                                 .msg_iovlen     = 1,
                                 .msg_control    = NULL,
                                 .msg_controllen = 0,
                                 .msg_flags      = 0};
    size_t len                = 0;
    ssize_t rv                = 0;

    // set msg_name
    if (lmsg->name) {
        data.msg_name    = (void *)&(lmsg->name->ai_addr);
        data.msg_namelen = lmsg->name->ai_addrlen;
    }
    // set msg_iov
    if (lmsg->iov && lmsg->iov->nbyte) {
        len             = lmsg->iov->nbyte;
        data.msg_iovlen = IOV_MAX;
        lua_iovec_setv(lmsg->iov, iov, &data.msg_iovlen, 0, 0);
    }
    // set msg_control
    if (lmsg->control && lmsg->control->len) {
        data.msg_control    = lmsg->control->data;
        data.msg_controllen = lmsg->control->len;
    }

    rv          = sendmsg(s->fd, &data, flg);
    lmsg->flags = data.msg_flags;
    switch (rv) {
    // got error
    case -1:
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // send buffer is insufficient
        else if (errno == EMSGSIZE) {
            lua_pushinteger(L, -1);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // closed by peer
        else if (errno == EPIPE || errno == ECONNRESET) {
            return 0;
        }
        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, rv);
        lua_pushnil(L);
        lua_pushboolean(L, len - (size_t)rv);
        return 3;
    }
}

#if defined(HAVE_SENDFILE)

# if defined(__linux__)
#  include <sys/sendfile.h>

static int sendfile_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = (int)lauxh_checkinteger(L, 2);
    size_t len      = (size_t)lauxh_checkinteger(L, 3);
    off_t offset    = (off_t)lauxh_optinteger(L, 4, 0);
    ssize_t rv      = 0;

    // invalid length
    if (!len) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 2;
    } else if ((rv = sendfile(s->fd, fd, &offset, len)) != -1) {
        lua_pushinteger(L, rv);
        lua_pushnil(L);
        lua_pushboolean(L, len - (size_t)rv);
        return 3;
    }
    // again
    else if (errno == EAGAIN || errno == EINTR) {
        lua_pushinteger(L, 0);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;
    }
    // closed by peer
    else if (errno == EPIPE || errno == ECONNRESET) {
        return 0;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

# elif defined(__APPLE__)

static int sendfile_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = (int)lauxh_checkinteger(L, 2);
    off_t len       = (off_t)lauxh_checkinteger(L, 3);
    off_t offset    = (off_t)lauxh_optinteger(L, 4, 0);

    // invalid length
    if (!len) {
        errno = EINVAL;
    } else if (sendfile(fd, s->fd, offset, &len, NULL, 0) == 0) {
        lua_pushinteger(L, len);
        return 1;
    }
    // again
    else if (errno == EAGAIN || errno == EINTR) {
        lua_pushinteger(L, len);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;
    }
    // closed by peer
    else if (errno == EPIPE) {
        return 0;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

# elif defined(__DragonFly__) || defined(__FreeBSD__) ||                       \
     defined(__NetBSD__) || defined(__OpenBSD__)

static int sendfile_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = (int)lauxh_checkinteger(L, 2);
    size_t len      = (size_t)lauxh_checkinteger(L, 3);
    off_t offset    = (off_t)lauxh_optinteger(L, 4, 0);
    off_t nbytes    = 0;

    // invalid length
    if (!len) {
        errno = EINVAL;
    } else if (sendfile(fd, s->fd, offset, len, NULL, &nbytes, 0) == 0) {
        lua_pushinteger(L, nbytes);
        return 1;
    }
    // again
    else if (errno == EAGAIN || errno == EINTR) {
        lua_pushinteger(L, nbytes);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;
    }
    // closed by peer
    else if (errno == EPIPE) {
        return 0;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

# else

// sendfile does not supported in this platform
#  undef HAVE_SENDFILE

# endif
#endif

#if !defined(HAVE_SENDFILE)

// sendfile implements for unsupported platform
static int sendfile_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    int fd          = (int)lauxh_checkinteger(L, 2);
    size_t len      = (size_t)lauxh_checkinteger(L, 3);
    off_t offset    = (off_t)lauxh_optinteger(L, 4, 0);
    ssize_t nbytes  = 0;
    void *buf       = NULL;

    lua_settop(L, 0);

    // invalid length
    if (!len) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 2;
    }

    // read data from file
    buf    = lua_newuserdata(L, len);
    nbytes = pread(fd, buf, len, offset);
    // reached to end-of-file
    if (!nbytes) {
        lua_pushinteger(L, 0);
        return 1;
    }
    // got error
    else if (nbytes == -1) {
        // again
        if (errno == EAGAIN || errno == EINTR) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }

        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    nbytes = send(s->fd, buf, nbytes, 0);
    switch (nbytes) {
    // got error
    case -1:
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // closed by peer
        else if (errno == EPIPE || errno == ECONNRESET) {
            return 0;
        }
        // got error
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, nbytes);
        lua_pushnil(L);
        lua_pushboolean(L, len - (size_t)nbytes);
        return 3;
    }
}

#endif

static int recv_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);
    lua_Integer len = lauxh_optinteger(L, 2, DEFAULT_RECVSIZE);
    int flg         = lauxh_optflags(L, 3);
    char *buf       = NULL;
    ssize_t rv      = 0;

    lua_settop(L, 0);

    // invalid length
    if (len <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 2;
    }

    buf = lua_newuserdata(L, len);
    rv  = recv(s->fd, buf, (size_t)len, flg);
    switch (rv) {
    // got error
    case -1:
        lua_pushnil(L);
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // got error
        lua_pushstring(L, strerror(errno));
        return 2;

    case 0:
        // close by peer
        if (s->socktype != SOCK_DGRAM && s->socktype != SOCK_RAW) {
            return 0;
        }
        // fall through

    default:
        lua_pushlstring(L, buf, rv);
        return 1;
    }
}

static int recvfrom_lua(lua_State *L)
{
    lls_socket_t *s             = lauxh_checkudata(L, 1, SOCKET_MT);
    lua_Integer len             = lauxh_optinteger(L, 2, DEFAULT_RECVSIZE);
    int flg                     = lauxh_optflags(L, 3);
    socklen_t slen              = sizeof(struct sockaddr_storage);
    struct sockaddr_storage src = SOCKADDR_STORAGE_INITIALIZER;
    ssize_t rv                  = 0;
    char *buf                   = NULL;

    // invalid length
    if (len <= 0) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushstring(L, strerror(EINVAL));
        return 3;
    }

    buf = lua_newuserdata(L, len);
    rv = recvfrom(s->fd, buf, (size_t)len, flg, (struct sockaddr *)&src, &slen);
    switch (rv) {
    // got error
    case -1:
        lua_pushnil(L);
        lua_pushnil(L);
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 4;
        }
        // got error
        lua_pushstring(L, strerror(errno));
        return 3;

    case 0:
        // close by peer
        if (s->socktype != SOCK_DGRAM && s->socktype != SOCK_RAW) {
            return 0;
        }
        // fall-through

    default:
        lua_pushlstring(L, buf, rv);
        if (slen > 0) {
            // with addrinfo
            struct addrinfo wrap = {.ai_flags     = 0,
                                    .ai_family    = s->family,
                                    .ai_socktype  = s->socktype,
                                    .ai_protocol  = s->protocol,
                                    .ai_addrlen   = slen,
                                    .ai_addr      = (struct sockaddr *)&src,
                                    .ai_canonname = NULL,
                                    .ai_next      = NULL};

            // push llsocket.addr udata
            lls_addrinfo_alloc(L, &wrap);
            return 2;
        }
        // no addrinfo
        return 1;
    }
}

static int recvfd_lua(lua_State *L)
{
    lls_socket_t *s        = lauxh_checkudata(L, 1, SOCKET_MT);
    int flg                = lauxh_optflags(L, 2);
    char empty_iov_base    = 0;
    struct iovec empty_iov = {.iov_base = &empty_iov_base,
                              .iov_len  = sizeof(empty_iov_base)};
    union {
        unsigned char buf[CMSG_SPACE(sizeof(int))];
        struct cmsghdr data;
    } ctrl             = {.data.cmsg_len   = CMSG_LEN(sizeof(int)),
                          .data.cmsg_level = 0,
                          .data.cmsg_type  = 0};
    struct msghdr data = (struct msghdr){.msg_name       = NULL,
                                         .msg_namelen    = 0,
                                         .msg_iov        = &empty_iov,
                                         .msg_iovlen     = 1,
                                         .msg_control    = &ctrl.data,
                                         .msg_controllen = ctrl.data.cmsg_len,
                                         .msg_flags      = 0};
    ssize_t rv         = recvmsg(s->fd, &data, flg);

    switch (rv) {
    // got error
    case -1:
        lua_pushnil(L);
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // got error
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        if (ctrl.data.cmsg_level == SOL_SOCKET &&
            ctrl.data.cmsg_type == SCM_RIGHTS) {
            lua_pushinteger(L, *(int *)CMSG_DATA(&ctrl.data));
            return 1;
        }
        // close by peer
        else if (!rv && s->socktype != SOCK_DGRAM && s->socktype != SOCK_RAW) {
            return 0;
        }

        // again - discard received messages
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushboolean(L, 1);

        return 3;
    }
}

static int recvmsg_lua(lua_State *L)
{
    lls_socket_t *s                      = lauxh_checkudata(L, 1, SOCKET_MT);
    lls_msghdr_t *lmsg                   = lauxh_checkudata(L, 2, MSGHDR_MT);
    int flg                              = lauxh_optflags(L, 3);
    unsigned char control[CMSG_SPACE(0)] = {0};
    struct iovec iov[IOV_MAX]            = {0};
    struct msghdr data                   = (struct msghdr){.msg_name       = NULL,
                                                           .msg_namelen    = 0,
                                                           .msg_iov        = iov,
                                                           .msg_iovlen     = 1,
                                                           .msg_control    = control,
                                                           .msg_controllen = sizeof(control),
                                                           .msg_flags      = 0};
    ssize_t rv                           = 0;

    // set msg_name
    if (lmsg->name) {
        data.msg_name    = (void *)&(lmsg->name->ai_addr);
        data.msg_namelen = sizeof(struct sockaddr_storage);
    }
    // set msg_iov
    if (lmsg->iov && lmsg->iov->nbyte) {
        data.msg_iovlen = IOV_MAX;
        lua_iovec_setv(lmsg->iov, data.msg_iov, &data.msg_iovlen, 0, 0);
    }
    // set msg_control
    if (lmsg->control && lmsg->control->len) {
        data.msg_control    = lmsg->control->data;
        data.msg_controllen = lmsg->control->len;
    }

    rv = recvmsg(s->fd, &data, flg);
    switch (rv) {
    // got error
    case -1:
        lua_pushnil(L);
        // again
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            return 3;
        }
        // got error
        lua_pushstring(L, strerror(errno));
        return 2;

    default:
        lua_pushinteger(L, rv);
        return 1;
    }
}

static int connect_lua(lua_State *L)
{
    lls_socket_t *s       = lauxh_checkudata(L, 1, SOCKET_MT);
    struct sockaddr *addr = (struct sockaddr *)&s->addr;
    socklen_t addrlen     = s->addrlen;

    // check argument
    if (!lua_isnoneornil(L, 2)) {
        lls_addrinfo_t *info = lauxh_checkudata(L, 2, ADDRINFO_MT);
        addr                 = info->ai.ai_addr;
        addrlen              = info->ai.ai_addrlen;
    }

    if (connect(s->fd, addr, addrlen) == 0) {
        return 0;
    }
    // nonblocking connect
    else if (errno == EINPROGRESS) {
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 2;
    }

    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static inline int select_lua(lua_State *L, int receivable, int sendable)
{
    lls_socket_t *s        = lauxh_checkudata(L, 1, SOCKET_MT);
    lua_Integer msec       = luaL_optinteger(L, 2, 0);
    int except             = lauxh_optboolean(L, 3, 0);
    struct timeval timeout = {.tv_sec = 0, .tv_usec = 0};
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    fd_set *rptr = NULL;
    fd_set *wptr = NULL;
    fd_set *eptr = NULL;

    lua_settop(L, 0);
    if (msec > 0) {
        timeout.tv_sec  = msec / 1000;
        timeout.tv_usec = msec % 1000 * 1000;
    }

    // select receivable
    if (receivable) {
        rptr = &rfds;
        FD_ZERO(rptr);
        FD_SET(s->fd, rptr);
    }
    // select sendable
    if (sendable) {
        wptr = &wfds;
        FD_ZERO(wptr);
        FD_SET(s->fd, wptr);
    }
    // select exception
    if (except) {
        eptr = &efds;
        FD_ZERO(eptr);
        FD_SET(s->fd, eptr);
    }

    // wait until usable or exceeded timeout
    switch (select(s->fd + 1, rptr, wptr, eptr, &timeout)) {
    // timeout
    case 0:
        lua_pushboolean(L, 0);
        lua_pushnil(L);
        lua_pushboolean(L, 1);
        return 3;

    case -1:
        lua_pushboolean(L, 0);
        lua_pushstring(L, strerror(errno));
        return 2;

    // selected
    default:
        lua_pushboolean(L, 1);
        return 1;
    }
}

static int sendable_lua(lua_State *L)
{
    return select_lua(L, 0, 1);
}

static int recvable_lua(lua_State *L)
{
    return select_lua(L, 1, 0);
}

static int bind_lua(lua_State *L)
{
    lls_socket_t *s       = lauxh_checkudata(L, 1, SOCKET_MT);
    struct sockaddr *addr = (struct sockaddr *)&s->addr;
    socklen_t addrlen     = s->addrlen;

    // check argument
    if (!lua_isnoneornil(L, 2)) {
        lls_addrinfo_t *info = lauxh_checkudata(L, 2, ADDRINFO_MT);
        addr                 = info->ai.ai_addr;
        addrlen              = info->ai.ai_addrlen;
    }

    if (bind(s->fd, addr, addrlen) == 0) {
        return 0;
    }

    // got error
    lua_pushstring(L, strerror(errno));

    return 1;
}

static int protocol_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    lua_pushinteger(L, s->protocol);

    return 1;
}

static int socktype_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    lua_pushinteger(L, s->socktype);

    return 1;
}

static int family_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    lua_pushinteger(L, s->family);

    return 1;
}

static int fd_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    lua_pushinteger(L, s->fd);

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, SOCKET_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    if (s->fd != -1) {
        close(s->fd);
    }

    return 0;
}

static int dup_lua(lua_State *L)
{
    lls_socket_t *s     = lauxh_checkudata(L, 1, SOCKET_MT);
    lls_addrinfo_t info = {
        .ai = (struct addrinfo){.ai_family   = s->family,
                                .ai_socktype = s->socktype,
                                .ai_protocol = s->protocol,
                                .ai_addrlen  = s->addrlen,
                                .ai_addr     = (struct sockaddr *)&s->addr},
    };
    lls_addrinfo_t *ptr = &info;
    int fd              = 0;

    // check argument
    if (!lua_isnoneornil(L, 2)) {
        ptr = lauxh_checkudata(L, 2, ADDRINFO_MT);
    }

    if ((fd = dup(s->fd)) != -1) {
        lls_socket_t *sd = NULL;

        if (fcntl(fd, F_SETFD, FD_CLOEXEC) != -1) {
            sd  = lua_newuserdata(L, sizeof(lls_socket_t));
            *sd = (lls_socket_t){.fd       = fd,
                                 .family   = ptr->ai.ai_family,
                                 .socktype = ptr->ai.ai_socktype,
                                 .protocol = ptr->ai.ai_protocol,
                                 .addrlen  = ptr->ai.ai_addrlen};
            // copy sockaddr
            memcpy((void *)&sd->addr, (void *)&ptr->ai.ai_addr,
                   ptr->ai.ai_addrlen);
            lauxh_setmetatable(L, SOCKET_MT);
            return 1;
        }

        // got error
        close(fd);
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int unwrap_lua(lua_State *L)
{
    lls_socket_t *s = lauxh_checkudata(L, 1, SOCKET_MT);

    lua_settop(L, 1);
    // remove metatable
    lua_pushnil(L);
    lua_setmetatable(L, -2);
    // return fd and then disable
    lua_pushinteger(L, s->fd);
    s->fd = -1;

    return 1;
}

static int wrap_lua(lua_State *L)
{
    int fd            = (int)lauxh_checkinteger(L, 1);
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    lls_socket_t *s   = NULL;
    socklen_t typelen = sizeof(int);
#if defined(SO_PROTOCOL)
    socklen_t protolen = sizeof(int);
#endif

    lua_settop(L, 1);
    s = lua_newuserdata(L, sizeof(lls_socket_t));
    if (getsockname(fd, (void *)&s->addr, &addrlen) == 0 &&
#if defined(SO_PROTOCOL)
        getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &s->protocol, &protolen) == 0 &&
#endif
        getsockopt(fd, SOL_SOCKET, SO_TYPE, &s->socktype, &typelen) == 0) {
        lauxh_setmetatable(L, SOCKET_MT);
        s->fd     = fd;
        s->family = s->addr.ss_family;
#if !defined(SO_PROTOCOL)
        s->protocol = 0;
#endif
        s->addrlen = addrlen;
        return 1;
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int new_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);
    int nonblock         = lauxh_optboolean(L, 2, 0);
    // create socket
    int fd =
        socket(info->ai.ai_family, info->ai.ai_socktype, info->ai.ai_protocol);

    if (fd != -1) {
        lls_socket_t *s = NULL;
        int fl          = 0;

        lua_settop(L, 1);
        // set cloexec and nonblock flag
        if (fcntl(fd, F_SETFD, FD_CLOEXEC) != -1 &&
            (!nonblock || ((fl = fcntl(fd, F_GETFL)) != -1 &&
                           fcntl(fd, F_SETFL, fl | O_NONBLOCK) != -1))) {
            s  = lua_newuserdata(L, sizeof(lls_socket_t));
            *s = (lls_socket_t){.fd       = fd,
                                .family   = info->ai.ai_family,
                                .socktype = info->ai.ai_socktype,
                                .protocol = info->ai.ai_protocol,
                                .addrlen  = info->ai.ai_addrlen};
            // copy sockaddr
            memcpy((void *)&s->addr, (void *)info->ai.ai_addr,
                   info->ai.ai_addrlen);
            lauxh_setmetatable(L, SOCKET_MT);

            return 1;
        }

        // got error
        close(fd);
    }

    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int pair_lua(lua_State *L)
{
    int socktype                 = (int)lauxh_checkinteger(L, 1);
    int nonblock                 = lauxh_optboolean(L, 2, 0);
    int protocol                 = (int)lauxh_optinteger(L, 3, 0);
    struct sockaddr_storage addr = SOCKADDR_STORAGE_INITIALIZER;
    socklen_t addrlen            = sizeof(struct sockaddr_storage);
    int fds[2];

    if (socketpair(AF_UNIX, socktype, protocol, fds) == 0) {
        lls_socket_t *s = NULL;
        int i           = 0;

        lua_createtable(L, 2, 0);
        for (; i < 2; i++) {
            // set flags
            if (getsockname(fds[i], (void *)&addr, &addrlen) == 0 &&
                fcntl(fds[i], F_SETFD, FD_CLOEXEC) != -1 &&
                (!nonblock || fcntl(fds[i], F_SETFL, O_NONBLOCK) != -1)) {
                s  = lua_newuserdata(L, sizeof(lls_socket_t));
                *s = (lls_socket_t){.fd       = fds[i],
                                    .family   = AF_UNIX,
                                    .socktype = socktype,
                                    .protocol = protocol,
                                    .addrlen  = addrlen};
                // copy sockaddr
                memcpy((void *)&s->addr, (void *)&addr, addrlen);
                lauxh_setmetatable(L, SOCKET_MT);
                lua_rawseti(L, -2, i + 1);

                continue;
            }

            close(fds[0]);
            close(fds[1]);
            goto FAILED;
        }

        return 1;
    }

FAILED:
    // got error
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    return 2;
}

static int shutdownfd_lua(lua_State *L)
{
    int fd  = (int)lauxh_checkinteger(L, 1);
    int how = (int)lauxh_optinteger(L, 2, SHUT_RDWR);

    return shutdownfd(L, fd, how);
}

static int closefd_lua(lua_State *L)
{
    int fd = (int)lauxh_checkinteger(L, 1);

    return closefd(L, fd);
}

LUALIB_API int luaopen_llsocket_socket(lua_State *L)
{
    // create metatable
    if (luaL_newmetatable(L, SOCKET_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"unwrap",          unwrap_lua         },
            {"dup",             dup_lua            },
            {"fd",              fd_lua             },
            {"family",          family_lua         },
            {"socktype",        socktype_lua       },
            {"protocol",        protocol_lua       },
            {"bind",            bind_lua           },
            {"recvable",        recvable_lua       },
            {"sendable",        sendable_lua       },
            {"connect",         connect_lua        },
            {"shutdown",        shutdown_lua       },
            {"close",           close_lua          },
            {"listen",          listen_lua         },
            {"accept",          accept_lua         },
            {"acceptfd",        acceptfd_lua       },
            {"send",            send_lua           },
            {"sendto",          sendto_lua         },
            {"sendfd",          sendfd_lua         },
            {"sendmsg",         sendmsg_lua        },
            {"sendfile",        sendfile_lua       },
            {"recv",            recv_lua           },
            {"recvfrom",        recvfrom_lua       },
            {"recvfd",          recvfd_lua         },
            {"recvmsg",         recvmsg_lua        },

 // state
            {"atmark",          atmark_lua         },

 // address info
            {"getsockname",     getsockname_lua    },
            {"getpeername",     getpeername_lua    },

 // fd option
            {"cloexec",         cloexec_lua        },
            {"nonblock",        nonblock_lua       },

 // read-only socket option
            {"error",           error_lua          },
            {"acceptconn",      acceptconn_lua     },
 // socket option
            {"tcpnodelay",      tcpnodelay_lua     },
            {"tcpkeepintvl",    tcpkeepintvl_lua   },
            {"tcpkeepcnt",      tcpkeepcnt_lua     },
            {"tcpkeepalive",    tcpkeepalive_lua   },
            {"tcpcork",         tcpcork_lua        },
            {"reuseport",       reuseport_lua      },
            {"reuseaddr",       reuseaddr_lua      },
            {"broadcast",       broadcast_lua      },
            {"debug",           debug_lua          },
            {"keepalive",       keepalive_lua      },
            {"oobinline",       oobinline_lua      },
            {"dontroute",       dontroute_lua      },
            {"timestamp",       timestamp_lua      },
            {"rcvbuf",          rcvbuf_lua         },
            {"rcvlowat",        rcvlowat_lua       },
            {"sndbuf",          sndbuf_lua         },
            {"sndlowat",        sndlowat_lua       },
            {"rcvtimeo",        rcvtimeo_lua       },
            {"sndtimeo",        sndtimeo_lua       },
            {"linger",          linger_lua         },
 // multicast
            {"mcastloop",       mcastloop_lua      },
            {"mcastttl",        mcastttl_lua       },
            {"mcastif",         mcastif_lua        },
            {"mcastjoin",       mcastjoin_lua      },
            {"mcastleave",      mcastleave_lua     },
            {"mcastjoinsrc",    mcastjoinsrc_lua   },
            {"mcastleavesrc",   mcastleavesrc_lua  },
            {"mcastblocksrc",   mcastblocksrc_lua  },
            {"mcastunblocksrc", mcastunblocksrc_lua},
            {NULL,              NULL               }
        };
        struct luaL_Reg *ptr = mmethod;

        // lock metatable
        lauxh_pushnum2tbl(L, "__metatable", 1);
        // metamethods
        do {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        } while (ptr->name);
        // methods
        lua_pushstring(L, "__index");
        lua_newtable(L);
        ptr = method;
        do {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        } while (ptr->name);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);

    // create table
    lua_newtable(L);
    // method
    lauxh_pushfn2tbl(L, "new", new_lua);
    lauxh_pushfn2tbl(L, "wrap", wrap_lua);
    lauxh_pushfn2tbl(L, "pair", pair_lua);
    lauxh_pushfn2tbl(L, "close", closefd_lua);
    lauxh_pushfn2tbl(L, "shutdown", shutdownfd_lua);

    return 1;
}
