using UnityEngine;
using SLua;

public class LuaStarter : MonoBehaviour
{
    private LuaSvr luaSvr;
    
    void Start()
    {
        // 初始化Lua虚拟机
        luaSvr = new LuaSvr();
        
        // 初始化slua，使用基本模式 + 扩展库（包含 socket）
        luaSvr.init(null, OnLuaInitComplete, LuaSvrFlag.LSF_BASIC | LuaSvrFlag.LSF_EXTLIB);
    }
    
    void OnLuaInitComplete()
    {
        // 方式1：直接执行Lua代码字符串
        //LuaSvr.mainState.doString("print('hello world')");

        //方式2：加载并执行Lua脚本文件（hello.txt位于Assets / Slua / Resources目录）
        luaSvr.start("hello");
        luaSvr.start("business.login_ui");

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
    }
}

