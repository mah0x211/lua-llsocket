--
-- Copyright (C) 2026 Masatoshi Fukunaga
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
--
-- codegen.lua expands tmpl/llsocket_tmpl.c using symbol lists from var/*.txt
-- and writes the result to src/llsocket.c. It runs as a luarocks-build hook
-- before the build step, so it operates without command-line arguments.
--

local VAR_FILES = {
    'address_family.txt',
    'ai_flag.txt',
    'ipproto.txt',
    'msg_flags.txt',
    'ni_flag.txt',
    'scm_types.txt',
    'socktype.txt',
    'sol_levels.txt',
}

local TMPL_FILE = 'tmpl/llsocket_tmpl.c'
local OUT_FILE = 'src/llsocket.c'

local TMPL = [=[
#ifdef %s
    lauxh_pushint2tbl( L, "%s", %s );
#endif

]=]

local function load_decls()
    local decls = {}
    for _, name in ipairs(VAR_FILES) do
        local path = 'var/' .. name
        local file = assert(io.open(path))
        local seen = {}
        for line in file:lines() do
            local def = line:match('^[a-zA-Z0-9_]+$')
            if not def then
                error('invalid line in ' .. path .. ': ' .. line)
            end
            seen[def] = true
        end
        file:close()

        local arr = {}
        for def in pairs(seen) do
            arr[#arr + 1] = def
        end
        table.sort(arr)

        local body = ''
        for _, def in ipairs(arr) do
            body = body .. TMPL:format(def, def, def)
        end

        local key = ('GEN_%s_DECL'):format(name:gsub('%.txt$', ''):upper())
        decls[key] = body
    end
    return decls
end

local function expand_template(decls)
    local file = assert(io.open(TMPL_FILE))
    local content = assert(file:read('*a'))
    file:close()
    for k, v in pairs(decls) do
        content = content:gsub('#define (' .. k .. ')\n', v)
    end
    local out = assert(io.open(OUT_FILE, 'w'))
    out:write(content)
    out:close()
end

expand_template(load_decls())
