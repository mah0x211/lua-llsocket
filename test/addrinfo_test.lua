local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local addrinfo = llsocket.addrinfo

function testcase.inet()
    local host = '127.0.0.1'
    local port = 8080

    -- create new addrinfo
    local ai = assert(addrinfo.inet(host, port, llsocket.SOCK_STREAM,
                                    llsocket.IPPROTO_TCP, llsocket.AI_PASSIVE))
    assert.match(tostring(ai), '^llsocket.addrinfo:', false)

    -- test that returns ai_family
    assert.equal(ai:family(), llsocket.AF_INET)

    -- test that returns ai_socktype
    assert.equal(ai:socktype(), llsocket.SOCK_STREAM)

    -- test that returns ai_protocol
    assert.equal(ai:protocol(), llsocket.IPPROTO_TCP)

    -- test that returns ai_canonname
    assert.equal(ai:canonname(), nil)

    -- test that returns ai_addr.sin_addr
    assert.equal(ai:addr(), host)

    -- test that returns ai_addr.sin_port
    assert.equal(ai:port(), port)

    -- test that returns nameinfo as table
    local nameinfo = assert(ai:getnameinfo())
    assert.not_empty(nameinfo)
    assert.equal(nameinfo.host, 'localhost')
    assert.equal(nameinfo.service, 'http-alt')
end

function testcase.inet6()
    local host = '::1'
    local port = 8080

    -- create new addrinfo
    local ai = assert(addrinfo.inet6(host, port, llsocket.SOCK_STREAM,
                                     llsocket.IPPROTO_TCP, llsocket.AI_PASSIVE))
    assert.match(tostring(ai), '^llsocket.addrinfo:', false)

    -- test that returns ai_family
    assert.equal(ai:family(), llsocket.AF_INET6)

    -- test that returns ai_socktype
    assert.equal(ai:socktype(), llsocket.SOCK_STREAM)

    -- test that returns ai_protocol
    assert.equal(ai:protocol(), llsocket.IPPROTO_TCP)

    -- test that returns ai_canonname
    assert.equal(ai:canonname(), nil)

    -- test that returns ai_addr.sin_addr
    assert.equal(ai:addr(), host)

    -- test that returns ai_addr.sin_port
    assert.equal(ai:port(), port)

    -- test that returns nameinfo as table
    local nameinfo = assert(ai:getnameinfo())
    assert.not_empty(nameinfo)
    assert.equal(nameinfo.host, 'localhost')
    assert.equal(nameinfo.service, 'http-alt')
end

function testcase.unix()
    local pathname = './test.sock'

    -- create new addrinfo
    local ai = assert(addrinfo.unix(pathname, llsocket.SOCK_STREAM,
                                    llsocket.IPPROTO_TCP, llsocket.AI_PASSIVE))
    assert.match(tostring(ai), '^llsocket.addrinfo:', false)

    -- test that returns ai_family
    assert.equal(ai:family(), llsocket.AF_UNIX)

    -- test that returns ai_socktype
    assert.equal(ai:socktype(), llsocket.SOCK_STREAM)

    -- test that returns ai_protocol
    assert.equal(ai:protocol(), llsocket.IPPROTO_TCP)

    -- test that returns ai_canonname
    assert.equal(ai:canonname(), nil)

    -- test that returns ai_addr.sin_addr
    assert.equal(ai:addr(), pathname)

    -- test that returns ai_addr.sin_port
    assert.is_nil(ai:port())

    -- -- NOTE: In the case of unix domain sockets, the behavior of getnameinfo()
    -- -- is platform-dependent and difficult to test..
    -- -- on darwin, it will return an error.
    -- local nameinfo, err = ai:getnameinfo()
    -- assert(err, 'getnameinfo() does not returns error')
    -- assert(not nameinfo, 'getnameinfo() does not returns nil nameinfo')
end

function testcase.getaddrinfo()
    local node = '127.0.0.1'
    local service = '8080'

    -- test that returns address list
    local list, err = addrinfo.getaddrinfo(node, service, llsocket.AF_UNSPEC,
                                           llsocket.SOCK_STREAM,
                                           llsocket.IPPROTO_TCP,
                                           llsocket.AI_PASSIVE)
    assert(not err, err)
    assert.not_empty(list)

    -- test that throws error with invalid host
    err = assert.throws(function()
        addrinfo.getaddrinfo(1)
    end)
    assert(err, 'getaddrinfo() does not throws error')
    assert.match(err, '#1 .+string expected', false)

    -- test that throws error with invalid port
    err = assert.throws(function()
        addrinfo.getaddrinfo(node, 1)
    end)
    assert(err, 'getaddrinfo() does not throws error')
    assert.match(err, '#2 .+string expected', false)

    -- test that throws error with invalid socktype
    err = assert.throws(function()
        addrinfo.getaddrinfo(node, service, 'invalid socktype')
    end)
    assert(err, 'getaddrinfo() does not throws error')
    assert.match(err, '#3 .+number expected', false)

    -- test that throws error with invalid protocol
    err = assert.throws(function()
        addrinfo.getaddrinfo(node, service, llsocket.SOCK_STREAM,
                             'invalid protocol')
    end)
    assert(err, 'getaddrinfo() does not throws error')
    assert.match(err, '#4 .+number expected', false)

    -- test that throws error with invalid flags
    err = assert.throws(function()
        addrinfo.getaddrinfo(node, service, llsocket.SOCK_STREAM,
                             llsocket.IPPROTO_TCP, 'invalid falgs')
    end)
    assert(err, 'getaddrinfo() does not throws error')
    assert.match(err, '#5 .+number expected', false)
end
