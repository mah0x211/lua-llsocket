local llsocket = require('llsocket');
local SOCK_STREAM = llsocket.SOCK_STREAM;
local SOCK_DGRAM = llsocket.SOCK_DGRAM;
local socket = llsocket.socket;
local getaddrinfoInet = llsocket.inet.getaddrinfo;
local getaddrinfoUnix = llsocket.unix.getaddrinfo;
local HOST = '127.0.0.1';
local SOCKFILE = './test.sock';


local function test( addr )
    -- create socket
    local sock = ifNil( socket.new( addr ) );

    ifNotTrue( sock:dontroute( true ) );
    ifNotFalse( sock:dontroute( false ) );

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
os.remove( SOCKFILE );


-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_DGRAM ) );
for _, addr in ipairs( addrs ) do
    test( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_DGRAM ) );
test( addrs );
os.remove( SOCKFILE );



