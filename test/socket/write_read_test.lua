local testcase = require('testcase')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.write_read()
    local sp = assert(socket.pair(llsocket.SOCK_STREAM))

    -- test that write a message to peer
    local smsg = 'hello'
    local n = assert(sp[1]:write(smsg))
    assert.equal(n, #smsg)

    -- test that read a message from peer
    local rmsg = assert(sp[2]:read())
    assert.equal(rmsg, smsg)

    sp[1]:close()
    sp[2]:close()
end

