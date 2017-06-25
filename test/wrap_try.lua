local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local NONBLOCK = true;

-- stream socket
function socketpair( ... )
    local fds = ifNil( llsocket.socket.pair( ... ) );

    fds[1] = llsocket.socket.wrap( ifNil( fds[1]:unwrap() ) );
    fds[2] = llsocket.socket.wrap( ifNil( fds[2]:unwrap() ) );

    return fds;
end

local fds, len, data;


-- stream socket
fds = ifNil( socketpair( STREAM ) );
-- send
len = ifNil( fds[1]:send( 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( fds[2]:send( 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( fds[1]:recv() );
ifNotEqual( data, 'hello' );
data = ifNil( fds[2]:recv() );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( fds[1]:close() );
ifNotNil( fds[2]:close() );

-- w nonblock
fds = ifNil( socketpair( STREAM, NONBLOCK ) );
-- check opt
ifNotTrue( fds[1]:nonblock() );
ifNotTrue( fds[2]:nonblock() );
-- send
len = ifNil( fds[1]:send( 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( fds[2]:send( 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( fds[1]:recv() );
ifNotEqual( data, 'hello' );
data = ifNil( fds[2]:recv() );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( fds[1]:close() );
ifNotNil( fds[2]:close() );


-- dgram socket
fds = ifNil( socketpair( DGRAM ) );
-- send
len = ifNil( fds[1]:send( 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( fds[2]:send( 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( fds[1]:recv() );
ifNotEqual( data, 'hello' );
data = ifNil( fds[2]:recv() );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( fds[1]:close() );
ifNotNil( fds[2]:close() );

-- w nonblock
fds = ifNil( socketpair( DGRAM, NONBLOCK ) );
-- check opt
ifNotTrue( fds[1]:nonblock() );
ifNotTrue( fds[2]:nonblock() );
-- send
len = ifNil( fds[1]:send( 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( fds[2]:send( 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( fds[1]:recv() );
ifNotEqual( data, 'hello' );
data = ifNil( fds[2]:recv() );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( fds[1]:close() );
ifNotNil( fds[2]:close() );

