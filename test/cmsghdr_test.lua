local unpack = unpack or table.unpack
local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local cmsghdr = llsocket.cmsghdr

function testcase.new()
    -- test that returns new instance of llsocket.cmsghdr
    local cmh = cmsghdr.new(1, 2, 'foo')
    assert(cmh, 'new() returns nil')
    assert.match(tostring(cmh), '^llsocket.cmsghdr:', false)
    assert.equal(cmh:level(), 1)
    assert.equal(cmh:type(), 2)
    assert.equal(cmh:data(), 'foo')

    -- test that throws an error with invalid arguments
    for _, v in ipairs({
        {
            arg = {},
            err = '#1 .+ [(]number expected, got no value',
        },
        {
            arg = {
                1,
            },
            err = '#2 .+ [(]number expected, got no value',
        },
        {
            arg = {
                1,
                1,
            },
            err = '#-1 .+ [(]string expected, got nil',
        },
        {
            arg = {
                1,
                1,
                true,
            },
            err = '#-1 .+ [(]string expected, got boolean',
        },
    }) do
        local err = assert.throws(function()
            cmsghdr.new(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end
end

function testcase.rights()
    -- test that returns new instance of llsocket.cmsghdr with file descriptors
    local cmh = cmsghdr.rights(1, 2, 3)
    assert(cmh, 'rights() returns nil')
    assert.match(tostring(cmh), '^llsocket.cmsghdr:', false)
    assert.equal(cmh:level(), llsocket.SOL_SOCKET)
    assert.equal(cmh:type(), llsocket.SCM_RIGHTS)
    assert.equal({
        cmh:data(),
    }, {
        1,
        2,
        3,
    })

    -- test that returns nil with no arguments
    cmh = cmsghdr.rights(1, 2, 3)
    assert(cmh, 'cmsghdr.rights() returns not nil')

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
            err = '#2 .+ [(]number expected, got table',
        },
    }) do
        local err = assert.throws(function()
            cmsghdr.rights(unpack(v.arg))
        end)
        assert.match(err, v.err, false)
    end
end
