local fork = require('process').fork;
local strerror = require('process').strerror;
local llsocket = require('llsocket');
local STREAM = llsocket.opt.SOCK_STREAM;
local DGRAM = llsocket.opt.SOCK_DGRAM;
local RAW = llsocket.opt.SOCK_RAW;
local MSG_WAITALL = llsocket.opt.MSG_WAITALL;
local MSG_PEEK = llsocket.opt.MSG_PEEK;
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


local function sendEcho( fd, msg )
    llsocket.send( fd, msg );
    msg = llsocket.recv( fd );
    return msg;
end


local function replyEcho( fd, sendfn )
    local msg, addr, err = llsocket.recvfrom( fd, 5, 
                                              -- wait 5 bytes
                                              MSG_WAITALL,
                                              -- do not delete msg from queue
                                              MSG_PEEK );
    
    if not err then
        -- read again
        msg, addr, err = llsocket.recvfrom( fd );
        if not err then
            sendfn( fd, msg, addr );
        end
    end
    
    if err then
        msg = strerror( err );
    end
    
    return msg;
end



local function runServer( host, port, socktype )
    local nwait = 0;
    local pid = fork();
    
    if pid == 0 then
        local s, c;
        
        -- create server socket
        s, err = inet.bind( host, port, socktype, nil, REUSE );
        if not err then
            if socktype == DGRAM then
                replyEcho( s, llsocket.sendto );
            else
                err = llsocket.listen( s );
                if not err then
                    -- accept client socket
                    c, err = llsocket.accept( s );
                    if not err then
                        replyEcho( c, llsocket.send );
                    end
                end
            end
            -- close server
            llsocket.close( s );
        end
        
        if err then
            print( strerror( err ) );
        end
        os.exit(0);
    -- failed to fork()
    elseif pid == -1 then
        return false;
    end
    
    -- wait until listen
    sleep(1);
    
    return true;
end


local fd, err, msg;

-- invalid host
ifNotFalse( isolate( inet.connect, 1 ) );
ifNotFalse( isolate( inet.connect, true ) );
ifNotFalse( isolate( inet.connect, 'hello' ) );
ifNotFalse( isolate( inet.connect, HOST ) );
-- invalid port
ifNotFalse( isolate( inet.connect, HOST, 1 ) );
ifNotFalse( isolate( inet.connect, HOST, true ) );
ifNotFalse( isolate( inet.connect, HOST, '8080' ) );
-- too few arguments
ifNotFalse( isolate( inet.connect ) );
ifNotFalse( isolate( inet.connect, HOST, PORT ) );
-- invalid nonblock
ifNotFalse( isolate( inet.connect, HOST, PORT, STREAM, 1 ) );
ifNotFalse( isolate( inet.connect, HOST, PORT, STREAM, 'true' ) );
-- invalid reuseaddr
ifNotFalse( isolate( inet.connect, HOST, PORT, STREAM, nil, 1 ) );
ifNotFalse( isolate( inet.connect, HOST, PORT, STREAM, nil, 'true' ) );

-- stream socket
runServer( HOST, PORT, STREAM );
fd = ifNil( isolate( inet.connect, HOST, PORT, STREAM, nil, REUSE ) );
ifNil( llsocket.peername( fd ) );
msg = sendEcho( fd, 'hello' );
ifNotNil( llsocket.close( fd ) );
ifNotEqual( msg, 'hello' );

-- dgram socket
sleep(1);
runServer( HOST, PORT, DGRAM );
fd = ifNil( isolate( inet.connect, HOST, PORT, DGRAM, nil, REUSE ) );
msg = sendEcho( fd, 'hello' );
ifNotNil( llsocket.close( fd ) );
ifNotEqual( msg, 'hello' );

