local assert = require('assertex')
local testcase = require('testcase')
local llsocket = require('llsocket')
local device = llsocket.device

function testcase.getifaddrs()
    -- test that returns list of interface address
    local addrs = assert(device.getifaddrs())
    assert.not_empty(addrs)
    for ifname, addr in pairs(addrs) do
        assert.is_string(ifname)
        assert.is_table(addr)
    end
end

