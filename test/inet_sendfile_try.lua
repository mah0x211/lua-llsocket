local stat = require('path').stat;
local strerror = require('process').strerror;
local llsocket = require('llsocket');
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local inet = llsocket.inet;
local sendfile = llsocket.sendfile;
local accept = llsocket.accept;
local recv = llsocket.recv;
local nonblock = llsocket.opt.nonblock;
local HOST = '127.0.0.1';
local PORT = 8080;
local REUSE = true;
local imgSmall = ifNil( stat( './small.png', false, true ) );
local imgLarge = ifNil( stat( './large.png', false, true ) );


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


local function sendfileNonBlocking( fdCli, fdRcv )
    local outByte = 0;
    local offset = 0;
    local fd = imgLarge.fd;
    local totalByte = imgLarge.size;
    local recvByte = 0;
    local data = {};
    local pkt, err, eagain;
    
    -- set nonblock flag
    ifNotTrue( nonblock( fdCli, true ) );
    ifNotTrue( nonblock( fdRcv, true ) );
    
    repeat
        -- sendfile
        if offset < totalByte then
            outByte, err, eagain = sendfile( fdCli, fd, totalByte - offset, offset );
            ifTrue( eagain ~= true and err ~= nil, err );
            -- update offset
            offset = offset + outByte;
            ifTrue( offset > totalByte, 'invalid implementation' );
        end
        -- recv
        pkt, err, eagain = recv( fdRcv, outByte == 0 and 1024 or outByte );
        ifTrue( eagain ~= true and err ~= nil, err );
        if not eagain then
            recvByte = recvByte + #pkt;
            data[#data+1] = pkt;
        end
    until recvByte == totalByte;
    
    data = table.concat( data );
    ifNotTrue(
        data == assert( io.open( './large.png' ) ):read('*a'),
        'invalid implementation'
    );
end


local function sendfileBlocking( fdCli, fdRcv )
    local outByte, err, eagain = sendfile( fdCli, imgSmall.fd, imgSmall.size, 0 );
    
    bytes, err, eagain = ifNil( recv( fdRcv, outByte ) );
    ifNotEqual( #bytes, outByte );
end


local function trySendfile( stype )
    local fdSvr = openServer( stype );
    local fdCli = connServer( stype );
    local fdRcv = fdSvr;
    local outByte = 0;
    local bytes, err, eagain;
    
    -- a sendfile API can be only use with a stream socket
    if stype == STREAM then
        fdRcv = ifNil( accept( fdSvr ) );
        sendfileBlocking( fdCli, fdRcv );
        sendfileNonBlocking( fdCli, fdRcv );
    else
        outByte, err, eagain = sendfile( fdCli, imgSmall.fd, outByte, imgSmall.size - outByte );
        ifTrue( outByte ~= 0, 'incorrect implementation' );
    end
    
    if fdRcv ~= fdSvr then
        ifNotNil( llsocket.close( fdRcv ) );
    end
    ifNotNil( llsocket.close( fdCli ) );
    ifNotNil( llsocket.close( fdSvr ) );
end

-- stream socket
trySendfile( STREAM );
-- dgram socket
trySendfile( DGRAM );
-- raw socket
trySendfile( RAW );


