require("hs_base")

-- 时间
TIME_TYPE_EVERYDAY = 0          -- 每天
TIME_TYPE_DAY = 1               -- 月日
TIME_TYPE_WDAY = 2              -- 星期
TIME_TYPE_DEFAULT = TIME_TYPE_EVERYDAY

-- 状态
ACTIVITY_STATE_IDLE = 0
ACTIVITY_STATE_RUNNING = 1
ACTIVITY_STATE_END = 2

-- 活动类
Activity = {state = ACTIVITY_STATE_IDLE, to_end = false}

function Activity:new(o)
    return base_object(o, self)
end

function Activity:get_state()
    return self.state
end

-- 设置时间，可能星期也可能月日
-- time {
--     act_type = nil,
--     wdays = nil,
--     begin_wday = nil,
--     end_wday = nil,

--     begin_year = nil,
--     begin_month = nil,
--     begin_day = nil,
--     end_year = nil,
--     end_month = nil,
--     end_day = nil,

--     begin_hour = nil,
--     begin_minute = nil,
--     end_hour = nil,
--     end_minute = nil,

--     duration = nil,
-- }
function Activity:set_time(time)
    self.time = time
end

local function check_begin_hour_minute(cur_date, setting_time)
    if setting_time.begin_hour ~= nil and setting_time.begin_minute ~= nil then
        if cur_date.hour < setting_time.begin_hour then
            return 0
        elseif cur_date.hour == setting_time.begin_hour then
            if cur_date.min < setting_time.begin_minute then
                return 0
            end
        end
    end
    return 1
end

local function check_end_hour_minute(cur_date, setting_time)
    if setting_time.end_hour ~= nil and setting_time.end_minute ~= nil then
        if cur_date.hour > setting_time.end_hour then
            return 0
        elseif cur_date.hour == setting_time.end_hour then
            if cur_date.min > setting_time.end_minute then
               return 0
            end
        end
    end
    return 1
end

