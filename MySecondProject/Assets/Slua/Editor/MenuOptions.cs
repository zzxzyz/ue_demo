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
            
            // 保存状态到EditorPrefs以便持久化（Editor模式）
            EditorPrefs.SetBool(LUAPANDA_PREF_KEY, isLuaPandaEnabled);
            // 同时保存到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(LUAPANDA_PREF_KEY, isLuaPandaEnabled ? 1 : 0);
            PlayerPrefs.Save();
            
            Debug.Log($"LuaPanda: {(isLuaPandaEnabled ? "已启用" : "已禁用")}");
            // 在这里添加LuaPanda的具体功能代码
        }

        [MenuItem("选项/LuaPanda", true)]
        public static bool ValidateToggleLuaPanda()
        {
            // 从EditorPrefs加载状态
            isLuaPandaEnabled = EditorPrefs.GetBool(LUAPANDA_PREF_KEY, false);
            // 同步到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(LUAPANDA_PREF_KEY, isLuaPandaEnabled ? 1 : 0);
            // 设置菜单项的勾选状态
            Menu.SetChecked("选项/LuaPanda", isLuaPandaEnabled);
            return true;
        }

        /// <summary>
        /// 获取LuaPanda是否启用（运行时可用）
        /// </summary>
        public static bool IsLuaPandaEnabled()
        {
            // 优先从PlayerPrefs读取（运行时）
            if (PlayerPrefs.HasKey(LUAPANDA_PREF_KEY))
            {
                return PlayerPrefs.GetInt(LUAPANDA_PREF_KEY, 0) == 1;
            }
            // 如果PlayerPrefs中没有，尝试从EditorPrefs读取（Editor模式）
            #if UNITY_EDITOR
            return EditorPrefs.GetBool(LUAPANDA_PREF_KEY, false);
            #else
            return false;
            #endif
        }

        private static bool isAsyncLoginEnabled = true;
        private const string ASYNC_LOGIN_PREF_KEY = "SLua_AsyncLogin_Enabled";

        [MenuItem("选项/异步登录")]
        public static void ToggleAsyncLogin()
        {
            // 切换勾选状态
            isAsyncLoginEnabled = !isAsyncLoginEnabled;
            Menu.SetChecked("选项/异步登录", isAsyncLoginEnabled);
            
            // 保存状态到EditorPrefs以便持久化（Editor模式）
            EditorPrefs.SetBool(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled);
            // 同时保存到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled ? 1 : 0);
            PlayerPrefs.Save();
            
            Debug.Log($"异步登录: {(isAsyncLoginEnabled ? "已启用" : "已禁用")}");
        }

        [MenuItem("选项/异步登录", true)]
        public static bool ValidateToggleAsyncLogin()
        {
            // 从EditorPrefs加载状态（默认启用）
            isAsyncLoginEnabled = EditorPrefs.GetBool(ASYNC_LOGIN_PREF_KEY, true);
            // 同步到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled ? 1 : 0);
            // 设置菜单项的勾选状态
            Menu.SetChecked("选项/异步登录", isAsyncLoginEnabled);
            return true;
        }

        /// <summary>
        /// 获取异步登录是否启用（运行时可用）
        /// </summary>
        public static bool IsAsyncLoginEnabled()
        {
            // 优先从PlayerPrefs读取（运行时）
            if (PlayerPrefs.HasKey(ASYNC_LOGIN_PREF_KEY))
            {
                return PlayerPrefs.GetInt(ASYNC_LOGIN_PREF_KEY, 1) == 1;
            }
            // 如果PlayerPrefs中没有，尝试从EditorPrefs读取（Editor模式，默认启用）
            #if UNITY_EDITOR
            return EditorPrefs.GetBool(ASYNC_LOGIN_PREF_KEY, true);
            #else
            return true;
            #endif
        }
    }
}

