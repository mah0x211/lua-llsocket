# lua-llsocket

low-level socket module.

## Constants

these constants defined at the `llsocket.opt.*`

**for connect/bind**

- SOCK_DGRAM
- SOCK_STREAM
- SOCK_SEQPACKET
- SOCK_RAW

**for shutdown/close**

- SHUT_RD
- SHUT_WR
- SHUT_RDWR


## llsocket.inet.*

### connect/bind ( host, port, socktype, nonblock, reuseaddr )

create connected/bound socket.

**Parameters**

- host: host string.
- port: port number string.
- socktype: SOCK_*.
- nonblock: set nonblock flag to created socket if specified to true.
- reuseaddr: set reuseaddr flag to created socket if specified to true.

**Returns**

1. fd: socket.
2. err: error number.

## llsocket.unix.*

### connect/bind ( path, socktype, nonblock )

create connected/bound socket.

**Parameters**

- path: string.
- socktype: SOCK_*.
- nonblock: set nonblock flag to created socket if specified to true.

**Returns**

1. fd: socket.
2. err: error number.


## llsocket.*

### sockname( fd )

returns the current address for socket.

**Parameters**

- fd: socket.

**Returns**

1. address: address.
2. errno: error number.


### shutdown( fd, how )

shut down socket.

**Parameters**

- fd: socket
- how: SHUT_*

**Returns**

1. ok: boolean value.
2. errno: error number.

### close( fd [, how] )

close socket.

**Parameters**

- fd: socket.
- how: SHUT_*, call shutdown before closing if specified.

**Returns**

1. ok: boolean value.
2. errno: error number.


### listen( fd [, backlog] )

listen for connections on a socket.

**Parameters**

- fd: socket.
- backlog: maximum length for the queue of pending connections. default SOMAXCONN.

**Returns**

1. ok: boolean value.
2. errno: error number.


### accept( fd [, nonblock] )

accept a connection on a socket.

**Parameters**

- fd: socket.
- nonblock: set nonblock flag to connected socket if specified to true.

**Returns**

1. fd: connected socket.
2. errno: error number.


## llsocket.opt.*

### cloexec( fd[, flag] )

returns the current FD_CLOEXEC flag for socket.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current flag state as a boolean value.
2. errno: error number.

### nonblock( fd[, flag] )

returns the current O_NONBLOCK flag for socket.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current flag state as a boolean value.
2. errno: error number.

### nodelay( fd[, flag] )

returns the TCP_NODELAY value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### type( fd )

returns the SO_TYPE value of socket option. **read only**

**Parameters**

- fd: socket.

**Returns**

1. type: socket type as a number value.
2. errno: error number.


### reuseaddr( fd[, flag] )

returns the SO_REUSEADDR value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.

### broadcast( fd[, flag] )

returns the SO_BROADCAST value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### debug( fd[, flag] )

returns the SO_DEBUG value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### error( fd )

returns the SO_ERROR value of socket option. **read only**

**Parameters**

- fd: socket.

**Returns**

1. flag: current option state as a number value.
2. errno: error number.


### keepalive( fd[, flag] )

returns the SO_KEEPALIVE value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### oobinline( fd[, flag] )

returns the SO_OOBINLINE value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### timestamp( fd[, flag] )

returns the SO_TIMESTAMP value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. errno: error number.


### rcvbuf( fd[, size] )

returns the SO_RCVBUF value of socket option.

**Parameters**

- fd: socket.
- size: set size of receive buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. errno: error number.

### rcvlowat( fd[, size] )

returns the SO_RCVLOWAT value of socket option.

**Parameters**

- fd: socket.
- size: set minimum size of receive buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. errno: error number.


### sndbuf( fd[, size] )

returns the SO_SNDBUF value of socket option.

**Parameters**

- fd: socket.
- size: set size of send buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. errno: error number.

### sndlowat( fd[, size] )

returns the SO_SNDLOWAT value of socket option.

**Parameters**

- fd: socket.
- size: set minimum size of send buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. errno: error number.



### rcvtimeo( fd[, sec] )

returns the SO_RCVTIMEO value of socket option.

**Parameters**

- fd: socket.
- sec: set value of receive-timeout for socket if specified.

**Returns**

1. sec: current option state as a number value.
2. errno: error number.

### sndtimeo( fd[, sec] )

returns the SO_SNDTIMEO value of socket option.

**Parameters**

- fd: socket.
- sec: set value of send-timeout for socket if specified.

**Returns**

1. sec: current option state as a number value.
2. errno: error number.

