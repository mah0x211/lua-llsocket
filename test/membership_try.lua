local llsocket = require('llsocket');
local SOCK_STREAM = llsocket.SOCK_STREAM;
local SOCK_DGRAM = llsocket.SOCK_DGRAM;
local socket = llsocket.socket;
local getaddrinfoInet = llsocket.inet.getaddrinfo;
local getaddrinfoUnix = llsocket.unix.getaddrinfo;
local HOST = '127.0.0.1';
local PORT = '5000';
local SOCKFILE = './test.sock';

local function test_sup( addr )
    -- create socket
    local sock = ifNil( socket.new( addr ) );

    ifNotNil( sock:mcastjoin( '224.0.0.251' ) );
    ifNotNil( sock:mcastleave( '224.0.0.251' ) );
    ifNotNil( sock:mcastjoin( '224.0.0.251', 'lo0' ) );
    ifNotNil( sock:mcastleave( '224.0.0.251', 'lo0' ) );

    ifNotNil( sock:close() );
end


local function test_nosup( addr )
    -- create socket
    local sock = ifNil( socket.new( addr ) );

    ifNil( sock:mcastjoin( '224.0.0.251' ) );
    ifNil( sock:mcastleave( '224.0.0.251' ) );

    ifNotNil( sock:close() );
end

-- stream socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_STREAM ) );
for _, addr in ipairs( addrs ) do
    test_nosup( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_STREAM ) );
test_nosup( addrs );
os.remove( SOCKFILE );


-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_DGRAM ) );
for _, addr in ipairs( addrs ) do
    test_sup( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_DGRAM ) );
test_nosup( addrs );
os.remove( SOCKFILE );


