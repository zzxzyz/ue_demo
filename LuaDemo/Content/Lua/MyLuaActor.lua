print("hello in game manager")

local actor={}

function actor:ReceiveBeginPlay()
    self.bCanEverTick = true
    self.Super:ReceiveBeginPlay()
    print("MyActor:ReceiveBeginPlay")
end

function actor:Tick(reason)
    self.Super:Tick(reason)
end

function actor:ReceiveTick(dt)
end

return Class(nil, nil, actor)