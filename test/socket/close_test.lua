require('nosigpipe')
local unpack = unpack or table.unpack
local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.close_with_valid_arguments()
    local flgs = {
        nil,
        llsocket.SHUT_RD,
        llsocket.SHUT_WR,
        llsocket.SHUT_RDWR,
    }

    for i = 1, 4 do
        -- test that close a file descriptor
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local flg = flgs[i]
        assert(socket.close(sp[1]:fd(), flg))

        -- test that returns error
        local _, err = sp[1]:close()
        assert(err, 'close() did not return an error')

        for _, s in ipairs(sp) do
            s:close()
        end
    end

    for i = 1, 4 do
        -- test that method close a internal file descriptor
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local flg = flgs[i]
        assert(sp[1]:close(flg))
        for _, s in ipairs(sp) do
            s:close()
        end
    end
end

function testcase.close_with_invalid_arguments()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that throw an error with invalid argument
    for _, v in ipairs({
        {
            arg = {
                true,
            },
            err = '#1 .+ [(]number expected, got boolean',
        },
        {
            arg = {
                sp[1]:fd(),
                'foo',
            },
            err = '#2 .+ [(]number expected, got string',
        },
    }) do
        local err = assert.throws(function()
            socket.close(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end

    -- test that method throw an error with invalid argument
    local err = assert.throws(function()
        sp[1]:close('foo')
    end)
    assert.match(err, '#1 .+ [(]number expected, got string', false)

    -- test that return an error with invalid argument
    local _, err = sp[1]:close(-1)
    assert.match(tostring(err), 'Invalid argument', false)
    assert.equal(sp[1]:fd(), -1)

    for _, s in ipairs(sp) do
        s:close()
    end
end

function testcase.call_close_twice()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that return an error if calls twice
    assert(socket.close(sp[1]:fd()))
    local _, err = socket.close(sp[1]:fd(), 123)
    assert.match(tostring(err), 'Bad file', false)

    -- test that return an error if calls twice
    assert(sp[2]:close())
    _, err = sp[2]:close()
    assert(not err, 'close method return an error')

    for _, s in ipairs(sp) do
        s:close()
    end
end

