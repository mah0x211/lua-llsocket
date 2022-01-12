local testcase = require('testcase')
local llsocket = require('llsocket')
local cmsghdrs = llsocket.cmsghdrs
local cmsghdr = llsocket.cmsghdr

function testcase.new()
    -- test that returns new instance of llsocket.cmsghdrs
    local cmhs = cmsghdrs.new()
    assert(cmhs, 'new() returns nil')
    assert.match(tostring(cmhs), '^llsocket.cmsghdrs:', false)
end

function testcase.push()
    local cmhs = assert(cmsghdrs.new())

    -- test that push a cmsghdr
    local cmh = assert(cmsghdr.new(1, 2, 'foo'))
    cmhs:push(cmh)
    local v = cmhs:shift()
    assert.equal({
        level = cmh:level(),
        type = cmh:type(),
        data = cmh:data(),
    }, {
        level = v:level(),
        type = v:type(),
        data = v:data(),
    })

    -- test that throws an error with invalid arguments
    local err = assert.throws(function()
        cmhs:push({})
    end)
    assert.match(err, '#1 .+ [(]llsocket.cmsghdr expected, got table', false)
end
