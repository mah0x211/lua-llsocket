local collectgarbage = collectgarbage
local unpack = unpack or table.unpack
local testcase = require('testcase')
local timer = require('testcase.timer')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.after_all()
    collectgarbage('restart')
end

local function time2ms(tv, unit)
    local ms = 1
    local us = ms / 1000
    local ns = us / 1000
    local sec = ms * 1000
    local min = sec * 60
    local tbl = {
        ns = ns,
        us = us,
        ms = 1,
        s = sec,
        m = min,
    }
    return tv * tbl[unit]
end

function testcase.sendable()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local s = sp[1]

    -- test that returns ok=true
    local ok, err, timeout = s:sendable()
    assert.is_true(ok)
    assert.is_nil(err)
    assert.is_nil(timeout)

    -- test that returns ok=true even set timeout
    ok, err, timeout = s:sendable(.100)
    assert.is_true(ok)
    assert.is_nil(err)
    assert.is_nil(timeout)

    -- NOTE: depends on the OS scheduler
    -- test that returns timeout=true after 150 ms
    s:nonblock(true)
    while 1 do
        -- fill the buffer
        local _, again
        _, err, again = s:send('foo')
        assert(not err, err)
        if again then
            collectgarbage('collect')
            collectgarbage('stop')
            local t = timer.new()
            t:start()
            ok, err, timeout = s:sendable(.150)
            local tv, _, unit = t:stop()
            tv = time2ms(tv, unit)
            collectgarbage('restart')
            assert.is_false(ok)
            assert(not err, err)
            assert.is_true(timeout)
            assert.is_true(100 < tv and tv < 450)
            break
        end
    end

    -- test that throws error with invalid arguments
    for _, v in ipairs({
        {
            arg = {
                true,
            },
            err = '#1 .+ [(]number expected, got boolean',
        },
        {
            arg = {
                100,
                'foo',
            },
            err = '#2 .+ [(]boolean expected, got string',
        },
    }) do
        err = assert.throws(function()
            s:sendable(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end

    for _, v in ipairs(sp) do
        v:close()
    end
end

