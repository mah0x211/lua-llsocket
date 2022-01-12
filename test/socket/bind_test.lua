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
        local sock = assert(socket.new(ai:family(), ai:socktype()))
        local _, err = sock:reuseaddr(true)
        assert(not err, err)

        -- test that bind socket
        assert(sock:bind(ai))

        -- test that cannot bind twice
        _, err = sock:bind(ai)
        assert(err, 'bind() did not returns an error')

        sock:close()
    end
end

function testcase.unix()
    for _, socktype in ipairs({
        llsocket.SOCK_STREAM,
        llsocket.SOCK_DGRAM,
    }) do
        local file = './test.sock'
        os.remove(file)

        local ai = assert(addrinfo.unix(file, socktype))
        local sock = assert(socket.new(ai:family(), ai:socktype()))
        local _, err = sock:reuseaddr(true)
        assert(not err, err)

        -- test that bind socket
        assert(sock:bind(ai))

        -- test that cannot bind twice
        _, err = sock:bind(ai)
        assert(err, 'bind() did not returns an error')

        sock:close()
        os.remove(file)
    end
end

