
local trim = function(str)
    return (str:match('^()%s*$') and '' or str:match('^%s*(.*%S)'))
end

local Template = {}
Template.__index = Template

function Template.new(input)
    local self = {}
    setmetatable(self, Template)
    self.input = input

    -- default filter function
    -- if this function returns nil, then no further data is appended
    -- to the buffer.
    -- this impl merely checks if the string isn't just useless whitespace
    self.fn_filter = function(src)
        local nstring = trim(src)
        if string.len(nstring) == 0 then
            return nil
        else
            return nstring
        end
    end

    -- default handler for codeblocks
    self.fn_on_codeblock = function(str)
        return string.format("%s\n", str)
    end

    -- default handler for codelines
    self.fn_on_codeline = function(str)
        return string.format("print(%s);\n", str)
    end

    -- default handler for everything else
    self.fn_on_data = function(str)
        return string.format("print(%q);\n", str)
    end

    return self
end

function Template:call_cb(fn, str)
    local newstr = self.fn_filter(str)
    if (newstr ~= nil) then
        -- use 'newstr' to get trimmed output
        return fn(str)
    end
    return ""
end


function Template:filter(fn)
   self.fn_filter = fn
end

function Template:on_codeblock(fn)
    self.fn_on_codeblock = fn
end

function Template:on_codeline(fn)
    self.fn_on_codeline = fn
end

function Template:on_data(self, fn)
    self.fn_on_data = fn
end


function Template:parse()
    -- remember that Lua is 1-index based (sigh) ...
    -- oh, and that Lua's builtin strings don't understand index operations (i.e., src[ci]),
    -- but won't error() about it, and just return nil. it's infuriating.
    local ci = 1
    local buf = ""
    local src = self.input
    local srclen = string.len(src)
    while (ci < srclen) do
        local chunk = ""
        if ((string.sub(src, ci, ci) == '<') and (string.sub(src, ci+1, ci+1) == '%')) then
            ci = ci + 2
            while (true) do
                if ((string.sub(src, ci, ci) == '%') and (string.sub(src, ci+1, ci+1) == '>')) then
                    ci = ci + 2
                    break
                end
                chunk = chunk .. string.sub(src, ci, ci)
                ci = ci + 1
            end
            if (string.sub(chunk, 1, 1) == '=') then
                chunk = string.sub(chunk, 2, string.len(chunk))
                buf = buf .. self:call_cb(self.fn_on_codeline, chunk)
            elseif (string.sub(chunk, 1, 1) == '#') then
                -- skip comments by doing nothing successfully
            else
                buf = buf .. self:call_cb(self.fn_on_codeblock, chunk)
            end
            chunk = ""
        else
            while (ci < srclen) do
                if ((string.sub(src, ci, ci) == '<') and (string.sub(src, ci+1, ci+1) == '%')) then
                    break
                end
                chunk = chunk .. string.sub(src, ci, ci)
                ci = ci + 1
            end
            buf = buf .. self:call_cb(self.fn_on_data, chunk)
            chunk = ""
        end
    end
    return buf
end

return Template
