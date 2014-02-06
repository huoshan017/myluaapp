local script_path = HSSystem:GetCurrentDir() .. "script\\"
package.path = package.path .. ";" .. script_path .. "?.lua"

dofile(script_path .. "main.lua")
dofile(script_path .. "hs_base.lua")
dofile(script_path .. "hs_activity.lua")

--dofile(script_path .. "list_test.lua")