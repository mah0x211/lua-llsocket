local llsocket = require('llsocket');
local SOCK_STREAM = llsocket.SOCK_STREAM;
local SOCK_DGRAM = llsocket.SOCK_DGRAM;
local SOCK_SEQPACKET = llsocket.SOCK_SEQPACKET;
local SOCK_RAW = llsocket.SOCK_RAW;
local socket = llsocket.socket;
local getaddrinfoInet = llsocket.inet.getaddrinfo;
local getaddrinfoUnix = llsocket.unix.getaddrinfo;
local HOST = '127.0.0.1';
local PORT = '8080';
local SOCKFILE = './test.sock';
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

local func = function()end
local co = coroutine.create(func);
local addrs;

local function test( addr )
    local sock;

    -- invalid first argument
    ifNotFalse( isolate( socket.new ) );
    ifNotFalse( isolate( socket.new, true ) );
    ifNotFalse( isolate( socket.new, 1 ) );
    ifNotFalse( isolate( socket.new, 'str' ) );
    ifNotFalse( isolate( socket.new, {} ) );
    ifNotFalse( isolate( socket.new, func ) );
    ifNotFalse( isolate( socket.new, co ) );
    -- invalid second argument
    ifNotFalse( isolate( socket.new, addr, 1 ) );
    ifNotFalse( isolate( socket.new, addr, 'str' ) );
    ifNotFalse( isolate( socket.new, addr, {} ) );
    ifNotFalse( isolate( socket.new, addr, func ) );
    ifNotFalse( isolate( socket.new, addr, co ) );
    -- create socket
    sock = ifNil( socket.new( addr ) );
    ifNotNil( sock:close() );
    -- w nonblock
    sock = ifNil( socket.new( addr, NONBLOCK ) );
    ifNotNil( sock:close() );
end

-- stream socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_STREAM ) );
for _, addr in ipairs( addrs ) do
    test( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_STREAM ) );
test( addrs );

-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_DGRAM ) );
for _, addr in ipairs( addrs ) do
    test( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_DGRAM ) );
test( addrs );


