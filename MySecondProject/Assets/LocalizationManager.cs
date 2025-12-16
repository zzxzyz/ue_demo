using System.Collections.Generic;
using UnityEngine;
using System.IO;
using SLua;

[System.Serializable]
public class LocalizationData
{
    public List<LocalizationItem> items;
}

[System.Serializable]
public class LocalizationItem
{
    public string key;
    public string value;
}

// 1. 允许在编辑器模式运行
// 2. 添加 CustomLuaClass 属性，让 Lua 可以访问这个类
[CustomLuaClass]
[ExecuteInEditMode] 
public class LocalizationManager : MonoBehaviour
{
    public static LocalizationManager Instance;

    // 2. 在 Inspector 面板暴露这个变量，用于切换预览语言（使用 BCP 47 标准：zh-CN, en-US）
    [Header("Editor Preview")]
    public string previewLanguage = "zh-CN"; 

    private Dictionary<string, string> localizedText;
    private bool isReady = false;

    void OnEnable() // 使用 OnEnable 替代 Awake，因为在 Editor 模式下编译代码后会重新 Enable
    {
        if (Instance == null) Instance = this;
        LoadLocalizedText(previewLanguage);
    }

    // 3. 当你在 Inspector 改变 previewLanguage 时，Unity 会自动调用这个方法
    void OnValidate()
    {
        // 只有在非运行模式下，修改语言才立即刷新，避免运行时的不必要开销
        if (!Application.isPlaying) 
        {
             // 重新加载语言并刷新
            LoadLocalizedText(previewLanguage);
        }
    }

    /// <summary>
    /// 加载本地化文本（统一使用 BCP 47 标准语言代码，如：zh-CN, en-US）
    /// Lua 和 Unity 使用相同的语言编码格式，无需映射转换
    /// </summary>
    /// <param name="langCode">BCP 47 标准语言代码（如：zh-CN, en-US, ja-JP, ko-KR）</param>
    public void LoadLocalizedText(string langCode)
    {
        // 直接使用 BCP 47 标准格式作为文件名，无需映射
        // JSON 文件名格式：zh-CN.json, en-US.json 等
        string filePath = Path.Combine(Application.streamingAssetsPath, langCode + ".json");

        if (File.Exists(filePath))
        {
            string dataAsJson = File.ReadAllText(filePath);
            LocalizationData loadedData = JsonUtility.FromJson<LocalizationData>(dataAsJson);

            localizedText = new Dictionary<string, string>();
            for (int i = 0; i < loadedData.items.Count; i++)
            {
                // 防止重复 Key 报错
                if (!localizedText.ContainsKey(loadedData.items[i].key))
                {
                    localizedText.Add(loadedData.items[i].key, loadedData.items[i].value);
                }
            }

            isReady = true;
            // 加载完数据后，立即通知所有文本刷新
            UpdateAllTexts();
        }
        else
        {
            Debug.LogWarning($"[LocalizationManager] 找不到语言文件: {filePath}");
        }
    }

    public string GetLocalizedValue(string key)
    {
        if (localizedText == null || !localizedText.ContainsKey(key))
        {
            return key; // 找不到 key 就直接显示 key 本身
        }
        return localizedText[key];
    }

    public void UpdateAllTexts()
    {
        // 查找场景中所有的 LocalizedText 并强制刷新
        LocalizedText[] allTexts = FindObjectsOfType<LocalizedText>();
        foreach (LocalizedText text in allTexts)
        {
            text.UpdateText();
        }
    }
}