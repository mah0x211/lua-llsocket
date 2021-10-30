#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
AC_PREREQ([2.69])
AC_INIT([llsocket],[0.1.0])
AM_INIT_AUTOMAKE([foreign no-dist no-installinfo no-installman -Wall])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

#
# Checks for programs.
#
AC_LANG([C])
AC_PROG_CC

#
# define luarocks variables
#
AC_SUBST([LIBFLAG], ["$LIBFLAG"])
AC_SUBST([OBJ_EXTENSION], ["$OBJ_EXTENSION"])
AC_SUBST([LIB_EXTENSION], ["$LIB_EXTENSION"])
AC_SUBST([LIBDIR], ["$LIBDIR"])
AC_SUBST([LIBS], ["$LIBS"])
AC_SUBST([TMPLDIR], ["$TMPLDIR"])
AC_SUBST([VARDIR], ["$VARDIR"])

#
# checking headers
#
AC_CHECK_HEADERS(
    stdlib.h \
    unistd.h \
    stdint.h \
    string.h \
    errno.h \
    signal.h \
    math.h \
    time.h \
    fcntl.h \
    netdb.h \
    sys/socket.h \
    sys/un.h \
    netdb.h \
    netinet/in.h \
    netinet/tcp.h \
    arpa/inet.h \
    net/if.h \
    sys/ioctl.h \
    ifaddrs.h,,
    AC_MSG_FAILURE([required header not found])
)
AC_CHECK_HEADERS(
    net/if_dl.h
)


#
# checking types
#
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_INTPTR_T


AC_CHECK_TYPES([
    socklen_t,
    struct in_addr,
    struct sockaddr,
    struct sockaddr_in,
    struct sockaddr_un,
    struct sockaddr_storage,
    struct addrinfo,
    struct ifaddrs,
    struct ifreq,
    struct ifconf,
    struct group_req,
    struct ip_mreq,
    struct ipv6_mreq,
    struct ip_mreq_source],,
    AC_MSG_FAILURE([required type not found]),
    [#include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>
    #include <signal.h>
    #include <math.h>
    #include <time.h>
    #include <stdint.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <sys/ioctl.h>
    #include <ifaddrs.h>]
)


#
# check data structure
#
# check for struct sockaddr.sa_len member
AC_MSG_CHECKING(for struct sockaddr.sa_len)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/socket.h>]], [[struct sockaddr x;
x.sa_len = 0;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_SOCKADDR_SA_LEN, 1, [Define to 1 if the `sockaddr' type has the `.sa_len' member])],
  [AC_MSG_RESULT(no)]
)


#
# checking functions
#
AC_CHECK_FUNCS([ \
    memset \
    memmove \
    memcpy \
    strncpy \
    ntohs \
    inet_ntop \
    snprintf \
    fcntl \
    ioctl \
    dup \
    shutdown \
    close \
    socketpair \
    socket \
    connect \
    bind \
    listen \
    accept \
    send \
    sendto \
    sendfile \
    recv \
    recvfrom \
    setsockopt \
    getsockopt \
    sockatmark \
    getaddrinfo \
    freeaddrinfo \
    getsockname \
    getnameinfo \
    getpeername \
    getifaddrs \
    freeifaddrs \
    if_indextoname \
    if_nametoindex \
    gai_strerror],,
    AC_MSG_FAILURE([required function not found])
)

AC_CHECK_FUNCS([ \
    accept4
])


AC_CONFIG_FILES([ \
    Makefile \
    src/Makefile \
])
AC_OUTPUT
