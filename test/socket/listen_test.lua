local llsocket = require('llsocket')
local testcase = require('testcase')
local errno = require('errno')
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

        -- test that throws an error with invalid argument
        err = assert.throws(function()
            s:listen('foo')
        end)
        assert.match(err, '#1 .+ [(]integer expected, got string', false)

        if socktype == llsocket.SOCK_STREAM then
            -- test that the stream socket can listen
            assert(s:bind(ai))
            assert(s:listen())

            -- confirm that connect to server
            local c = assert(socket.new(ai:family(), ai:socktype()))
            assert(c:connect(ai))

            c:close()
        else
            -- test that the dgram socket cannot listen
            _, err = s:listen()
            assert.equal(err.type, errno.EOPNOTSUPP)
        end

        s:close()
    end

    -- test that throws an error with invalid arguments
    local ai = assert(addrinfo.inet('127.0.0.1', 8080, llsocket.SOCK_STREAM))
    local s = assert(socket.new(ai:family(), ai:socktype()))
    local _, err = s:reuseaddr(true)
    assert(not err, err)
    err = assert.throws(function()
        s:listen('foo')
    end)
    assert.match(err, '#1 .+ [(]integer expected, got string', false)

    s:close()
end

function testcase.unix()
    local filename = './test.sock'
    os.remove(filename)

    for _, socktype in ipairs({
        llsocket.SOCK_STREAM,
        llsocket.SOCK_DGRAM,
    }) do

        local ai = assert(addrinfo.unix(filename, socktype))
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local _, err = s:reuseaddr(true)
        assert(not err, err)

        if socktype == llsocket.SOCK_STREAM then
            -- test that the stream socket can listen
            assert(s:bind(ai))
            assert(s:listen())
            -- confirm that can connect to server
            local c = assert(socket.new(ai:family(), ai:socktype()))
            assert(c:connect(ai))
            c:close()
        else
            -- test that the dgram socket cannot listen
            _, err = s:listen()
            assert.equal(err.type, errno.EOPNOTSUPP)
        end

        s:close()
        os.remove(filename)
    end

    -- test that throws an error with invalid arguments
    local ai = assert(addrinfo.unix(filename, llsocket.SOCK_STREAM))
    local s = assert(socket.new(ai:family(), ai:socktype()))
    local _, err = s:reuseaddr(true)
    assert(not err, err)
    err = assert.throws(function()
        s:listen('foo')
    end)
    assert.match(err, '#1 .+ [(]integer expected, got string', false)
    s:close()
    os.remove(filename)
end

