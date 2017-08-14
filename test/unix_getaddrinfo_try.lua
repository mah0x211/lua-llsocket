local llsocket = require('llsocket');
local SOCK_STREAM = llsocket.SOCK_STREAM;
local SOCK_DGRAM = llsocket.SOCK_DGRAM;
local SOCK_SEQPACKET = llsocket.SOCK_SEQPACKET;
local SOCK_RAW = llsocket.SOCK_RAW;
local IPPROTO_TCP = llsocket.IPPROTO_TCP;
local IPPROTO_UDP = llsocket.IPPROTO_UDP;
local AI_PASSIVE = llsocket.AI_PASSIVE;
local getaddrinfo = llsocket.unix.getaddrinfo;
local SOCKFILE = './test.sock';

local fnval = function()end;
local coval = coroutine.create(fnval);

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


-- invalid sunpath
ifNotFalse( isolate( getaddrinfo ) );
ifNotFalse( isolate( getaddrinfo, 1 ) );
ifNotFalse( isolate( getaddrinfo, true ) );
ifNotFalse( isolate( getaddrinfo, fnval ) );
ifNotFalse( isolate( getaddrinfo, coval ) );
-- invalid socktype
ifNotFalse( isolate( getaddrinfo, SOCKFILE, '1' ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, true ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, fnval ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, coval ) );
-- invalid protocol
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, '1' ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, true ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, fnval ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, coval ) );
-- invalid flags
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, '1' ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, true ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, fnval ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, coval ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, '1' ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, true ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, fnval ) );
ifNotFalse( isolate( getaddrinfo, SOCKFILE, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, coval ) );


-- stream addrinfo
ifNil( getaddrinfo( SOCKFILE, SOCK_STREAM ) );
-- w protocol
ifNil( getaddrinfo( SOCKFILE, SOCK_STREAM, IPPROTO_TCP ) );
-- w flag
ifNil( getaddrinfo( SOCKFILE, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE ) );

-- dgram socket
ifNil( getaddrinfo( SOCKFILE, SOCK_DGRAM ) );
-- w protocol
ifNil( getaddrinfo( SOCKFILE, SOCK_DGRAM, IPPROTO_UDP ) );
-- w flag
ifNil( getaddrinfo( SOCKFILE, SOCK_DGRAM, IPPROTO_UDP, AI_PASSIVE ) );
