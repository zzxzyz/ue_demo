using UnityEngine;
using UnityEditor;

namespace SLua
{
    public class MenuOptions
    {
        private static bool isLuaPandaEnabled = false;
        private const string LUAPANDA_PREF_KEY = "SLua_LuaPanda_Enabled";

        [MenuItem("选项/LuaPanda")]
        public static void ToggleLuaPanda()
        {
            // 切换勾选状态
            isLuaPandaEnabled = !isLuaPandaEnabled;
            Menu.SetChecked("选项/LuaPanda", isLuaPandaEnabled);
            
            // 保存状态到EditorPrefs以便持久化
            EditorPrefs.SetBool(LUAPANDA_PREF_KEY, isLuaPandaEnabled);
            
            Debug.Log($"LuaPanda: {(isLuaPandaEnabled ? "已启用" : "已禁用")}");
            // 在这里添加LuaPanda的具体功能代码
        }

        [MenuItem("选项/LuaPanda", true)]
        public static bool ValidateToggleLuaPanda()
        {
            // 从EditorPrefs加载状态
            isLuaPandaEnabled = EditorPrefs.GetBool(LUAPANDA_PREF_KEY, false);
            // 设置菜单项的勾选状态
            Menu.SetChecked("选项/LuaPanda", isLuaPandaEnabled);
            return true;
        }

        [MenuItem("选项/异步登录")]
        public static void OpenAsyncLogin()
        {
            Debug.Log("打开异步登录功能");
            // 在这里添加异步登录的具体功能代码
        }
    }
}

