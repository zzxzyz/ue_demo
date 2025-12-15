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
        
        // 自动映射表：Lua文件名 -> AssetBundle名称
        // 例如: "hello" -> "login.logic", "business.login_ui" -> "login.logic"
        private static Dictionary<string, string> luaFileToBundleMap = new Dictionary<string, string>();

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
            
            // 自动扫描并建立映射表
            BuildAutoMapping();
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
            // 检查加载模式设置
            if (!ShouldLoadFromAssetBundle())
            {
                return null;
            }

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
        /// 检查是否应该从 AssetBundle 加载
        /// </summary>
        private static bool ShouldLoadFromAssetBundle()
        {
#if UNITY_EDITOR
            // Editor 模式下从 EditorPrefs 读取
            int mode = UnityEditor.EditorPrefs.GetInt("SLua_LuaLoadMode", 0);
            return mode == 1; // 1 = AssetBundle
#else
            // 运行时从 PlayerPrefs 读取
            int mode = PlayerPrefs.GetInt("SLua_LuaLoadMode", 0);
            return mode == 1; // 1 = AssetBundle
#endif
        }

        /// <summary>
        /// 自动扫描所有 AssetBundle 的 manifest，建立 Lua 文件到 Bundle 的映射
        /// </summary>
        private static void BuildAutoMapping()
        {
            luaFileToBundleMap.Clear();

            if (manifest == null)
            {
                Logger.LogWarning("[LuaAssetBundleLoader] 主清单未加载，无法建立自动映射");
                return;
            }

            try
            {
                // 获取所有 AssetBundle 名称
                string[] allBundles = manifest.GetAllAssetBundles();
                
                foreach (string bundleName in allBundles)
                {
                    // 读取每个 Bundle 的 manifest 文件
                    string manifestPath = Path.Combine(assetBundleBasePath, bundleName + ".manifest");
                    
                    if (File.Exists(manifestPath))
                    {
                        ParseBundleManifest(manifestPath, bundleName);
                    }
                    else
                    {
                        Logger.LogWarning($"[LuaAssetBundleLoader] 未找到 manifest 文件: {manifestPath}");
                    }
                }

                Logger.Log($"[LuaAssetBundleLoader] 自动映射完成，共映射 {luaFileToBundleMap.Count} 个 Lua 文件");
            }
            catch (System.Exception e)
            {
                Logger.LogError($"[LuaAssetBundleLoader] 建立自动映射失败: {e.Message}");
            }
        }

        /// <summary>
        /// 解析单个 AssetBundle 的 manifest 文件
        /// </summary>
        private static void ParseBundleManifest(string manifestPath, string bundleName)
        {
            try
            {
                string[] lines = File.ReadAllLines(manifestPath);
                bool inAssetsSection = false;
                
                foreach (string line in lines)
                {
                    string trimmedLine = line.Trim();
                    
                    // 检测 Assets 部分开始
                    if (trimmedLine == "Assets:")
                    {
                        inAssetsSection = true;
                        continue;
                    }
                    
                    // 检测下一个主要部分（结束 Assets 部分）
                    if (inAssetsSection && trimmedLine.StartsWith("-") == false && 
                        trimmedLine.Length > 0 && !trimmedLine.StartsWith(" "))
                    {
                        inAssetsSection = false;
                        continue;
                    }
                    
                    // 解析 Assets 列表
                    if (inAssetsSection && trimmedLine.StartsWith("- Assets/Slua/Resources/"))
                    {
                        // 提取资源路径，例如: "- Assets/Slua/Resources/hello.txt"
                        string assetPath = trimmedLine.Substring(2).Trim(); // 去掉 "- " 前缀
                        
                        // 转换为 Lua 文件名
                        // "Assets/Slua/Resources/hello.txt" -> "hello"
                        // "Assets/Slua/Resources/business/login_ui.txt" -> "business.login_ui"
                        string luaFileName = ConvertAssetPathToLuaFileName(assetPath);
                        
                        if (!string.IsNullOrEmpty(luaFileName))
                        {
                            // 如果已存在映射，记录警告（可能有冲突）
                            if (luaFileToBundleMap.ContainsKey(luaFileName))
                            {
                                Logger.LogWarning($"[LuaAssetBundleLoader] Lua 文件 '{luaFileName}' 在多个 Bundle 中存在: {luaFileToBundleMap[luaFileName]} 和 {bundleName}");
                            }
                            
                            luaFileToBundleMap[luaFileName] = bundleName;
                            Logger.Log($"[LuaAssetBundleLoader] 映射: {luaFileName} -> {bundleName}");
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                Logger.LogError($"[LuaAssetBundleLoader] 解析 manifest 失败 {manifestPath}: {e.Message}");
            }
        }

        /// <summary>
        /// 将资源路径转换为 Lua 文件名
        /// </summary>
        /// <param name="assetPath">例如: "Assets/Slua/Resources/hello.txt" 或 "Assets/Slua/Resources/business/login_ui.txt"</param>
        /// <returns>例如: "hello" 或 "business.login_ui"</returns>
        private static string ConvertAssetPathToLuaFileName(string assetPath)
        {
            const string resourcesPrefix = "Assets/Slua/Resources/";
            const string txtExtension = ".txt";
            
            if (!assetPath.StartsWith(resourcesPrefix) || !assetPath.EndsWith(txtExtension))
            {
                return null;
            }
            
            // 提取相对路径部分
            // "Assets/Slua/Resources/hello.txt" -> "hello.txt"
            // "Assets/Slua/Resources/business/login_ui.txt" -> "business/login_ui.txt"
            string relativePath = assetPath.Substring(resourcesPrefix.Length);
            
            // 去掉 .txt 扩展名
            // "hello.txt" -> "hello"
            // "business/login_ui.txt" -> "business/login_ui"
            string withoutExtension = relativePath.Substring(0, relativePath.Length - txtExtension.Length);
            
            // 将路径分隔符转换为点号
            // "hello" -> "hello"
            // "business/login_ui" -> "business.login_ui"
            string luaFileName = withoutExtension.Replace("/", ".");
            
            return luaFileName;
        }

        /// <summary>
        /// 根据 Lua 文件名获取对应的 AssetBundle 名称（自动映射）
        /// </summary>
        private static string GetBundleNameForLuaFile(string luaFileName)
        {
            // 优先使用自动映射表
            if (luaFileToBundleMap.ContainsKey(luaFileName))
            {
                return luaFileToBundleMap[luaFileName];
            }

            // 如果没有自动映射，返回 null（将回退到 Resources 加载）
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

