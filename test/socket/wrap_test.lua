local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.wrap()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that fd is wrapped in a new socket instance
    for _, v in ipairs(sp) do
        local fd = v:fd()
        local s = socket.wrap(v:unwrap())
        assert.equal(fd, s:fd())
        assert.match(tostring(s), 'llsocket.socket:', false)
        s:close()
    end
end

