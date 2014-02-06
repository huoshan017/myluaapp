require("hs_activity")

local s_last_time = 0

function run()
    local interval = 1
    local cur_time = os.time()
    if cur_time - s_last_time >= interval then
        ActivityManager:run()
        s_last_time = cur_time
        --print("ActivityManager:run")
    end
end