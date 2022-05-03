local unpack = unpack or table.unpack
local pcall = pcall
local testcase = require('testcase')
local has_suffix = require('string.contains').suffix
local llsocket = require('llsocket')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo
local device = llsocket.device
local addrs = {}
local lodev

function testcase.before_all()
    local ipv4 = '127.0.0.1'
    local ipv6 = '::1'
    local port = 8080
    local pathname = './test.sock'
    addrs.inet = {
        stream = assert(addrinfo.inet(ipv4, port, llsocket.SOCK_STREAM)),
        dgram = assert(addrinfo.inet(ipv4, port, llsocket.SOCK_DGRAM)),
    }
    addrs.inet6 = {
        stream = assert(addrinfo.inet6(ipv6, port, llsocket.SOCK_STREAM)),
        dgram = assert(addrinfo.inet6(ipv6, port, llsocket.SOCK_DGRAM)),
    }
    addrs.unix = {
        stream = assert(addrinfo.unix(pathname, llsocket.SOCK_STREAM)),
        dgram = assert(addrinfo.unix(pathname, llsocket.SOCK_DGRAM)),
    }

    -- get loopback interface
    local list = assert(device.getifaddrs())
    for k, v in pairs(list) do
        if v.loopback then
            lodev = k
            break
        end
    end
    assert(lodev, 'loopback interface not found')
end

function testcase.after_all()
    os.remove('./test.sock')
end

