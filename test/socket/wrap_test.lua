local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.wrap()

    -- test that fd is wrapped in a new socket instance
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    for _, v in ipairs(sp) do
        local fd = v:fd()
        local s = socket.wrap(v:unwrap())
        assert.equal(fd, s:fd())
        local enabled, err = s:nonblock()
        assert(not err, err)
        assert.is_false(enabled)
        assert.match(tostring(s), 'llsocket.socket:', false)
        s:close()
    end

    -- test that fd is wrapped in a new socket instance with nonblock flag
    sp = assert(socket.pair(llsocket.SOCK_STREAM))
    for _, v in ipairs(sp) do
        local fd = v:fd()
        local s = socket.wrap(v:unwrap(), true)
        assert.equal(fd, s:fd())
        local enabled, err = s:nonblock()
        assert(not err, err)
        assert.is_true(enabled)
        assert.match(tostring(s), 'llsocket.socket:', false)
        s:close()
    end
end

