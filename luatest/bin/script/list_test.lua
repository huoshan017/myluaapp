require("hs_list")

local list_values = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
}

local l = slist:new()

for i, v in ipairs(list_values) do
    l:push_back(v)
end

l:insert(10, 9.5)
l:delete(12)

for i=1, l.length do
    local v = l:pop_front()
    print(v .. "\n")
end

local l2 = slist:new()

for i, v in ipairs(list_values) do
    l2:push_front(v)
end

