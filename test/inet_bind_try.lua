local strerror = require('process').strerror;
local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local inet = llsocket.inet;
local HOST = '127.0.0.1';
local PORT = 8080;
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


local fd;

-- invalid host
ifNotFalse( isolate( inet.bind, 1 ) );
ifNotFalse( isolate( inet.bind, true ) );
ifNotFalse( isolate( inet.bind, 'hello' ) );
ifNotFalse( isolate( inet.bind, HOST ) );
-- invalid port
ifNotFalse( isolate( inet.bind, HOST, 1 ) );
ifNotFalse( isolate( inet.bind, HOST, true ) );
ifNotFalse( isolate( inet.bind, HOST, '8080' ) );
-- too few arguments
ifNotFalse( isolate( inet.bind ) );
ifNotFalse( isolate( inet.bind, HOST, PORT ) );
-- invalid nonblock
ifNotFalse( isolate( inet.bind, HOST, PORT, STREAM, 1 ) );
ifNotFalse( isolate( inet.bind, HOST, PORT, STREAM, 'true' ) );
-- invalid reuseaddr
ifNotFalse( isolate( inet.bind, HOST, PORT, STREAM, nil, 1 ) );
ifNotFalse( isolate( inet.bind, HOST, PORT, STREAM, nil, 'true' ) );


-- stream socket
fd = ifFalse( isolate( inet.bind, HOST, PORT, STREAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o port
fd = ifNil( isolate( inet.bind, HOST, nil, STREAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o host
fd = ifNil( isolate( inet.bind, nil, PORT, STREAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w nonblock
fd = ifNil( isolate( inet.bind, HOST, nil, STREAM, NONBLOCK, REUSE ) );
ifNotTrue( llsocket.opt.nonblock( fd ) );
ifNotNil( llsocket.close( fd ) );


-- dgram socket
fd = ifNil( inet.bind( HOST, PORT, DGRAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o port
fd = ifNil( inet.bind( HOST, nil, DGRAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o host
fd = ifNil( inet.bind( nil, PORT, DGRAM, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w nonblock
fd = ifNil( inet.bind( HOST, nil, DGRAM, NONBLOCK, REUSE ) );
ifNotTrue( llsocket.opt.nonblock( fd ) );
ifNotNil( llsocket.close( fd ) );


-- raw socket
--[[
fd = ifNil( inet.bind( HOST, PORT, RAW, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o port
fd = ifNil( inet.bind( HOST, nil, RAW, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w/o host
fd = ifNil( inet.bind( nil, PORT, RAW, nil, REUSE ) );
ifNotNil( llsocket.close( fd ) );
-- w nonblock
fd = ifNil( inet.bind( HOST, nil, RAW, NONBLOCK, REUSE ) );
ifNotTrue( llsocket.opt.nonblock( fd ) );
ifNotNil( llsocket.close( fd ) );
--]]
