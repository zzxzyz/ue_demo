using UnityEngine;
using UnityEditor;
using System.IO;

namespace SLua
{
    public class MenuOptions
    {
        private static bool isLuaPandaEnabled = false;
        private const string LUAPANDA_PREF_KEY = "SLua_LuaPanda_Enabled";

        [MenuItem("应用扩展/LuaPanda")]
        public static void ToggleLuaPanda()
        {
            // 切换勾选状态
            isLuaPandaEnabled = !isLuaPandaEnabled;
            Menu.SetChecked("应用扩展/LuaPanda", isLuaPandaEnabled);
            
            // 保存状态到EditorPrefs以便持久化（Editor模式）
            EditorPrefs.SetBool(LUAPANDA_PREF_KEY, isLuaPandaEnabled);
            // 同时保存到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(LUAPANDA_PREF_KEY, isLuaPandaEnabled ? 1 : 0);
            PlayerPrefs.Save();
            
            Debug.Log($"LuaPanda: {(isLuaPandaEnabled ? "已启用" : "已禁用")}");
            // 在这里添加LuaPanda的具体功能代码
        }

        [MenuItem("应用扩展/LuaPanda", true)]
        public static bool ValidateToggleLuaPanda()
        {
            // 从EditorPrefs加载状态
            isLuaPandaEnabled = EditorPrefs.GetBool(LUAPANDA_PREF_KEY, false);
            // 同步到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(LUAPANDA_PREF_KEY, isLuaPandaEnabled ? 1 : 0);
            // 设置菜单项的勾选状态
            Menu.SetChecked("应用扩展/LuaPanda", isLuaPandaEnabled);
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

        [MenuItem("应用扩展/异步登录")]
        public static void ToggleAsyncLogin()
        {
            // 切换勾选状态
            isAsyncLoginEnabled = !isAsyncLoginEnabled;
            Menu.SetChecked("应用扩展/异步登录", isAsyncLoginEnabled);
            
            // 保存状态到EditorPrefs以便持久化（Editor模式）
            EditorPrefs.SetBool(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled);
            // 同时保存到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled ? 1 : 0);
            PlayerPrefs.Save();
            
            Debug.Log($"异步登录: {(isAsyncLoginEnabled ? "已启用" : "已禁用")}");
        }

        [MenuItem("应用扩展/异步登录", true)]
        public static bool ValidateToggleAsyncLogin()
        {
            // 从EditorPrefs加载状态（默认启用）
            isAsyncLoginEnabled = EditorPrefs.GetBool(ASYNC_LOGIN_PREF_KEY, true);
            // 同步到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(ASYNC_LOGIN_PREF_KEY, isAsyncLoginEnabled ? 1 : 0);
            // 设置菜单项的勾选状态
            Menu.SetChecked("应用扩展/异步登录", isAsyncLoginEnabled);
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

        // ========== Lua 加载模式设置 ==========
        public enum LuaLoadMode
        {
            Local = 0,      // 从 Resources 加载
            AssetBundle = 1 // 从 AssetBundle 加载
        }

        private const string LUA_LOAD_MODE_PREF_KEY = "SLua_LuaLoadMode";
        private static LuaLoadMode currentLoadMode = LuaLoadMode.Local;

        [MenuItem("应用扩展/加载模式/本地")]
        public static void SetLoadModeLocal()
        {
            SetLoadMode(LuaLoadMode.Local);
        }

        [MenuItem("应用扩展/加载模式/本地", true)]
        public static bool ValidateSetLoadModeLocal()
        {
            currentLoadMode = GetLoadMode();
            Menu.SetChecked("应用扩展/加载模式/本地", currentLoadMode == LuaLoadMode.Local);
            return true;
        }

        [MenuItem("应用扩展/加载模式/AssetBundle")]
        public static void SetLoadModeAssetBundle()
        {
            SetLoadMode(LuaLoadMode.AssetBundle);
        }

        [MenuItem("应用扩展/加载模式/AssetBundle", true)]
        public static bool ValidateSetLoadModeAssetBundle()
        {
            currentLoadMode = GetLoadMode();
            Menu.SetChecked("应用扩展/加载模式/AssetBundle", currentLoadMode == LuaLoadMode.AssetBundle);
            return true;
        }

        private static void SetLoadMode(LuaLoadMode mode)
        {
            currentLoadMode = mode;
            
            // 更新菜单勾选状态
            Menu.SetChecked("应用扩展/加载模式/本地", mode == LuaLoadMode.Local);
            Menu.SetChecked("应用扩展/加载模式/AssetBundle", mode == LuaLoadMode.AssetBundle);
            
            // 保存状态到EditorPrefs以便持久化（Editor模式）
            EditorPrefs.SetInt(LUA_LOAD_MODE_PREF_KEY, (int)mode);
            // 同时保存到PlayerPrefs以便运行时访问
            PlayerPrefs.SetInt(LUA_LOAD_MODE_PREF_KEY, (int)mode);
            PlayerPrefs.Save();
            
            string modeName = mode == LuaLoadMode.Local ? "本地" : "AssetBundle";
            Debug.Log($"Lua 加载模式已设置为: {modeName}");
        }

        /// <summary>
        /// 获取当前加载模式（运行时可用）
        /// </summary>
        public static LuaLoadMode GetLoadMode()
        {
            // 优先从PlayerPrefs读取（运行时）
            if (PlayerPrefs.HasKey(LUA_LOAD_MODE_PREF_KEY))
            {
                return (LuaLoadMode)PlayerPrefs.GetInt(LUA_LOAD_MODE_PREF_KEY, (int)LuaLoadMode.Local);
            }
            // 如果PlayerPrefs中没有，尝试从EditorPrefs读取（Editor模式，默认本地）
            #if UNITY_EDITOR
            return (LuaLoadMode)EditorPrefs.GetInt(LUA_LOAD_MODE_PREF_KEY, (int)LuaLoadMode.Local);
            #else
            return LuaLoadMode.Local;
            #endif
        }

        // ========== AssetBundle 打包功能 ==========
        [MenuItem("应用扩展/打包 AssetBundles")]
        public static void BuildAllAssetBundles()
        {
            // 创建输出目录
            string outputPath = "Assets/AssetBundles";
            if (!Directory.Exists(outputPath))
            {
                Directory.CreateDirectory(outputPath);
            }
            
            // 打包AssetBundles
            BuildPipeline.BuildAssetBundles(
                outputPath,
                BuildAssetBundleOptions.None,
                EditorUserBuildSettings.activeBuildTarget
            );
            
            Debug.Log("AssetBundle打包完成！路径: " + outputPath);
        }
    }
}

