using UnityEngine;
using SLua;

public class LuaStarter : MonoBehaviour
{
    private LuaSvr luaSvr;
    
    void Start()
    {
        try
        {
            // 初始化 AssetBundle 加载器（优先从 AssetBundle 加载）
            SLua.LuaAssetBundleLoader.Initialize();
            
            // 初始化Lua虚拟机
            luaSvr = new LuaSvr();
            
            // 初始化slua，使用基本模式 + 扩展库（包含 socket）
            luaSvr.init(null, OnLuaInitComplete, LuaSvrFlag.LSF_BASIC | LuaSvrFlag.LSF_EXTLIB);

            Debug.LogWarning($"current screen mode is {Screen.fullScreenMode}");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[LuaStarter] 初始化失败: {e.Message}\n{e.StackTrace}");
        }
    }
    
    void OnLuaInitComplete()
    {
        // 方式1：直接执行Lua代码字符串
        //LuaSvr.mainState.doString("print('hello world')");

        // 检查LuaPanda是否启用，并传递给Lua
        // 从PlayerPrefs读取设置（MenuOptions已经将设置保存到PlayerPrefs）
        bool luaPandaEnabled = PlayerPrefs.GetInt("SLua_LuaPanda_Enabled", 0) == 1;
        
        // 将LuaPanda启用状态传递给Lua全局变量
        LuaSvr.mainState.doString($"_G._LuaPandaEnabled = {(luaPandaEnabled ? "true" : "false")}");
        
        // 检查异步登录是否启用，并传递给Lua
        // 从PlayerPrefs读取设置（MenuOptions已经将设置保存到PlayerPrefs，默认启用）
        bool asyncLoginEnabled = PlayerPrefs.GetInt("SLua_AsyncLogin_Enabled", 1) == 1;
        
        // 将异步登录启用状态传递给Lua全局变量
        LuaSvr.mainState.doString($"_G._AsyncLoginEnabled = {(asyncLoginEnabled ? "true" : "false")}");

        //方式2：加载并执行Lua脚本文件（hello.txt位于Assets / Slua / Resources目录）
        luaSvr.start("hello");
        luaSvr.start("business.login_ui");
        luaSvr.start("world");

        // 初始化C#和Lua之间的桥接
        LoginBridge.Instance.InitializeBridge();

        Debug.Log("Lua脚本已执行，C#和Lua桥接已初始化");
    }
    
    void OnDestroy()
    {
        // 清理资源
        if (luaSvr != null)
        {
            luaSvr = null;
        }
        
        // 清理 AssetBundle 资源
        SLua.LuaAssetBundleLoader.UnloadAll();
    }
}

