local llsocket = require('llsocket');
local SOCK_STREAM = llsocket.SOCK_STREAM;
local SOCK_DGRAM = llsocket.SOCK_DGRAM;
local socket = llsocket.socket;
local getaddrinfoInet = llsocket.inet.getaddrinfo;
local getaddrinfoUnix = llsocket.unix.getaddrinfo;
local HOST = '127.0.0.1';
local SOCKFILE = './test.sock';


local function test_sup( addr )
    -- create socket
    local sock = ifNil( socket.new( addr ) );

    ifNotNil( sock:addmembership( '225.1.1.1' ) );
    ifNotNil( sock:dropmembership( '225.1.1.1' ) );
    ifNotNil( sock:addmembership( '225.1.1.1', '0.0.0.0' ) );
    ifNotNil( sock:dropmembership( '225.1.1.1', '0.0.0.0' ) );

    ifNotNil( sock:close() );
end


local function test_nosup( addr )
    -- create socket
    local sock = ifNil( socket.new( addr ) );

    ifNil( sock:addmembership( '225.1.1.1' ) );
    ifNil( sock:dropmembership( '225.1.1.1' ) );

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


-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, SOCK_DGRAM ) );
for _, addr in ipairs( addrs ) do
    test_sup( addr );
end
-- via unix domain
addrs = ifNil( getaddrinfoUnix( SOCKFILE, SOCK_DGRAM ) );
test_nosup( addrs );


