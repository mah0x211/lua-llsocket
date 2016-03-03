local stat = require('path').stat;
local strerror = require('process').strerror;
local llsocket = require('llsocket');
local socket = llsocket.socket;
local STREAM = llsocket.SOCK_STREAM;
local DGRAM = llsocket.SOCK_DGRAM;
local SEQPACKET = llsocket.SOCK_SEQPACKET;
local RAW = llsocket.SOCK_RAW;
local getaddrinfoInet = llsocket.inet.getaddrinfo;
local HOST = '127.0.0.1';
local PORT = '8080';
local REUSE = true;
local imgSmall = ifNil( stat( './small.png', false, true ) );
local imgLarge = ifNil( stat( './large.png', false, true ) );


local function connServer( stype, addr )
    local sock = ifNil( socket.new( addr ) );

    ifNotNil( sock:connect() );

    return sock;
end


local function openServer( stype, addr )
    local sock = ifNil( socket.new( addr ) );

    ifNotTrue( sock:reuseaddr( true ) );
    ifNotNil( sock:bind() );

    if stype == STREAM then
        ifNotNil( sock:listen() );
    end
    
    return sock;
end


local function sendfileNonBlocking( cli, rcv )
    local outByte = 0;
    local offset = 0;
    local fd = imgLarge.fd;
    local totalByte = imgLarge.size;
    local recvByte = 0;
    local data = {};
    local pkt, err, eagain;
    
    -- set nonblock flag
    ifNotTrue( cli:nonblock( true ) );
    ifNotTrue( rcv:nonblock( true ) );
    
    repeat
        -- sendfile
        if offset < totalByte then
            outByte, err, eagain = cli:sendfile( fd, totalByte - offset, offset );
            ifTrue( eagain ~= true and err ~= nil, err );
            -- update offset
            offset = offset + outByte;
            ifTrue( offset > totalByte, 'invalid implementation' );
        end
        -- recv
        pkt, err, eagain = rcv:recv( outByte == 0 and 1024 or outByte );
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


local function sendfileBlocking( cli, rcv )
    local outByte = ifNil( cli:sendfile( imgSmall.fd, imgSmall.size, 0 ) );

    ifNotEqual( outByte, imgSmall.size );
    bytes = ifNil( rcv:recv( outByte ) );
    ifNotEqual( #bytes, outByte );
end


local function trySendfile( stype, addr )
    local svr = openServer( stype, addr );
    local cli = connServer( stype, addr );
    local rcv = svr;
    local outByte = 0;
    local bytes, err, eagain;
    
    -- a sendfile API can be only use with a stream socket
    if stype == STREAM then
        rcv = ifNil( svr:accept() );
        sendfileBlocking( cli, rcv );
        sendfileNonBlocking( cli, rcv );
    else
        outByte, err, eagain = cli:sendfile( imgSmall.fd, outByte, imgSmall.size - outByte );
        ifNotNil( outByte, 'incorrect implementation' );
        ifNotNil( eagain, 'incorrect implementation' );
        ifNil( err, 'incorrect implementation' );
    end
    
    if rcv ~= svr then
        ifNotNil( rcv:close() );
    end
    ifNotNil( cli:close() );
    ifNotNil( svr:close() );
end

-- stream socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, STREAM ) );
for _, addr in ipairs( addrs ) do
    trySendfile( STREAM, addr );
    break;
end
-- dgram socket
addrs = ifNil( getaddrinfoInet( HOST, PORT, DGRAM ) );
for _, addr in ipairs( addrs ) do
    trySendfile( DGRAM, addr );
    break;
end
-- raw socket
--trySendfile( RAW );


