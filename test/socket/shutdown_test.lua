local unpack = unpack or table.unpack
local testcase = require('testcase')
local errno = require('errno')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.shutdown_with_valid_arguments()
    local flgs = {
        nil,
        llsocket.SHUT_RD,
        llsocket.SHUT_WR,
        llsocket.SHUT_RDWR,
    }

    -- test that shutdown a file descriptor
    for i = 1, 4 do
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local flg = flgs[i]

        assert(socket.shutdown(sp[1]:fd(), flg))

        -- nil == SHUT_RDWR
        if flg == nil or flg == llsocket.SHUT_RDWR then
            -- test that return EPIPE error
            local n, err = sp[1]:send('foo')
            assert.is_nil(n)
            assert.equal(err.type, errno.EPIPE)

            -- test that recv does not return values
            assert.empty({
                sp[1]:recv(),
            })

            -- test that peer:recv does not return values
            assert.empty({
                sp[2]:recv(),
            })
        elseif flg == llsocket.SHUT_RD then
            -- test that socket can send message
            local n = assert(sp[1]:send('foo'))
            assert.equal(n, 3)

            -- test that recv does not return values
            assert.empty({
                sp[1]:recv(),
            })

            -- test that peer:recv returns message
            assert.equal(sp[2]:recv(), 'foo')
        elseif flg == llsocket.SHUT_WR then
            assert(sp[2]:send('foo'))

            -- test that return EPIPE error
            local n, err = sp[1]:send('foo')
            assert.is_nil(n)
            assert.equal(err.type, errno.EPIPE)

            -- test that recv does not return values
            assert.equal(sp[1]:recv(), 'foo')

            -- test that peer:recv does not return values
            assert.empty({
                sp[2]:recv(),
            })
        end

        for _, s in ipairs(sp) do
            s:close()
        end
    end

    -- test that method shutdown a internal file descriptor
    for i = 1, 4 do
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local flg = flgs[i]

        assert(sp[1]:shutdown(flg))

        -- nil == SHUT_RDWR
        if flg == nil or flg == llsocket.SHUT_RDWR then
            -- test that return EPIPE error
            local n, err = sp[1]:send('foo')
            assert.is_nil(n)
            assert.equal(err.type, errno.EPIPE)

            -- test that recv does not return values
            assert.empty({
                sp[1]:recv(),
            })

            -- test that peer:recv does not return values
            assert.empty({
                sp[2]:recv(),
            })
        elseif flg == llsocket.SHUT_RD then
            -- test that socket can send message
            local n = assert(sp[1]:send('foo'))
            assert.equal(n, 3)

            -- test that recv does not return values
            assert.empty({
                sp[1]:recv(),
            })

            -- test that peer:recv returns message
            assert.equal(sp[2]:recv(), 'foo')
        elseif flg == llsocket.SHUT_WR then
            assert(sp[2]:send('foo'))

            -- test that return EPIPE error
            local n, err = sp[1]:send('foo')
            assert.is_nil(n)
            assert.equal(err.type, errno.EPIPE)

            -- test that recv does not return values
            assert.equal(sp[1]:recv(), 'foo')

            -- test that peer:recv does not return values
            assert.empty({
                sp[2]:recv(),
            })
        end

        for _, s in ipairs(sp) do
            s:close()
        end
    end
end

function testcase.shutdown_with_invalid_arguments()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that throw an error with invalid argument
    for _, v in ipairs({
        {
            arg = {
                true,
            },
            err = '#1 .+ [(]integer expected, got boolean',
        },
        {
            arg = {
                sp[1]:fd(),
                'foo',
            },
            err = '#2 .+ [(]integer expected, got string',
        },
    }) do
        local err = assert.throws(function()
            socket.shutdown(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end

    -- test that method throw an error with invalid argument
    local err = assert.throws(function()
        sp[1]:shutdown('foo')
    end)
    assert.match(err, '#1 .+ [(]integer expected, got string', false)

    for _, s in ipairs(sp) do
        s:close()
    end
end

