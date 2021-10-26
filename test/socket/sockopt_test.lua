local collectgarbage = collectgarbage
local assert = require('assertex')
local testcase = require('testcase')
local timer = require('testcase.timer')
local llsocket = require('llsocket')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo
local addrs = {}

function testcase.before_all()
    local host = '127.0.0.1'
    local port = 8080
    local pathname = './test.sock'
    addrs.inet_stream = assert(addrinfo.inet(host, port, llsocket.SOCK_STREAM))
    addrs.inet_dgram = assert(addrinfo.inet(host, port, llsocket.SOCK_DGRAM))
    addrs.unix_stream = assert(addrinfo.unix(pathname, llsocket.SOCK_STREAM))
    addrs.unix_dgram = assert(addrinfo.unix(pathname, llsocket.SOCK_DGRAM))
end

function testcase.after_all()
    collectgarbage('restart')
    os.remove('./test.sock');
end

function testcase.error()
    for k, ai in pairs(addrs) do
        if k == 'inet_stream' then
            local s = assert(socket.new(ai:family(), ai:socktype()))
            local _, err = s:nonblock(true)
            assert(not err, err)

            -- test that returns an errno associated with socket
            local ok, err, again = s:connect(ai)
            assert(not ok, 'connect() returns ok=true')
            assert(not err, err)
            assert(again, 'connect() returns again=false')
            timer.usleep(1000)
            assert.greater(s:error(), 0)

            s:close()
        end
        os.remove('./test.sock');
    end
end

function testcase.acceptconn()
    -- the following platforms are does not support SO_ACCEPTCONN
    local envos = llsocket.env.os
    if envos == 'apple' or envos == 'openbsd' then
        return
    end

    for k, ai in pairs(addrs) do
        if k == 'inet_stream' or k == 'unix_stream' then
            local s = assert(socket.new(ai:family(), ai:socktype()))

            -- test that returns false before listen
            local ok, err = s:acceptconn()
            assert(not err, err)
            assert.is_false(ok)

            -- test that returns true after listen
            assert(s:bind(ai))
            assert(s:listen())
            ok, err = s:acceptconn()
            assert(not err, err)
            assert.is_true(ok)

            s:close()
            os.remove('./test.sock');
        end
    end
end

function testcase.tcpnodelay()
    for k, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        if k == 'inet_stream' then
            -- test that tcpnodelay socket option can be set
            local defval = s:tcpnodelay()
            assert.equal(s:tcpnodelay(true), defval)
            assert.is_true(s:tcpnodelay())
            assert.is_true(s:tcpnodelay(false))
            assert.is_false(s:tcpnodelay())
        else
            -- test that tcpnodelay socket option cannot be set
            local ok, err = s:tcpnodelay(true)
            assert(not ok, 'tcpnodelay() returns true')
            assert(err, 'tcpnodelay() did not return error')
        end

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.tcpkeepintvl()
    for k, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        if k == 'inet_stream' then
            -- test that tcpkeepintvl socket option can be set
            local defval = s:tcpkeepintvl()
            assert.equal(s:tcpkeepintvl(3), defval)
            assert.equal(s:tcpkeepintvl(), 3)
            assert.equal(s:tcpkeepintvl(1), 3)
            assert.equal(s:tcpkeepintvl(), 1)
        else
            -- test that tcpkeepintvl socket option cannot be set
            local ok, err = s:tcpkeepintvl(1)
            assert(not ok, 'tcpkeepintvl() returns true')
            assert(err, 'tcpkeepintvl() did not return error')
        end

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.tcpkeepcnt()
    for k, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        if k == 'inet_stream' then
            -- test that tcpkeepcnt socket option can be set
            local defval = s:tcpkeepcnt()
            assert.equal(s:tcpkeepcnt(3), defval)
            assert.equal(s:tcpkeepcnt(), 3)
            assert.equal(s:tcpkeepcnt(1), 3)
            assert.equal(s:tcpkeepcnt(), 1)
        else
            -- test that tcpkeepcnt socket option cannot be set
            local ok, err = s:tcpkeepcnt(9)
            assert(not ok, 'tcpkeepcnt() returns true')
            assert(err, 'tcpkeepcnt() did not return error')
        end

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.tcpkeepalive()
    for k, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        if k == 'inet_stream' then
            -- test that tcpkeepalive socket option can be set
            local defval = s:tcpkeepalive()
            assert.equal(s:tcpkeepalive(3), defval)
            assert.equal(s:tcpkeepalive(), 3)
            assert.equal(s:tcpkeepalive(1), 3)
            assert.equal(s:tcpkeepalive(), 1)
        else
            -- test that tcpkeepalive socket option cannot be set
            local ok, err = s:tcpkeepalive(9)
            assert(not ok, 'tcpkeepalive() returns true')
            assert(err, 'tcpkeepalive() did not return error')
        end

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.tcpcork()
    for k, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        if k == 'inet_stream' then
            -- test that tcpcork socket option can be set
            local defval = s:tcpcork()
            assert.equal(s:tcpcork(true), defval)
            assert.is_true(s:tcpcork())
            assert.is_true(s:tcpcork(false))
            assert.is_false(s:tcpcork())
        else
            -- test that tcpcork socket option cannot be set
            local ok, err = s:tcpcork(true)
            assert(not ok, 'tcpcork() returns true')
            assert(err, 'tcpcork() did not return error')
        end

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.reuseport()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that reuseport socket option can be set
        local defval = s:reuseport()
        assert.equal(s:reuseport(true), defval)
        assert.is_true(s:reuseport())
        assert.is_true(s:reuseport(false))
        assert.is_false(s:reuseport())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.reuseaddr()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that reuseaddr socket option can be set
        local defval = s:reuseaddr()
        assert.equal(s:reuseaddr(true), defval)
        assert.is_true(s:reuseaddr())
        assert.is_true(s:reuseaddr(false))
        assert.is_false(s:reuseaddr())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.broadcast()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that broadcast socket option can be set
        local defval = s:broadcast()
        assert.equal(s:broadcast(true), defval)
        assert.is_true(s:broadcast())
        assert.is_true(s:broadcast(false))
        assert.is_false(s:broadcast())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.debug()
    -- no-test: allowed only for processes with the CAP_NET_ADMIN capability or
    --          an effective user ID of 0.
    -- for _, v in pairs(addrs) do
    --     for _, addr in ipairs(v) do
    --         local s = assert(socket.new(addr))

    --         -- test that debug socket option can be set
    --         assert.is_true(s:debug(true))
    --         assert.is_true(s:debug())
    --         assert.is_false(s:debug(false))
    --         assert.is_false(s:debug())

    --         s:close()
    --         os.remove('./test.sock');
    --     end
    -- end
