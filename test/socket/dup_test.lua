local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.dup()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))
    local s1, s2 = sp[1], sp[2]
    s1:nonblock(true)
    s2:nonblock(true)

    -- test that returns duplicate of socket
    local ds = assert(s1:dup())
    assert.match(tostring(ds), '^llsocket.socket: ', false)

    -- test that the dup socket can read message
    assert(s2:send('hello'))
    local msg, err, again = assert(ds:recv())
    assert.equal(msg, 'hello')
    assert.is_nil(err)
    assert.is_nil(again)

    -- test that the dup socket cannot read message when it is consumed by
    -- another socket
    assert(s2:send('hello'))
    msg = s1:recv()
    assert.equal(msg, 'hello')
    msg, err, again = ds:recv()
    assert.is_nil(msg)
    assert.is_nil(err)
    assert.is_true(again)
end

