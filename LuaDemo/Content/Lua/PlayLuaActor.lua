print("hello in play lua actor")

local actor={}

function actor:ReceiveBeginPlay()
    print("PlayLuaActor:ReceiveBeginPlay enter")
    self.bCanEverTick = true
    self.Super:ReceiveBeginPlay()
end

function actor:Tick(reason)
    self.Super:Tick(reason)
end

function actor:ReceiveTick(dt)
end

return Class(nil, nil, actor)