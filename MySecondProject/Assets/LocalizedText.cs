using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Text))]
[ExecuteInEditMode] // 1. 允许在编辑器模式运行
public class LocalizedText : MonoBehaviour
{
    public string key;

    void Start()
    {
        UpdateText();
    }

    // 2. 当你在 Inspector 修改 "Key" 字段时，自动触发刷新
    void OnValidate()
    {
        UpdateText();
    }

    public void UpdateText()
    {
        // 安全检查：防止在编辑器里删掉了 Manager 报错
        if (LocalizationManager.Instance == null) 
        {
            // 尝试在场景里找一下 (应对 Editor 模式下的单例丢失问题)
            LocalizationManager.Instance = FindObjectOfType<LocalizationManager>();
        }

        if (LocalizationManager.Instance != null)
        {
            Text textComponent = GetComponent<Text>();
            // 获取文本并赋值
            string value = LocalizationManager.Instance.GetLocalizedValue(key);
            textComponent.text = value;
        }
    }
}