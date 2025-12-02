local LoginWidget = {}

function LoginWidget:NativeConstruct()
    -- 注意：不需要调用 Super:NativeConstruct()，因为 C++ 已经调用了父类的 NativeConstruct
    -- 直接绑定 CancelButton 的 OnClicked 委托到 Lua 函数
    print("NativeConstruct in Lua!")
    if self.CancelButton then
        print("Binding CancelButton OnClicked in Lua")
        -- OnClicked 是多播委托，使用 Add 而不是 Bind
        self.CancelButton.OnClicked:Add(function()
            print("OnClicked callback called in Lua")
            self:OnCancelButtonClicked()
        end)
        print("CancelButton OnClicked bound successfully")
    else
        print("CancelButton is nil!")
    end

    -- 在 Lua 中获取 CloseButton（通过 FindWidget 方法）
    self.CloseButton = self:FindWidget("CloseButton")
    if self.CloseButton then
        print("Binding CloseButton OnClicked in Lua")
        self.CloseButton.OnClicked:Add(function()
            self:OnCloseButtonClicked()
        end)
    else
        print("CloseButton is nil!")
    end
end

function LoginWidget:OnCancelButtonClicked()
    print("Cancel button clicked in Lua!")
    -- 可以在这里添加Lua逻辑
    -- 例如：关闭窗口、清理数据等
    self:RemoveFromParent()
end

function LoginWidget:OnCloseButtonClicked()
    print("Close button clicked in Lua!")
    -- 可以在这里添加Lua逻辑
    -- 例如：关闭窗口、清理数据等
    self:RemoveFromParent()
end

return Class(nil, nil, LoginWidget)

