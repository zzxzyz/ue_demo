local LoginPanel = {}

-- 导入 UIHelper（C++ 蓝图函数库）
local UIHelper = import("UIHelper")

-- 导入 KismetSystemLibrary 用于屏幕调试信息
local KismetSystemLibrary = import("KismetSystemLibrary")

-- 设置输入模式为 UI 模式，显示鼠标光标
function LoginPanel:SetUIInputMode()
    local playerController = self:GetOwningPlayer()
    if playerController then
        UIHelper.SetInputModeGameAndUI(playerController, self)
        print("LoginPanel: Input mode set to GameAndUI")
    end
end

-- 恢复输入模式为游戏模式，隐藏鼠标光标
function LoginPanel:RestoreGameInputMode()
    local playerController = self:GetOwningPlayer()
    if playerController then
        UIHelper.SetInputModeGameOnly(playerController)
        print("LoginPanel: Input mode set to GameOnly")
    end
end

-- 初始化控件的通用函数
function LoginPanel:InitWidgets()
    print("LoginPanel:InitWidgets()")
    
    -- 使用 FindWidget 获取所有控件
    self.EmailTextBox = self:FindWidget("EmailTextBox")
    self.PasswordTextBox = self:FindWidget("PasswordTextBox")
    self.LoginButton = self:FindWidget("LoginButton")
    self.CancelButton = self:FindWidget("CancelButton")
    self.ErrorText = self:FindWidget("ErrorText")
    self.SwitchButton = self:FindWidget("SwitchButton")
    self.InfoButton = self:FindWidget("InfoButton")
    self.PasskeyButton = self:FindWidget("PasskeyButton")
    
    -- 检查控件是否成功获取
    if not self.EmailTextBox then
        print("Warning: EmailTextBox not found!")
    end
    if not self.PasswordTextBox then
        print("Warning: PasswordTextBox not found!")
    end
    if not self.LoginButton then
        print("Warning: LoginButton not found!")
    end
    if not self.CancelButton then
        print("Warning: CancelButton not found!")
    end
    if not self.ErrorText then
        print("Warning: ErrorText not found!")
    end
    if not self.SwitchButton then
        print("Warning: SwitchButton not found!")
    end
    if not self.InfoButton then
        print("Warning: InfoButton not found!")
    end
    if not self.PasskeyButton then
        print("Warning: PasskeyButton not found!")
    end
    
    -- 绑定登录按钮点击事件
    if self.LoginButton then
        print("Binding LoginButton OnClicked in Lua")
        self.LoginButton.OnClicked:Clear()
        self.LoginButton.OnClicked:Add(function()
            print("LoginButton OnClicked callback called in Lua")
            self:OnLoginButtonClicked()
        end)
        print("LoginButton OnClicked bound successfully")
    end
    
    -- 绑定取消按钮点击事件
    if self.CancelButton then
        print("Binding CancelButton OnClicked in Lua")
        self.CancelButton.OnClicked:Clear()
        self.CancelButton.OnClicked:Add(function()
            print("CancelButton OnClicked callback called in Lua")
            self:OnCancelButtonClicked()
        end)
        print("CancelButton OnClicked bound successfully")
    end

    if self.CloseButton then
        print("Binding CloseButton OnClicked in Lua")
        self.CloseButton.OnClicked:Clear()
        self.CloseButton.OnClicked:Add(function()
            print("CloseButton OnClicked callback called in Lua")
            self:OnCloseButtonClicked()
        end)
        print("CloseButton OnClicked bound successfully")
    end
    
    -- 绑定切换按钮点击事件
    if self.SwitchButton then
        print("Binding SwitchButton OnClicked in Lua")
        self.SwitchButton.OnClicked:Clear()
        self.SwitchButton.OnClicked:Add(function()
            print("SwitchButton OnClicked callback called in Lua")
            self:OnSwitchButtonClicked()
        end)
        print("SwitchButton OnClicked bound successfully")
    end
    
    -- 绑定信息按钮点击事件
    if self.InfoButton then
        print("Binding InfoButton OnClicked in Lua")
        self.InfoButton.OnClicked:Clear()
        self.InfoButton.OnClicked:Add(function()
            print("InfoButton OnClicked callback called in Lua")
            self:OnInfoButtonClicked()
        end)
        print("InfoButton OnClicked bound successfully")
    end
    
    -- 绑定 Passkey 按钮点击事件
    if self.PasskeyButton then
        print("Binding PasskeyButton OnClicked in Lua")
        self.PasskeyButton.OnClicked:Clear()
        self.PasskeyButton.OnClicked:Add(function()
            print("PasskeyButton OnClicked callback called in Lua")
            self:OnPasskeyButtonClicked()
        end)
        print("PasskeyButton OnClicked bound successfully")
    end
    
    -- 设置密码输入框为密码模式
    if self.PasswordTextBox then
        self.PasswordTextBox:SetIsPassword(true)
    end
    
    -- 初始化时清除错误信息
    self:ClearError()
end

-- Initialize 函数：LuaUserWidget 会调用此函数
function LoginPanel:Initialize()
    print("LoginPanel:Initialize in Lua!")
    
    -- 设置输入模式为 UI 模式，显示鼠标光标
    self:SetUIInputMode()
    
    -- 调用初始化控件的函数
    self:InitWidgets()
end

