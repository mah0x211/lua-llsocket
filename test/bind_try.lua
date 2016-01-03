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
local REUSE = true;

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


local function test( addr, file )
    local sock;
    -- create socket
    sock = ifNil( socket.new( addr ) );
    ifNotTrue( sock:reuseaddr( true ) );
    ifNotTrue( sock:bind() );
    ifNotNil( sock:close() );
    if file then
        os.remove( file );
    end
    -- w nonblock
    sock = ifNil( socket.new( addr, NONBLOCK ) );
    ifNotTrue( sock:reuseaddr( true ) );
    ifNotTrue( sock:bind() );
    ifNotNil( sock:close() );
    if file then
        os.remove( file );
    end
end

-- stream socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_STREAM ) );
for _, addr in ipairs( addrs ) do
    test( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_STREAM ) );
test( addrs, SOCKFILE );

-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_DGRAM ) );
for _, addr in ipairs( addrs ) do
    test( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_DGRAM ) );
test( addrs, SOCKFILE );

