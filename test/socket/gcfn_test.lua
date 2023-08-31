local testcase = require('testcase')
local errno = require('errno')
local llsocket = require('llsocket')
local socket = llsocket.socket

function testcase.gc_functions_are_called_on_close()
    local calls = {}
    local fn1 = function(...)
        assert.equal({
            ...,
        }, {
            'foo',
            'bar',
        })
        calls[#calls + 1] = 'fn1'
    end
    local fn2 = function(...)
        assert.equal({
            ...,
        }, {
            'baz',
            'qux',
        })
        calls[#calls + 1] = 'fn2'
    end

    do
        -- test that add gc function
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local gcfn1 = sp[1]:addgcfn(debug.traceback, fn1, 'foo', 'bar')
        assert.match(gcfn1, '^llsocket.gcfn:', false)

        -- test that add gc function
        local gcfn2 = sp[1]:addgcfn(debug.traceback, fn2, 'baz', 'qux')
        assert.match(gcfn2, '^llsocket.gcfn:', false)

        -- test that call gc functions
        sp[1]:close()
        assert.equal(calls, {
            'fn2',
            'fn1',
        })

        -- test that cannot add gc function after close
        local gcfn, err = sp[1]:addgcfn(debug.traceback, fn1, 'foo', 'bar')
        assert.equal(gcfn, nil)
        assert.equal(err.type, errno.EBADF)

        calls = {}
    end

    -- test that call gc functions only once
    collectgarbage('collect')
    assert.equal(calls, {})

    do
        -- test that add gc function
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local gcfn1 = sp[1]:addgcfn(debug.traceback, fn1, 'foo', 'bar')
        assert.match(gcfn1, '^llsocket.gcfn:', false)

        -- test that add gc function
        local gcfn2 = sp[1]:addgcfn(debug.traceback, fn2, 'baz', 'qux')
        assert.match(gcfn2, '^llsocket.gcfn:', false)
    end

    -- test that call gc functions on gc
    assert.equal(calls, {})
    collectgarbage('collect')
    assert.equal(calls, {
        'fn2',
        'fn1',
    })
end

function testcase.delete_gc_functions()
    local calls = {}
    local fn1 = function(...)
        assert.equal({
            ...,
        }, {
            'foo',
            'bar',
        })
        calls[#calls + 1] = 'fn1'
    end
    local fn2 = function(...)
        assert.equal({
            ...,
        }, {
            'baz',
            'qux',
        })
        calls[#calls + 1] = 'fn2'
    end
    local fn3 = function(...)
        assert.equal({
            ...,
        }, {
            'hello',
            'world',
        })
        calls[#calls + 1] = 'fn3'
    end

    do
        local sp = assert(socket.pair(llsocket.SOCK_STREAM))
        local gcfn1 = assert(sp[1]:addgcfn(debug.traceback, fn1, 'foo', 'bar'))
        assert(sp[1]:addgcfn(debug.traceback, fn2, 'baz', 'qux'))
        local gcfn3 = assert(sp[1]:addgcfn(debug.traceback, fn3, 'hello',
                                           'world'))

        -- test that remove gc function
        assert.is_true(sp[1]:delgcfn(gcfn1))
        assert.is_true(sp[1]:delgcfn(gcfn3))

        -- test that call gc functions
        sp[1]:close()
        assert.equal(calls, {
            'fn2',
        })
    end
end