-- 检测时间，返回1表示在范围内，0表示不在
function Activity:check_state(cur_date)
    local st = self.time
    if st.act_type == TIME_TYPE_DAY then
        if st.begin_year ~= nil then
            if cur_date.year < st.begin_year then
                return 0
            end
        end
        if st.end_year ~= nil then
            if cur_date.year > st.end_year then
                return 0
            end
            -- st.end_year 小于 st.begin_year
            if st.begin_year ~= nil and st.begin_year > st.end_year then
                return 0
            end
        end
        
        if st.begin_month ~= nil then
            if cur_date.month < st.begin_month and st.begin_year ~= nil and st.begin_year == cur_date.year then
                return 0
            end
        end
        if st.end_month ~= nil then
            if cur_date.month > st.end_month and st.end_year ~= nil and cur_date.year == st.end_year then
                return 0
            end
            
            -- st.end_month 小于 st.begin_month
            if st.begin_month ~= nil and st.end_month < st.begin_month and st.end_year ~= nil and cur_date.year == st.end_year then
                return 0
            end
        end
        
        if st.begin_day ~= nil then
            if cur_date.day < st.begin_day and st.begin_year ~= nil and st.begin_year == cur_date.year and st.begin_month ~= nil and st.begin_month == cur_date.month then
                return 0
            end
        end
        if st.end_day ~= nil then
            if cur_date.day > st.end_day and st.end_year ~= nil and st.end_year == cur_date.year and st.end_month ~= nil and st.end_month == cur_date.month then
                return 0
            end
            
            if st.begin_day ~= nil and st.end_day < st.begin_day 
                and st.end_year ~= nil and cur_date.year == st.end_year 
                and st.end_month ~= nil and cur_date.month == st.end_month then
                return 0
            end
        end
    elseif st.act_type == TIME_TYPE_WDAY then
        if st.begin_wday == nil then
            st.begin_wday = 0
        end
        if st.end_wday == nil then
            st.end_wday = 6
        end
        
        if st.begin_wday < st.end_wday then
            if cur_date.wday-1 < st.begin_wday or cur_date.wday-1 > st.end_wday then
                return 0
            end
        elseif st.begin_wday > st.end_wday then
            if cur_date.wday-1 > st.end_wday and cur_date.wday-1 < st.begin_wday then
                return 0
            end
        else
            if cur_date.wday-1 ~= st.begin_wday then
                return 0
            end
        end
    elseif st.act_type == TIMIE_TYPE_EVERYDAY then
        if st.begin_year==nil or st.end_year==nil or st.begin_month==nil or st.end_month==nil or st.begin_day==nil or st.end_day==nil then
            return 0
        end
        if cur_date.year < st.begin_year or cur_date.year > st.end_year then
            return 0
        end
        
        if cur_date.month < st.begin_month or cur_date.month > st.end_month then
            return 0
        end
        
        if cur_date.day < st.begin_day or cur_date.day > st.end_day then
            return 0
        end
    end
    
    -- 判断时分（begin_hour和begin_minute要么都填要么都不填，end_hour和end_minute也一样）
    if (st.begin_hour==nil and st.begin_minute~=nil) or (st.begin_hour~=nil and st.begin_minute==nil) 
        or (st.end_hour==nil and st.end_minute~=nil) or (st.end_hour~=nil and st.end_minute==nil) then
        return 0
    end

    if st.act_type == TIME_TYPE_DAY then
        if (st.begin_year ~= nil and cur_date.year == st.begin_year
            and st.begin_month ~= nil and cur_date.month == st.begin_month
            and st.begin_day ~= nil and cur_date.day == st.begin_day) then
            if st.begin_hour ~= nil then
                if check_begin_hour_minute(cur_date, st) == 0 then
                    return 0
                end
            end
        end
    
        if (st.end_year ~= nil and cur_date.year == st.end_year
            and st.end_month ~= nil and cur_date.month == st.end_month
            and st.end_day ~= nil and cur_date.day == st.end_day) then
            if st.end_hour ~= nil then
                if check_end_hour_minute(cur_date, st) == 0 then
                    return 0
                end
            end
        end
    elseif st.act_type == TIME_TYPE_WDAY then
        if cur_date.wday == st.begin_wday then
            if check_begin_hour_minute(cur_date, st) == 0 then
                return 0
            end
        elseif st.begin_wday ~= st.end_wday and cur_date.wday == st.end_wday then
            if check_end_hour_minute(cur_date, st) == 0 then
                return 0
            end
        end
    elseif st.act_type == TIME_TYPE_EVERYDAY then
        if check_begin_hour_minute(cur_date, st) == 0 then
            return 0
        end
        if check_end_hour_minute(cur_date, st) == 0 then
            return 0
        end
    else
        -- 不处理
    end

    return 1
end

