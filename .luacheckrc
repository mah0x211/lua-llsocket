std = 'max'
include_files = {
    'test/*_test.lua',
    'test/*/*_test.lua',
}
ignore = {
    'assert',
    -- unused argument
    '212',
    -- unused loop variable
    '213',
    -- redefining a local variable
    '411',
    -- shadowing a local variable
    '421',
}
