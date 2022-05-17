local testcase = require('testcase')
local iovec = require('iovec')
local llsocket = require('llsocket')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo

function testcase.send_recv()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that send a message to peer
    local smsg = 'hello'
    local n = assert(sp[1]:send(smsg))
    assert.equal(n, #smsg)

    -- test that recv a message from peer
    local rmsg = assert(sp[2]:recvfrom())
    assert.equal(rmsg, smsg)

    sp[1]:close()
    sp[2]:close()
end

function testcase.sendto_recvfrom()
    local ai1 = assert(addrinfo.inet('127.0.0.1', 8080, llsocket.SOCK_DGRAM))
    local s1 = assert(socket.new(ai1:family(), ai1:socktype()))
    s1:reuseaddr(true)
    s1:bind(ai1)
    local ai2 = assert(addrinfo.inet('127.0.0.1', 8081, llsocket.SOCK_DGRAM))
    local s2 = assert(socket.new(ai2:family(), ai2:socktype()))
    s2:reuseaddr(true)
    s2:bind(ai2)

    -- test that send a message to peer
    local smsg = 'hello'
    local n = assert(s1:sendto(smsg, ai2))
    assert.equal(n, #smsg)

    -- test that recv a message from peer
    local rmsg, _, _, ai = assert(s2:recvfrom())
    assert.equal(rmsg, smsg)
    assert.equal(ai:getnameinfo(), ai1:getnameinfo())

    s1:close()
    s2:close()
end

function testcase.sendmsg_recvmsg()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local siov = iovec.new()
    siov:add('hello')
    siov:add('world')
    local riov = iovec.new()
    riov:addn(127)

    -- test that send a message to peer
    local smh = llsocket.msghdr.new()
    smh:iov(siov)
    local n = assert(sp[1]:sendmsg(smh))
    assert.equal(n, siov:bytes())

    -- test that recv a message from peer
    local rmh = llsocket.msghdr.new()
    rmh:iov(riov)
    n = assert(sp[2]:recvmsg(rmh))
    assert.equal(n, siov:bytes())
    assert.equal(riov:concat(0, n), siov:concat())

    sp[1]:close()
    sp[2]:close()
end

function testcase.sendfd_recvfd()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that send a fd
    local n = assert(sp[1]:sendfd(sp[1]:fd()))
    assert.equal(n, 0)

    -- test that recv a message from peer
    local fd = assert(sp[2]:recvfd())
    local s = assert(socket.wrap(fd))
    assert(s:send('hello'))
    assert.equal(sp[2]:recv(), 'hello')
    s:close()

    sp[1]:close()
    sp[2]:close()
end

function testcase.sendfile_recv()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local simg = assert(io.open('./small.png'))
    local limg = assert(io.open('./large.png'))

    -- test that send a small file
    local n, err, again = assert(sp[1]:sendfile(simg, simg:seek('end')))
    assert.equal(n, simg:seek('end'))
    assert.is_nil(err)
    assert.is_nil(again)
    local data = assert(sp[2]:recv())
    assert.equal(#data, n)

    -- test that send a large file
    local _
    _, err = sp[1]:nonblock(true)
    assert(not err, err)
    local size = limg:seek('end')
    local remain = size
    local offset = 0
    local total = 0
    -- repeat until file has been received
    repeat
        n, err, again = sp[1]:sendfile(limg, remain, offset)
        assert(not err, err)

        -- update next params
        offset = assert.less_or_equal(offset + n, size)
        remain = assert.greater_or_equal(remain - n, 0)

        -- repeat until all sent data has been received
        repeat
            -- luacheck: ignore data
            data = assert(sp[2]:recv())
            n = assert.greater_or_equal(n - #data, 0)
            total = total + #data
        until n == 0
    until not again
    assert.equal(size, total)

    for _, v in ipairs({
        sp[1],
        sp[2],
        simg,
        limg,
    }) do
        v:close()
    end
end