-- 获取剩余时间，可能是负数
function Activity:get_left_time(cur_date)
    local left_time = nil
    -- 剩余开始时间
    if self.state == ACTIVITY_STATE_IDLE or self.state == ACTIVITY_STATE_END then
        if self.time.begin_hour == nil or self.time.begin_minute == nil then
            return nil
        end
        
        if self.time.act_type == TIME_TYPE_DAY then
            local y = self.time.begin_year
            if y == nil then
                y = cur_date.year
            end
            local m = self.time.begin_month
            if m == nil then
                m = cur_date.month
            end
            local d = self.time.begin_day
            if d == nil then
                d = cur_date.day
            end
            
            local bt = os.time{year=y,month=m,day=d,hour=self.time.begin_hour,min=self.time.begin_minute}
            local ct = os.time{year=cur_date.year,month=cur_date.month,day=cur_date.day,hour=cur_date.hour,min=cur_date.min}
            
            left_time = bt - ct
        elseif self.time.act_type == TIME_TYPE_WDAY then
            local bw = self.time.begin_wday
            if bw == nil then
                bw = cur_date.wday-1
            end
            -- 当前时间大于开始时间
            if bw < cur_date.wday-1 
                or (bw==cur_date.wday-1 and self.time.begin_hour>cur_date.hour)
                or (bw==cur_date.wday-1 and self.time.begin_hour==cur_date.hour and self.time.begin_minute>cur_date.min) then
                bw = bw + 7
            end
            
            left_time = (bw-(cur_date.wday-1))*24*3600 + (self.time.begin_hour-cur_date.hour)*3600 + (self.time.begin_minute-cur_date.min)*60 + (0-cur_date.sec)*1
        elseif self.time.act_type == TIME_TYPE_EVERYDAY then
            left_time = (self.time.begin_hour*3600+self.time.begin_minute*60) - (cur_date.hour*3600+cur_date.min*60+cur_date.sec)
            if left_time < 0 then
                left_time = left_time + 24*3600
            end
        end
    -- 剩余结束时间
    elseif self.state == ACTIVITY_STATE_RUNNING then
        if self.time.end_hour == nil or self.time.end_minute == nil then
            return nil
        end
        if self.time.act_type == TIME_TYPE_DAY then
            local y = self.time.end_year
            local m = self.time.end_month
            local d = self.time.end_day
            if y == nil then y = cur_date.year end
            if m == nil then m = cur_date.month end
            if d == nil then d = cur_date.day end
            
            local et = os.time{year=y,month=m,day=d,hour=self.time.end_hour,min=self.time.end_minute}
            local ct = os.time{year=cur_date.year,month=cur_date.month,day=cur_date.day,hour=cur_date.hour,min=cur_date.min}
            
            left_time = et - ct
        elseif self.time.act_type == TIME_TYPE_WDAY then
            left_time = (self.time.end_wday-(cur_date.wday-1))*24*2600 + (self.time.end_hour-cur_date.hour)*3600 + (self.time.end_minute-cur_date.min)*60 + (0-cur_date.sec)*1
        elseif self.time.act_type == TIME_TYPE_EVERYDAY then
            left_time = (self.time.end_hour*3600+self.time.end_minute*60) - (cur_date.hour*3600+cur_date.min*60+cur_date.sec)
        end
    end
    
    return left_time
end

-- 初始化
function Activity:init(param)
    return
end

-- 开始
function Activity:begin(param)
    return
end

-- 结束
function Activity:finish(param)
    return
end

-- 执行，参数param用来必要的数据
function Activity:doing(param)
    return
end

-- 手动结束
function Activity:set_end(is_end)
    self.to_end = is_end
end

-- 主循环
function Activity:run(cur_date)
    local res = self:check_state(cur_date)
    if self.state == ACTIVITY_STATE_IDLE or self.state == ACTIVITY_STATE_END then
        if res == 1 then
            self:begin()
            self.state = ACTIVITY_STATE_RUNNING
        end
    elseif self.state == ACTIVITY_STATE_RUNNING then
        if res == 1 then
            if self.to_end == true then
                self:finish()
                self.state = ACTIVITY_STATE_END
                self.to_end = false
            else
                self:doing()
            end
        else
            self:finish()
            self.state = ACTIVITY_STATE_IDLE
        end
    else
        -- 未知的状态
    end
end

-- 活动集
ActivityManager = {act_sets={}}

function ActivityManager:add(activity)
    table.insert(self.act_sets, activity)
end

function ActivityManager:add_by_type(act_type, activity)
    self.act_sets[act_type] = activity
end

function ActivityManager:get(act_type)
    return self.act_sets[act_type]
end

local function get_cur_date()
    local ct = os.time()
	cur_date = os.date("*t", ct)
	return cur_date
end

function ActivityManager:run(cur_date)
    if cur_date == nil then
        cur_date = get_cur_date()
    end
    for _, a in pairs(self.act_sets) do
        a:run(cur_date)
    end
end