function testcase.inet_mcastttl()
    for k, ai in pairs(addrs.inet) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                -- test that sets the IP_MULTICAST_TTL value
                local defval = assert(s:mcastttl())
                assert.equal(s:mcastttl(10), defval)
                assert.equal(s:mcastttl(), 10)
                assert.equal(s:mcastttl(defval), 10)
                assert.equal(s:mcastttl(), defval)

                -- test that throws error with invalid argument
                local err = assert.throws(function()
                    s:mcastttl('foo')
                end)
                assert.match(err, '#1 .+ [(]number expected, got string', false)
            else
                -- test that cannot be sets the IP_MULTICAST_TTL value
                local ttl, err = s:mcastttl()
                assert.is_nil(ttl)
                assert(err, 'mcastttl did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet6_mcastttl()
    for k, ai in pairs(addrs.inet6) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                -- test that sets the IP_MULTICAST_TTL value
                local defval = assert(s:mcastttl())
                assert.equal(s:mcastttl(10), defval)
                assert.equal(s:mcastttl(), 10)
                assert.equal(s:mcastttl(defval), 10)
                assert.equal(s:mcastttl(), defval)

                -- test that throws error with invalid argument
                local err = assert.throws(function()
                    s:mcastttl('foo')
                end)
                assert.match(err, '#1 .+ [(]number expected, got string', false)
            else
                -- test that cannot be sets the IP_MULTICAST_TTL value
                local ttl, err = s:mcastttl()
                assert.is_nil(ttl)
                assert(err, 'mcastttl did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet_mcastloop()
    for k, ai in pairs(addrs.inet) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                -- test that sets the IP_MULTICAST_LOOP value
                local defval, err = s:mcastloop()
                assert(not err, err)
                assert.equal(s:mcastloop(true), defval)
                assert.equal(s:mcastloop(), true)
                assert.equal(s:mcastloop(false), true)
                assert.equal(s:mcastloop(), false)

                -- test that throws error with invalid argument
                local err = assert.throws(function()
                    s:mcastloop('foo')
                end)
                assert.match(err, '#1 .+ [(]boolean expected, got string', false)
            else
                -- test that cannot be sets the IP_MULTICAST_TTL value
                local ttl, err = s:mcastttl()
                assert.is_nil(ttl)
                assert(err, 'mcastttl did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet6_mcastloop()
    for k, ai in pairs(addrs.inet6) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                -- test that sets the IP_MULTICAST_LOOP value
                local defval, err = s:mcastloop()
                assert(not err, err)
                assert.equal(s:mcastloop(true), defval)
                assert.equal(s:mcastloop(), true)
                assert.equal(s:mcastloop(false), true)
                assert.equal(s:mcastloop(), false)

                -- test that throws error with invalid argument
                local err = assert.throws(function()
                    s:mcastloop('foo')
                end)
                assert.match(err, '#1 .+ [(]boolean expected, got string', false)
            else
                -- test that cannot be sets the IP_MULTICAST_TTL value
                local ttl, err = s:mcastttl()
                assert.is_nil(ttl)
                assert(err, 'mcastttl did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet_mcastif()
    for k, ai in pairs(addrs.inet) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                local defval, err = s:mcastif()
                assert(not err, err)

                -- test that sets the IP_MULTICAST_IF value
                assert.equal(s:mcastif(lodev), defval)
                assert.equal(s:mcastif(), lodev)

                -- NOTE: reset is not supported on apple
                -- test that reset the IP_MULTICAST_IF value
                if defval == nil and llsocket.env.os ~= 'apple' then
                    assert.equal(s:mcastif(defval), lodev)
                    assert.equal(s:mcastif(), defval)
                end

                -- test that throws an error with invalid arguments
                err = assert.throws(function()
                    s:mcastif(123)
                end)
                assert.match(err, '#1 .+ [(]string expected, got number', false)
            else
                -- test that cannot be sets the IP_MULTICAST_IF value
                local defval, err = s:mcastif()
                assert.is_nil(defval)
                assert(err, 'mcastif did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet6_mcastif()
    for k, ai in pairs(addrs.inet6) do
        local s = assert(socket.new(ai:family(), ai:socktype()))
        local ok, err = pcall(function()
            if has_suffix(k, 'dgram') then
                local defval, err = s:mcastif()
                assert(not err, err)

                -- test that sets the IP_MULTICAST_IF value
                assert.equal(s:mcastif(lodev), defval)
                assert.equal(s:mcastif(), lodev)

                -- NOTE: reset is not supported on apple
                -- test that reset the IP_MULTICAST_IF value
                if defval == nil and llsocket.env.os ~= 'apple' then
                    assert.equal(s:mcastif(defval), lodev)
                    assert.equal(s:mcastif(), defval)
                end

                -- test that throws an error with invalid arguments
                err = assert.throws(function()
                    s:mcastif(123)
                end)
                assert.match(err, '#1 .+ [(]string expected, got number', false)
            else
                -- test that cannot be sets the IP_MULTICAST_IF value
                local defval, err = s:mcastif()
                assert.is_nil(defval)
                assert(err, 'mcastif did not returns error')
            end
        end)
        s:close()
        assert(ok, err)
    end
end

function testcase.inet_mcastjoin_leave()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local s = assert(socket.new(llsocket.AF_INET, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        -- test that join and leave
        assert(s:mcastjoin(grp))
        assert(s:mcastleave(grp))

        -- test that join and leave with ifname argument
        assert(s:mcastjoin(grp, lodev))
        assert(s:mcastleave(grp, lodev))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                addrs.inet.dgram,
            },
            {
                -- invalid address family
                grp6,
                lodev,
            },
            {
                -- invalid device
                grp,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastjoin(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastjoin() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastleave(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastleave() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp,
                    true,
                },
                err = '#2 .+ [(]string expected, got boolean',
            },
        }) do
            local err = assert.throws(function()
                s:mcastjoin(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastleave(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end
    end)
    s:close()
    assert(ok, err)
end

function testcase.inet6_mcastjoin_leave()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local s = assert(socket.new(llsocket.AF_INET6, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        -- test that join and leave with ifname argument
        assert(s:mcastjoin(grp6, lodev))
        assert(s:mcastleave(grp6, lodev))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                addrs.inet6.dgram,
            },
            {
                -- invalid address family
                grp,
                lodev,
            },
            {
                -- invalid device
                grp6,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastjoin(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastjoin() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastleave(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastleave() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp6,
                    true,
                },
                err = '#2 .+ [(]string expected, got boolean',
            },
        }) do
            local err = assert.throws(function()
                s:mcastjoin(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastleave(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end
    end)
    s:close()
    assert(ok, err)
end

function testcase.inet_mcastjoinsrc_leavesrc()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local src = addrs.inet.dgram
    local s = assert(socket.new(llsocket.AF_INET, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        -- test that join and leave
        assert(s:mcastjoinsrc(grp, src))
        assert(s:mcastleavesrc(grp, src))

        -- test that join and leave with ifname argument
        assert(s:mcastjoinsrc(grp, src, lodev))
        assert(s:mcastleavesrc(grp, src, lodev))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                src,
                grp,
            },
            {
                -- invalid address family
                src,
                grp6,
            },
            {
                -- invalid device
                grp,
                src,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastjoinsrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastjoinsrc() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastleavesrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastleavesrc() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp,
                    true,
                },
                err = '#2 .+ [(]llsocket.addrinfo expected, got boolean',
            },
            {
                arg = {
                    grp,
                    src,
                    {},
                },
                err = '#3 .+ [(]string expected, got table',
            },
        }) do
            local err = assert.throws(function()
                s:mcastjoinsrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastleavesrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end
    end)
    s:close()
    assert(ok, err)
end

function testcase.inet6_mcastjoinsrc_leavesrc()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local src = addrs.inet6.dgram
    local s = assert(socket.new(llsocket.AF_INET6, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        -- test that join and leave with ifname argument
        assert(s:mcastjoinsrc(grp6, src, lodev))
        assert(s:mcastleavesrc(grp6, src, lodev))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                src,
                grp6,
            },
            {
                -- invalid address family
                grp,
                src,
            },
            {
                -- invalid device
                grp6,
                src,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastjoinsrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastjoinsrc() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastleavesrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastleavesrc() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp,
                    true,
                },
                err = '#2 .+ [(]llsocket.addrinfo expected, got boolean',
            },
            {
                arg = {
                    grp,
                    src,
                    {},
                },
                err = '#3 .+ [(]string expected, got table',
            },
        }) do
            local err = assert.throws(function()
                s:mcastjoinsrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastleavesrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end
    end)
    s:close()
    assert(ok, err)
end

function testcase.inet_mcastblock_unblock()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local src = addrs.inet.dgram
    local s = assert(socket.new(llsocket.AF_INET, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        assert(s:mcastjoin(grp))

        -- test that block and unblock
        assert(s:mcastblocksrc(grp, src))
        assert(s:mcastunblocksrc(grp, src))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                src,
                grp,
            },
            {
                -- invalid address family
                grp,
                grp6,
            },
            {
                -- invalid device
                grp,
                src,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastblocksrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastblocksrc() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastunblocksrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastunblocksrc() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp,
                    true,
                },
                err = '#2 .+ [(]llsocket.addrinfo expected, got boolean',
            },
            {
                arg = {
                    grp,
                    src,
                    {},
                },
                err = '#3 .+ [(]string expected, got table',
            },
        }) do
            local err = assert.throws(function()
                s:mcastblocksrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastunblocksrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end

        assert(s:mcastleave(grp))
    end)
    s:close()
    assert(ok, err)
end

function testcase.inet6_mcastblock_unblock()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local grp6 = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    local src = addrs.inet6.dgram
    local s = assert(socket.new(llsocket.AF_INET6, llsocket.SOCK_DGRAM))
    local ok, err = pcall(function()
        assert(s:mcastjoin(grp6, lodev))

        -- test that block and unblock
        assert(s:mcastblocksrc(grp6, src, lodev))
        assert(s:mcastunblocksrc(grp6, src, lodev))

        -- test that returns error with invalid argument
        for _, arg in ipairs({
            {
                -- non-multicast address
                src,
                src,
            },
            {
                -- invalid address family
                grp,
                grp6,
            },
            {
                -- invalid device
                grp6,
                src,
                'foo.bar.baz.qux',
            },
        }) do
            local ok, err = s:mcastblocksrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastblocksrc() did not returns error')

            -- test that returns error with invalid ip
            ok, err = s:mcastunblocksrc(unpack(arg))
            assert.is_false(ok)
            assert(err, 'mcastunblocksrc() did not returns error')
        end

        -- test that throws an error with invalid arguments
        for _, v in ipairs({
            {
                arg = {
                    1,
                },
                err = '#1 .+ [(]llsocket.addrinfo expected, got number',
            },
            {
                arg = {
                    grp6,
                    true,
                },
                err = '#2 .+ [(]llsocket.addrinfo expected, got boolean',
            },
            {
                arg = {
                    grp6,
                    src,
                    {},
                },
                err = '#3 .+ [(]string expected, got table',
            },
        }) do
            local err = assert.throws(function()
                s:mcastblocksrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)

            err = assert.throws(function()
                s:mcastunblocksrc(unpack(v.arg))
            end)
            assert.match(err, v.err, false)
        end

        assert(s:mcastleave(grp6, lodev))
    end)
    s:close()
    assert(ok, err)
end

function testcase.ipv4_sendto_recvfrom()
    local grp = assert(addrinfo.inet('224.0.0.251', 8080, llsocket.SOCK_DGRAM))
    local cmsg = 'hello world!'

    -- test that send and recv message via ipv4 multicast
    local s = socket.new(llsocket.AF_INET, llsocket.SOCK_DGRAM)
    local c = socket.new(llsocket.AF_INET, llsocket.SOCK_DGRAM)
    local ok, err = pcall(function()
        assert(s:bind(grp))
        assert(s:mcastjoin(grp, lodev))

        -- send message
        c:mcastif(lodev)
        local n = assert(c:sendto(cmsg, grp))
        assert.equal(n, #cmsg)

        -- recv message
        local msg = assert(s:recv())
        assert.equal(msg, cmsg)
    end)

    s:close()
    c:close()
    assert(ok, err)
end

function testcase.ipv6_sendto_recvfrom()
    local cmsg = 'hello world!'
    local s = assert(socket.new(llsocket.AF_INET6, llsocket.SOCK_DGRAM))
    local c = assert(socket.new(llsocket.AF_INET6, llsocket.SOCK_DGRAM))
    local grp = assert(addrinfo.inet6('ff02::1', 8080, llsocket.SOCK_DGRAM))
    if llsocket.env.os == 'linux' then
        grp = assert(addrinfo.inet6('::1', 8080, llsocket.SOCK_DGRAM))
    end

    local ok, err = pcall(function()
        assert(s:bind(grp))
        if llsocket.env.os ~= 'linux' then
            assert(s:mcastjoin(grp, lodev))
        end

        -- send message
        local _, err = c:mcastif(lodev)
        assert(not err, err)
        local n = assert(c:sendto(cmsg, grp))
        assert.equal(n, #cmsg)

        -- recv message
        local msg = assert(s:recv())
        assert.equal(msg, cmsg)
    end)

    s:close()
    c:close()
    assert(ok, err)
end
