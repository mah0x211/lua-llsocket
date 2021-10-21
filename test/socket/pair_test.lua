local unpack = unpack or table.unpack
local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.stream_pair()
    -- test that returns socket pair
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    assert.equal(#sp, 2)
    assert.match(tostring(sp[1]), 'llsocket.socket: ')
    assert.match(tostring(sp[2]), 'llsocket.socket: ')
    for _, v in ipairs(sp) do
        assert.is_false(v:nonblock())
        v:close()
    end

    -- test that socket flag is nonblocking
    sp = assert(socket.pair(llsocket.SOCK_STREAM, true))
    for _, v in ipairs(sp) do
        assert.is_true(v:nonblock())
    end

    -- luacheck: ignore err
    -- test that sockets are connected to each other
    local smsg = 'hello'
    local n, err = sp[1]:send(smsg)
    assert(not err, err)
    assert.equal(n, #smsg)
    local rmsg, err = sp[2]:recv()
    assert(not err, err)
    assert.equal(rmsg, smsg)

    -- test that throws an error with invalid arguments
    for _, v in ipairs({
        {
            arg = {
                true,
            },
            err = '#1 .+ [(]number expected, got boolean',
        },
        {
            arg = {
                1,
                {},
            },
            err = '#2 .+ [(]boolean expected, got table',
        },
        {
            arg = {
                1,
                false,
                true,

            },
            err = '#3 .+ [(]number expected, got boolean',
        },
    }) do
        err = assert.throws(function()
            socket.pair(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end
end

