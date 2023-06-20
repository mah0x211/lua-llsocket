local configh = require('configh')
local cfgh = configh(os.getenv('CC'))
cfgh:output_status(true)
cfgh:set_feature('_GNU_SOURCE')
do
    local headers = {}
    for _, header in ipairs({
        'sys/types.h',
        'sys/socket.h',
        'sys/sendfile.h',
    }) do
        if cfgh:check_header(header) then
            headers[#headers + 1] = header
        end
    end

    for _, func in ipairs({
        'sendfile',
        'accept4',
    }) do
        cfgh:check_func(headers, func)
    end

    headers = {
        'sys/socket.h',
        'sys/types.h',
    }
    for ctype, members in pairs({
        ['struct sockaddr'] = {
            'sa_len',
        },
    }) do
        if cfgh:check_type(headers, ctype) then
            -- check members
            for _, member in ipairs(members) do
                cfgh:check_member(headers, ctype, member)
            end
        end
    end
end
assert(cfgh:flush('src/config.h'))
