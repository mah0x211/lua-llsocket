local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local unix = llsocket.unix;
local recv = llsocket.recv;
local send = llsocket.send;
local NONBLOCK = true;

local function isolate( fn, ... )
    local vals = { pcall( fn, ... ) };
    local tail = 1;
    local idx, v;
    
    if vals[1] == false then
        return unpack( vals );
    end
    
    table.remove( vals, 1 );
    -- find last index
    idx, v = next( vals );
    while idx do
        tail = idx;
        idx, v = next( vals, idx );
    end
    
    return unpack( vals, 1, tail );
end


local fds, len, data;

-- too few arguments
ifNotFalse( isolate( unix.socketpair ) );
-- invalid socktype
ifNotFalse( isolate( unix.socketpair, 'str' ) );
ifNotFalse( isolate( unix.socketpair, true ) );
-- invalid nonblock
ifNotFalse( isolate( unix.socketpair, STREAM, 1 ) );
ifNotFalse( isolate( unix.socketpair, STREAM, 'true' ) );
-- invalid protocol
ifNotFalse( isolate( unix.socketpair, STREAM, nil, 'str' ) );
ifNotFalse( isolate( unix.socketpair, STREAM, nil, true ) );


-- stream socket
fds = { unix.socketpair( STREAM ) };
ifNil( unpack( fds, 2 ) );
-- send
len = ifNil( send( fds[1], 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( send( fds[2], 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( recv( fds[1] ) );
ifNotEqual( data, 'hello' );
data = ifNil( recv( fds[2] ) );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( llsocket.close( fds[1] ) );
ifNotNil( llsocket.close( fds[2] ) );

-- w nonblock
fds = { unix.socketpair( STREAM, NONBLOCK ) };
ifNil( unpack( fds, 2 ) );
-- check opt
ifNotTrue( llsocket.opt.nonblock( fds[1] ) );
ifNotTrue( llsocket.opt.nonblock( fds[2] ) );
-- send
len = ifNil( send( fds[1], 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( send( fds[2], 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( recv( fds[1] ) );
ifNotEqual( data, 'hello' );
data = ifNil( recv( fds[2] ) );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( llsocket.close( fds[1] ) );
ifNotNil( llsocket.close( fds[2] ) );


-- dgram socket
fds = { unix.socketpair( DGRAM ) };
ifNil( unpack( fds, 2 ) );
-- send
len = ifNil( send( fds[1], 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( send( fds[2], 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( recv( fds[1] ) );
ifNotEqual( data, 'hello' );
data = ifNil( recv( fds[2] ) );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( llsocket.close( fds[1] ) );
ifNotNil( llsocket.close( fds[2] ) );

-- w nonblock
fds = { unix.socketpair( DGRAM, NONBLOCK ) };
ifNil( unpack( fds, 2 ) );
-- check opt
ifNotTrue( llsocket.opt.nonblock( fds[1] ) );
ifNotTrue( llsocket.opt.nonblock( fds[2] ) );
-- send
len = ifNil( send( fds[1], 'hello' ) );
ifNotEqual( len, 5 );
len = ifNil( send( fds[2], 'hello' ) );
ifNotEqual( len, 5 );
-- recv
data = ifNil( recv( fds[1] ) );
ifNotEqual( data, 'hello' );
data = ifNil( recv( fds[2] ) );
ifNotEqual( data, 'hello' );
-- close
ifNotNil( llsocket.close( fds[1] ) );
ifNotNil( llsocket.close( fds[2] ) );

