local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local socketpair = require('llsocket').socket.pair;
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
ifNotFalse( isolate( socketpair ) );
-- invalid socktype
ifNotFalse( isolate( socketpair, 'str' ) );
ifNotFalse( isolate( socketpair, true ) );
-- invalid nonblock
ifNotFalse( isolate( socketpair, STREAM, 1 ) );
ifNotFalse( isolate( socketpair, STREAM, 'true' ) );
-- invalid protocol
ifNotFalse( isolate( socketpair, STREAM, nil, 'str' ) );
ifNotFalse( isolate( socketpair, STREAM, nil, true ) );


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

