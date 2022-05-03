local unpack = unpack or table.unpack
local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo

function testcase.new()
    local host = '127.0.0.1'
    local port = 8080
    local pathname = './test.sock'

    for _, ai in ipairs({
        assert(addrinfo.inet(host, port, llsocket.SOCK_STREAM,
                             llsocket.IPPROTO_TCP)),
        assert(addrinfo.unix(pathname, llsocket.SOCK_STREAM,
                             llsocket.IPPROTO_TCP)),
    }) do
        -- test that returns new instance of socket
        local s = assert(socket.new(ai:family(), ai:socktype()))
        assert.match(tostring(s), 'llsocket.socket: ')
        assert.is_false(s:nonblock())
        assert.equal(s:family(), ai:family())
        assert.equal(s:socktype(), ai:socktype())
        assert.equal(s:protocol(), 0)
        local sai = s:getsockname()
        assert.equal(sai:family(), ai:family())
        assert.equal(sai:socktype(), ai:socktype())
        assert.equal(sai:protocol(), 0)
        s:close()

        -- test that socket flag is nonblocking
        s = assert(socket.new(ai:family(), ai:socktype(), 0, true))
        assert.is_true(s:nonblock())
        s:close()

        -- luacheck: ignore err
        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    'foo',
                },
                err = '#1 .+ [(]integer expected, got string',
            },
            {
                arg = {
                    ai:family(),
                    true,
                },
                err = '#2 .+ [(]integer expected, got boolean',
            },
            {
                arg = {
                    ai:family(),
                    ai:socktype(),
                    {},
                },
                err = '#3 .+ [(]integer expected, got table',
            },
            {
                arg = {
                    ai:family(),
                    ai:socktype(),
                    ai:protocol(),
                    1,
                },
                err = '#4 .+ [(]boolean expected, got number',
            },
        }) do
            local err = assert.throws(function()
                socket.new(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end
    end
end

