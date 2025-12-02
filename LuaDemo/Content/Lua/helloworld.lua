print("hello world")

xx={}
function xx.text(v)
    print("xx.text",v)
end

-- 判断是否有成功引入slua_profiler模块
if slua_profile then
	-- 根据实际的情况填入对应的 host 和 port
    print("slua_profile.start")
	slua_profile.start("127.0.0.1", 8081)
    slua_profile.startLocalRecord()
    slua_profile.startMemoryTrack()
end

require("LuaPanda").start("127.0.0.1",8818);