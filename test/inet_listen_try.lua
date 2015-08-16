local strerror = require('process').strerror;
local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local inet = llsocket.inet;
local HOST = '127.0.0.1';
local PORT = 8080;
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


local fd;

-- stream socket
fd = ifNil( isolate( inet.bind, HOST, PORT, STREAM, nil, REUSE ) );
-- invalid arguments
ifNil( isolate( llsocket.listen ) );
ifNil( isolate( llsocket.listen, fd, 'test' ) );
ifNil( isolate( llsocket.listen, fd, true ) );
ifNil( isolate( llsocket.listen, fd, 9223372036854775808 ) );
ifNotNil( llsocket.listen( fd ) );
ifNotNil( llsocket.close( fd ) );


-- dgram socket
fd = ifNil( inet.bind( HOST, PORT, DGRAM, nil, REUSE ) );
-- dgram socket cannot listen
ifNil( llsocket.listen( fd ) );
ifNotNil( llsocket.close( fd ) );


-- raw socket
fd = ifNil( inet.bind( HOST, PORT, RAW, nil, REUSE ) );
-- raw socket cannot listen
ifNil( llsocket.listen( fd ) );
ifNotNil( llsocket.close( fd ) );