--[[
-- 下面是派生类

local config_mgr = get_cconfig_mgr()
if config_mgr == nil then
    assert("cconfig manager not instance")
end

-- 挑战双倍
ChallengeDoubleActivity = Activity:new()

function ChallengeDoubleActivity:begin()
    config_mgr:Lua_SetFightDoubleExpSwitch(1)
    print("ChallengeDoubleActivity:begin")
end

function ChallengeDoubleActivity:finish()
    config_mgr:Lua_SetFightDoubleExpSwitch(0)
    print("ChallengeDoubleActivity:finish")
end

-- 副本
FubenActivity = Activity:new()

function FubenActivity:begin()
    config_mgr:Lua_SetInstanceMultiGoldSwitch(1)
    config_mgr:Lua_SetInstanceMultiGoldFactor(2)
    print("FubenActivity:begin")
end

function FubenActivity:finish()
    config_mgr:Lua_SetInstanceMultiGoldSwitch(0)
    --config_mgr:Lua_SetInstanceMultiGoldFactor(1)
    print("FubenActivity:finish")
end

-- 荣誉
GloryActivity = Activity:new()

function GloryActivity:begin()
    config_mgr:Lua_SetDailyGloryLimit0(300)
    config_mgr:Lua_SetDailyGloryLimit1(300)
    print("GloryActivity:begin")
end

function GloryActivity:finish()
    config_mgr:Lua_SetDailyGloryLimit0(150)
    config_mgr:Lua_SetDailyGloryLimit1(300)
    print("GloryActivity:finish")
end

-- 登陆奖励 { players = {}, award_items = {} }
LoginActivity = Activity:new()

function LoginActivity:set_award(items)
    self.award_items = items
end

function LoginActivity:enter_game(player_id)
    local cplayer = get_cplayer(player_id)
    if cplayer == nil then
        return
    end
    if self.state == ACTIVITY_STATE_RUNNING then
        self.players = self.players or {}
        if self.players[player_id] ~= nil then
            return
        end
        for id, num in pairs(self.award_items) do
            cplayer:Lua_AddItem(id, 0, num, "login_award", "boya_login_award")
            local item_name = get_citem_name(id)
            if item_name ~= nil then
                local str = string.format("{\"DataType\":\"SystemGiveItemNotify\", \"PlayerID\":%u, \"ItemName\":\"%s\", \"ItemNum\":%d}", player_id, item_name, num)
                --cplayer:Lua_SendToOSStream(str)
            end
        end
        self.players[player_id] = 1
    end
end

-- 商城优惠活动 { items_and_price = {}, items_and_old_price = {} }
MallSpecialOffer = Activity:new()

function MallSpecialOffer:set_special_items(items_and_price)
    self.items_and_price = items_and_price
end

function MallSpecialOffer:begin()
    local item_mgr = get_citem_mgr()
    if item_mgr == nil then
        return
    end
    self.items_and_old_price = self.items_and_old_price or {}
    for item_id, price in pairs(self.items_and_price) do
        old_price = item_mgr:Lua_GetItemBuyPrice(item_id)
        if old_price ~= nil then
            self.items_and_old_price[item_id] = old_price
        end
        -- 设置新价格
        item_mgr:Lua_ChangeItemBuyPrice(item_id, price)
    end
    print("MallSpecialOffer:begin")
end

function MallSpecialOffer:finish()
    local item_mgr = get_citem_mgr()
    if item_mgr == nil then
        return
    end
    for item_id, old_price in pairs(self.items_and_old_price) do
        if item_mgr:Lua_GetItemBuyPrice(item_id) ~= price then
            item_mgr:Lua_ChangeItemBuyPrice(item_id, old_price)
        end
    end
    print("MallSpecialOffer:finish")
end

-- 多倍经验
MultiExpActivity = Activity:new()

function MultiExpActivity:begin()
    config_mgr:Lua_SetMultiExpSwitch(1)
    config_mgr:Lua_SetMultiExpFactor(2)
    print("MultiExpActivity:begin")
end

function MultiExpActivity:finish()
    config_mgr:Lua_SetMultiExpSwitch(0)
    config_mgr:Lua_SetMultiExpFactor(1)
    print("MultiExpActivity:finish")
end

-- 充值返利
RechargeRewardActivity = Activity:new()

function RechargeRewardActivity:begin()
    config_mgr:Lua_SetRechargeRewardSwitch(1)
    config_mgr:Lua_SetRechargeRewardPercentage(10)
    print("RechargeRewardActivity:begin")
end

function RechargeRewardActivity:finish()
    config_mgr:Lua_SetRechargeRewardSwitch(0)
    print("RechargeRewardActivity:finish")
end

-- 活动数据
local s_login_award_items = {
    [18001] = 1,
    [11503] = 2,
    [16001] = 2,
    [14501] = 5,
    [20506] = 5,
    [13503] = 5,
    --[10503] = 10,
}

-- 活动时间
local s_activity_time = {
    --act_type = TIME_TYPE_WDAY,
    
    --begin_wday = 3,
    --end_wday = 0,
    
    --begin_hour = 0,
    --begin_minute = 0,
    --end_hour = 23,
    --end_minute = 59,
    
    --act_type = TIME_TYPE_DAY,
    
    --begin_year = 2013,
    --end_year = 2013,
    
    --begin_month = 7,
    --end_month = 7,
    
    --begin_day = 4,
    --end_day = 7,
    
	act_type = TIME_TYPE_DAY,
	begin_year = 2013,
    end_year = 2013,
    begin_month = 7,
    end_month = 7,
    begin_day = 6,
    end_day = 7,
    begin_hour = 0,
    begin_minute = 0,
    end_hour = 23,
    end_minute = 59,
}

local s_login_time = {
    act_type = TIME_TYPE_DAY,
    begin_year = 2013,
    end_year = 2013,
    begin_month = 7,
    end_month = 7,
    begin_day = 6,
    end_day = 7,
    begin_hour = 0,
    begin_minute = 0,
    end_hour = 23,
    end_minute = 59,
}

local s_multi_exp_time = {
    act_type = TIME_TYPE_EVERYDAY,
    begin_year = 2013,
    end_year = 2013,
    begin_month = 7,
    end_month = 7,
    begin_day = 5,
    end_day = 7,
    
    begin_hour = 19,
    begin_minute = 0,
    end_hour = 21,
    end_minute = 0,
}

local s_recharge_time = {
	act_type = TIME_TYPE_DAY,
	begin_year = 2013,
	begin_month = 7,
	begin_day = 4,
	end_year = 2013,
	end_month = 7,
	end_day = 7,

	begin_hour = 0,
	begin_minute = 0,
	end_hour = 23,
	end_minute = 59,
}

local s_mall_item_new_price = {
        [10503] = 4,
        [20509] = 12,
        [20005] = 16,
        [20006] = 16,
        [20007] = 799,
        [20008] = 799,
        [20011] = 1510,
        [20012] = 1510,
        [14501] = 8,
        [18001] = 80,
        [18002] = 384,
        [18003] = 688,
        [20503] = 4,
        [20506] = 8,
        [16001] = 40,
        [13503] = 4,
}

-- 创建活动
local challenge_activity = ChallengeDoubleActivity:new()
challenge_activity:set_time(s_activity_time)

local fuben_activity = FubenActivity:new()
fuben_activity:set_time(s_activity_time)

local glory_activity = GloryActivity:new()
glory_activity:set_time(s_activity_time)

local login_activity = LoginActivity:new({players={},award_items=s_login_award_items})
login_activity:set_time(s_login_time)

local multi_exp_activity = MultiExpActivity:new()
multi_exp_activity:set_time(s_multi_exp_time)

local recharge_activity = RechargeRewardActivity:new()
recharge_activity:set_time(s_recharge_time)

local mall_special_offer = MallSpecialOffer:new({items_and_price=s_mall_item_new_price, items_and_old_price={}})
mall_special_offer:set_time(s_activity_time)

-- 把活动加入活动管理器
ActivityManager:add(challenge_activity)
ActivityManager:add(fuben_activity)
--ActivityManager:add(glory_activity)
ActivityManager:add(login_activity)
--ActivityManager:add(multi_exp_activity)
--ActivityManager:add(recharge_activity)
--ActivityManager:add(mall_special_offer)

-- 获得登陆活动
function get_login_activity()
    return login_activity
end
--]]