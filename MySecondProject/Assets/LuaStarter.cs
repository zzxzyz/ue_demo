using UnityEngine;
using SLua;

public class LuaStarter : MonoBehaviour
{
    private LuaSvr luaSvr;
    
    void Start()
    {
        // 初始化Lua虚拟机
        luaSvr = new LuaSvr();
        
        // 初始化slua，使用基本模式
        luaSvr.init(null, OnLuaInitComplete, LuaSvrFlag.LSF_BASIC);
    }
    
    void OnLuaInitComplete()
    {
        // 方式1：直接执行Lua代码字符串
        //LuaSvr.mainState.doString("print('hello world')");

        //方式2：加载并执行Lua脚本文件（hello.txt位于Assets / Slua / Resources目录）
        luaSvr.start("hello");

        Debug.Log("Lua脚本已执行，已打印hello world");
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

