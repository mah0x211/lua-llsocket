local llsocket = require('llsocket')
local testcase = require('testcase')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo

function testcase.inet()
    for _, socktype in ipairs({
        llsocket.SOCK_STREAM,
        llsocket.SOCK_DGRAM,
    }) do
        local ai = assert(addrinfo.inet('127.0.0.1', 8080, socktype))
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local _, err = s:reuseaddr(true)
        assert(not err, err)
        assert(s:bind(ai))
        if socktype == llsocket.SOCK_STREAM then
            assert(s:listen())
        end

        -- test that connect to server
        local c = assert(socket.new(ai:family(), ai:socktype()))
        assert(c:connect(ai))

        s:close()
        c:close()
    end
end

function testcase.unix()
    for _, socktype in ipairs({
        llsocket.SOCK_STREAM,
        llsocket.SOCK_DGRAM,
    }) do
        local filename = './test.sock'
        os.remove(filename)

        local ai = assert(addrinfo.unix(filename, socktype))
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local _, err = s:reuseaddr(true)
        assert(not err, err)
        assert(s:bind(ai))
        if socktype == llsocket.SOCK_STREAM then
            assert(s:listen())
        end

        -- test that connect to server
        local c = assert(socket.new(ai:family(), ai:socktype()))
        assert(c:connect(ai))

        s:close()
        c:close()
        os.remove(filename)
    end
end