-- NativeConstruct 函数：继承自 UUserWidget 并重写 NativeConstruct 的类会调用此函数
function LoginPanel:NativeConstruct()
    -- 注意：不需要调用 Super:NativeConstruct()，因为 C++ 已经调用了父类的 NativeConstruct
    print("LoginPanel:NativeConstruct in Lua!")
    
    -- 设置输入模式为 UI 模式，显示鼠标光标
    self:SetUIInputMode()
    
    -- 调用初始化控件的函数
    self:InitWidgets()
end

-- NativeDestruct 函数：Widget 被销毁时调用
function LoginPanel:NativeDestruct()
    print("LoginPanel:NativeDestruct in Lua!")
    
    -- 恢复输入模式为游戏模式
    self:RestoreGameInputMode()
end

function LoginPanel:OnLoginButtonClicked()
    if not self.EmailTextBox or not self.PasswordTextBox then
        return
    end
    
    local email = self.EmailTextBox:GetText()
    local password = self.PasswordTextBox:GetText()
    
    -- 清除之前的错误信息
    self:ClearError()
    
    -- 验证邮箱
    if email == "" or email == nil then
        self:ShowError("请输入邮箱地址")
        return
    end
    
    if not self:IsValidEmail(email) then
        self:ShowError("邮箱格式不正确")
        return
    end
    
    -- 验证密码
    if password == "" or password == nil then
        self:ShowError("请输入密码")
        return
    end
    
    if not self:IsValidPassword(password) then
        self:ShowError("密码长度至少为6位")
        return
    end
    
    -- 这里可以添加实际的登录逻辑
    -- 例如：调用服务器API、验证用户凭据等
    print("Login attempt - Email: " .. email .. ", Password: " .. password)
    
    -- 登录成功后的处理（示例）
    self:HandleLoginSuccess()
end

function LoginPanel:HandleLoginSuccess()
    -- 成功后可以隐藏或销毁登录界面
    self:ClearError()
    
    print("Login success, closing login widget.")
    -- 可在此触发后续事件，如通知控制器切换关卡、加载主菜单等
    self:RemoveFromParent()
end

function LoginPanel:IsValidEmail(email)
    -- 简单的邮箱格式验证
    -- 检查是否包含@符号和点号
    local atIndex = string.find(email, "@")
    if not atIndex or atIndex == 1 then
        return false
    end
    
    -- 从@符号之后查找点号
    local dotIndex = string.find(email, "%.", atIndex)
    if not dotIndex or dotIndex == atIndex + 1 then
        return false
    end
    
    -- 确保点号在@符号之后
    if dotIndex <= atIndex then
        return false
    end
    
    return true
end

function LoginPanel:IsValidPassword(password)
    -- 密码至少6位
    return string.len(password) >= 6
end

function LoginPanel:ShowError(errorMessage)
    if self.ErrorText then
        self.ErrorText:SetText(errorMessage)
        -- 设置可见性为可见（ESlateVisibility::Visible = 0）
        local visibility = (UEnums and UEnums.ESlateVisibility and UEnums.ESlateVisibility.Visible) or 0
        self.ErrorText:SetVisibility(visibility)
    end
end

function LoginPanel:ClearError()
    if self.ErrorText then
        self.ErrorText:SetText("")
        -- 设置可见性为折叠（ESlateVisibility::Collapsed = 1）
        local visibility = (UEnums and UEnums.ESlateVisibility and UEnums.ESlateVisibility.Collapsed) or 1
        self.ErrorText:SetVisibility(visibility)
    end
end

function LoginPanel:OnCancelButtonClicked()
    print("=== Cancel button clicked in Lua! ===")
    print("self type: " .. type(self))
    print("RemoveFromParent exists: " .. tostring(self.RemoveFromParent ~= nil))
    
    -- 可以在这里添加Lua逻辑
    -- 例如：关闭窗口、清理数据等
    if self.RemoveFromParent then
        print("Calling RemoveFromParent...")
        self:RemoveFromParent()
        print("RemoveFromParent called successfully")
    else
        print("ERROR: RemoveFromParent function not found!")
    end
end

function LoginPanel:OnCloseButtonClicked()
    print("Close button clicked in Lua!")
    -- 可以在这里添加Lua逻辑
    -- 例如：关闭窗口、清理数据等
    if self.RemoveFromParent then
        print("Calling RemoveFromParent...")
        self:RemoveFromParent()
        print("RemoveFromParent called successfully")
    else
        print("ERROR: RemoveFromParent function not found!")
    end
end

-- SwitchButton 点击处理函数：切换窗口模式
function LoginPanel:OnSwitchButtonClicked()
    print("=== SwitchButton clicked in Lua! ===")
    UIHelper.ToggleWindowMode()
end

-- InfoButton 点击处理函数：显示当前窗口模式信息
function LoginPanel:OnInfoButtonClicked()
    print("=== InfoButton clicked in Lua! ===")
    UIHelper.PrintWindowMode()
end

-- PasskeyButton 点击处理函数
function LoginPanel:OnPasskeyButtonClicked()
    print("=== PasskeyButton clicked in Lua! ===")
    -- TODO: 添加 Passkey 认证逻辑
    local success = UIHelper.PerformPasskeyAuthenticationWithFullScreen()
    if success then
        print("Passkey authentication successful!")
    else
        print("Passkey authentication failed!")
    end
end

return Class(nil, nil, LoginPanel)

