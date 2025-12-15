using UnityEngine;
using System.Collections.Generic;
using System.IO;

namespace SLua
{
    /// <summary>
    /// Lua 文件 AssetBundle 加载器
    /// 优先从 AssetBundle 加载，失败则回退到 Resources
    /// </summary>
    public static class LuaAssetBundleLoader
    {
        private static Dictionary<string, AssetBundle> loadedBundles = new Dictionary<string, AssetBundle>();
        private static AssetBundleManifest manifest = null;
        private static AssetBundle manifestBundle = null;
        private static string assetBundleBasePath = null;

        /// <summary>
        /// 初始化 AssetBundle 加载器
        /// </summary>
        /// <param name="basePath">AssetBundle 文件的基础路径（不包含文件名）</param>
        public static void Initialize(string basePath = null)
        {
            if (string.IsNullOrEmpty(basePath))
            {
                // 默认路径：StreamingAssets 或 Application.dataPath
#if UNITY_ANDROID && !UNITY_EDITOR
                assetBundleBasePath = Application.streamingAssetsPath;
#else
                assetBundleBasePath = Application.streamingAssetsPath;
                if (!Directory.Exists(assetBundleBasePath))
                {
                    // 如果 StreamingAssets 不存在，使用 Assets/AssetBundles（开发时）
                    assetBundleBasePath = Application.dataPath + "/AssetBundles";
                }
#endif
            }
            else
            {
                assetBundleBasePath = basePath;
            }

            Logger.Log($"[LuaAssetBundleLoader] 初始化，基础路径: {assetBundleBasePath}");

            // 加载主清单
            LoadManifest();
        }

        /// <summary>
        /// 加载主清单文件
        /// </summary>
        private static void LoadManifest()
        {
            try
            {
                string manifestPath = Path.Combine(assetBundleBasePath, "AssetBundles");
                
                if (File.Exists(manifestPath))
                {
                    manifestBundle = AssetBundle.LoadFromFile(manifestPath);
                    if (manifestBundle != null)
                    {
                        manifest = manifestBundle.LoadAsset<AssetBundleManifest>("AssetBundleManifest");
                        if (manifest != null)
                        {
                            Logger.Log("[LuaAssetBundleLoader] 主清单加载成功");
                        }
                        else
                        {
                            Logger.LogWarning("[LuaAssetBundleLoader] 无法从 AssetBundles 中加载 AssetBundleManifest");
                        }
                    }
                    else
                    {
                        Logger.LogWarning($"[LuaAssetBundleLoader] 无法加载清单文件: {manifestPath}");
                    }
                }
                else
                {
                    Logger.LogWarning($"[LuaAssetBundleLoader] 清单文件不存在: {manifestPath}");
                }
            }
            catch (System.Exception e)
            {
                Logger.LogError($"[LuaAssetBundleLoader] 加载清单失败: {e.Message}");
            }
        }

        /// <summary>
        /// 从 AssetBundle 加载 Lua 文件
        /// </summary>
        /// <param name="bundleName">AssetBundle 名称（例如 "login.logic"）</param>
        /// <param name="assetPath">资源在 AssetBundle 中的路径（例如 "Assets/Slua/Resources/business/login_ui.txt"）</param>
        /// <returns>TextAsset 或 null</returns>
        public static TextAsset LoadFromAssetBundle(string bundleName, string assetPath)
        {
            if (string.IsNullOrEmpty(bundleName) || string.IsNullOrEmpty(assetPath))
            {
                return null;
            }

            try
            {
                // 检查是否已加载
                if (!loadedBundles.ContainsKey(bundleName))
                {
                    string bundlePath = Path.Combine(assetBundleBasePath, bundleName);
                    
                    if (!File.Exists(bundlePath))
                    {
                        Logger.LogWarning($"[LuaAssetBundleLoader] AssetBundle 文件不存在: {bundlePath}");
                        return null;
                    }

                    AssetBundle bundle = AssetBundle.LoadFromFile(bundlePath);
                    if (bundle == null)
                    {
                        Logger.LogWarning($"[LuaAssetBundleLoader] 无法加载 AssetBundle: {bundlePath}");
                        return null;
                    }

                    loadedBundles[bundleName] = bundle;
                    Logger.Log($"[LuaAssetBundleLoader] 已加载 AssetBundle: {bundleName}");
                }

                AssetBundle targetBundle = loadedBundles[bundleName];
                if (targetBundle != null)
                {
                    // 尝试加载资源
                    TextAsset asset = targetBundle.LoadAsset<TextAsset>(assetPath);
                    if (asset != null)
                    {
                        Logger.Log($"[LuaAssetBundleLoader] 从 AssetBundle 加载成功: {bundleName}/{assetPath}");
                        return asset;
                    }
                    else
                    {
                        Logger.LogWarning($"[LuaAssetBundleLoader] AssetBundle 中未找到资源: {bundleName}/{assetPath}");
                    }
                }
            }
            catch (System.Exception e)
            {
                Logger.LogError($"[LuaAssetBundleLoader] 从 AssetBundle 加载失败: {e.Message}");
            }

            return null;
        }

        /// <summary>
        /// 尝试从 AssetBundle 加载 Lua 文件，失败则返回 null
        /// </summary>
        /// <param name="luaFileName">Lua 文件名（例如 "business.login_ui"）</param>
        /// <returns>TextAsset 或 null</returns>
        public static TextAsset TryLoadLuaFile(string luaFileName)
        {
            // 映射 Lua 文件名到 AssetBundle
            // 例如: "business.login_ui" -> "login.logic" bundle
            string bundleName = GetBundleNameForLuaFile(luaFileName);
            if (string.IsNullOrEmpty(bundleName))
            {
                return null;
            }

            // 构建资源路径（AssetBundle 中的完整路径）
            string assetPath = "Assets/Slua/Resources/" + luaFileName.Replace(".", "/") + ".txt";

            return LoadFromAssetBundle(bundleName, assetPath);
        }

        /// <summary>
        /// 根据 Lua 文件名获取对应的 AssetBundle 名称
        /// </summary>
        private static string GetBundleNameForLuaFile(string luaFileName)
        {
            // 这里可以根据你的命名规则映射
            // 例如: "business.login_ui" -> "login.logic"
            if (luaFileName == "business.login_ui")
            {
                return "login.logic";
            }

            // 可以添加更多映射规则
            // 或者从配置文件读取映射关系

            return null;
        }

        /// <summary>
        /// 清理所有已加载的 AssetBundle
        /// </summary>
        public static void UnloadAll()
        {
            foreach (var bundle in loadedBundles.Values)
            {
                if (bundle != null)
                {
                    bundle.Unload(false);
                }
            }
            loadedBundles.Clear();

            if (manifestBundle != null)
            {
                manifestBundle.Unload(false);
                manifestBundle = null;
            }

            manifest = null;
            Logger.Log("[LuaAssetBundleLoader] 已清理所有 AssetBundle");
        }
    }
}