end

function testcase.keepalive()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that keepalive socket option can be set
        local defval = s:keepalive()
        assert.equal(s:keepalive(true), defval)
        assert.is_true(s:keepalive())
        assert.is_true(s:keepalive(false))
        assert.is_false(s:keepalive())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.oobinline()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that oobinline socket option can be set
        local defval = s:oobinline()
        assert.equal(s:oobinline(true), defval)
        assert.is_true(s:oobinline())
        assert.is_true(s:oobinline(false))
        assert.is_false(s:oobinline())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.dontroute()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that dontroute socket option can be set
        local defval = s:dontroute()
        assert.equal(s:dontroute(true), defval)
        assert.is_true(s:dontroute())
        assert.is_true(s:dontroute(false))
        assert.is_false(s:dontroute())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.timestamp()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that timestamp socket option can be set
        local defval = s:timestamp()
        assert.equal(s:timestamp(true), defval)
        assert.is_true(s:timestamp())
        assert.is_true(s:timestamp(false))
        assert.is_false(s:timestamp())

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.rcvbuf()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that rcvbuf socket option can be set
        local defval = s:rcvbuf()
        local n = 12345
        local ncmp = n
        local defcmp = defval

        if llsocket.env.os == 'linux' then
            ncmp = n * 2
            defcmp = defval * 2
        end

        assert.equal(s:rcvbuf(n), defval)
        assert.equal(s:rcvbuf(), ncmp)
        assert.equal(s:rcvbuf(defval), ncmp)
        assert.equal(s:rcvbuf(), defcmp)

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.rcvlowat()
    -- no-test: OS-specific behaviors
end

function testcase.sndbuf()
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that sndbuf socket option can be set
        local defval = assert(s:sndbuf())
        local n = 12345
        local ncmp = n
        local defcmp = defval

        if llsocket.env.os == 'linux' then
            ncmp = n * 2
            defcmp = defval * 2
        end

        assert.equal(s:sndbuf(n), defval)
        assert.equal(s:sndbuf(), ncmp)
        assert.equal(s:sndbuf(defval), ncmp)
        assert.equal(s:sndbuf(), defcmp)

        s:close()
        os.remove('./test.sock');
    end
end

function testcase.sndlowat()
    -- no-test: OS-specific behaviors
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

function testcase.rcvtimeo()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local t = timer.new()

    -- NOTE: depends on the OS scheduler
    -- test that returns again=true after 0.12 sec
    assert(sp[1]:rcvtimeo(0.12))
    collectgarbage('collect')
    collectgarbage('stop')
    t:start()
    local msg, err, again = sp[1]:recv()
    local tv, _, unit = t:stop()
    collectgarbage('restart')
    assert.is_nil(msg)
    assert.is_nil(err)
    assert.is_true(again)
    tv = time2ms(tv, unit)
    assert.is_true(110 < tv and tv < 450)

    for _, s in ipairs(sp) do
        s:close()
    end
end

function testcase.sndtimeo()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local t = timer.new()

    -- fill the buffer
    sp[1]:nonblock(true)
    while 1 do
        -- luacheck: ignore err
        local _, err, again = sp[1]:send('foo')
        assert(not err, err)
        if again then
            sp[1]:nonblock(false)

            -- NOTE: depends on the OS scheduler
            -- test that returns again=true after 0.12 sec
            assert(sp[1]:sndtimeo(0.12))
            collectgarbage('collect')
            collectgarbage('stop')
            t:start()
            _, _, again = sp[1]:send('hello')
            local tv, _, unit = t:stop()
            collectgarbage('restart')
            assert.is_true(again)
            tv = time2ms(tv, unit)
            assert.is_true(110 < tv and tv < 450)
            break
        end
    end

    for _, s in ipairs(sp) do
        s:close()
    end
end

function testcase.linger()
    -- no-test: hard to test this option
    for _, ai in pairs(addrs) do
        local s = assert(socket.new(ai:family(), ai:socktype()))

        -- test that rcvbuf socket option can be set
        local defval = s:linger()
        assert.equal(s:linger(123), defval)
        assert.equal(s:linger(), 123)
        assert.equal(s:linger(defval), 123)
        assert.equal(s:linger(), -1)

        s:close()
        os.remove('./test.sock');
    end
end
