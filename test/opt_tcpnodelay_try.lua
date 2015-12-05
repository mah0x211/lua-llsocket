local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local inet = llsocket.inet;
local accept = llsocket.accept;
local tcpnodelay = llsocket.opt.tcpnodelay;
local HOST = '127.0.0.1';
local PORT = 8080;
local REUSE = true;


local function isolate( fn, ... )
    local vals = { pcall( fn, ... ) };
    local tail = 1;
    local idx, v;
    
    if vals[1] == false then
        vals[1] = nil;
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


local function connServer( stype )
    return ifFalse( isolate( inet.connect, HOST, PORT, stype, nil, REUSE ) );
end


local function openServer( stype )
    local fd = ifNil( isolate( inet.bind, HOST, PORT, stype, nil, REUSE ) );
    
    if stype == STREAM then
        ifNotNil( llsocket.listen( fd ) );
    -- dgram/raw socket cannot listen
    else
        ifNil( llsocket.listen( fd ) );
    end
    
    return fd;
end



local function trySendfile( stype )
    local fdSvr = openServer( stype );
    local fdCli = connServer( stype );

    if stype == STREAM then
        local fdPeer = ifNil( accept( fdSvr ) );
        -- tcpnodelay flag
        ifNotTrue( tcpnodelay( fdPeer, true ) );
        ifNotTrue( tcpnodelay( fdPeer ) );
        ifNotFalse( tcpnodelay( fdPeer, false ) );
        ifNotFalse( tcpnodelay( fdPeer ) );

        ifNotNil( llsocket.close( fdPeer ) );
    else
        ifNotNil( tcpnodelay( fdCli, true ) );
    end
    
    ifNotNil( llsocket.close( fdCli ) );
    ifNotNil( llsocket.close( fdSvr ) );
end

-- stream socket
trySendfile( STREAM );
-- dgram socket
trySendfile( DGRAM );


