local assert = require('assertex')
local testcase = require('testcase')
local trim_prefix = require('stringex').trim_prefix
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.unwrap()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that returns of fd() and unwrap() are the same
    for _, v in ipairs(sp) do
        local fd = v:fd()
        local ptraddr = trim_prefix(tostring(v), 'llsocket.socket: ')
        assert.equal(v:unwrap(), fd)
        -- test that the metatable has been removed
        assert.match(tostring(v), ptraddr .. '$', false)
    end
end

