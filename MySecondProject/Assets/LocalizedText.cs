using UnityEngine;
using UnityEngine.UI;
using TMPro;

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
            // 获取文本值
            string value = LocalizationManager.Instance.GetLocalizedValue(key);
            
            // 优先尝试 TextMeshProUGUI（UI Canvas 用）
            TextMeshProUGUI tmpText = GetComponent<TextMeshProUGUI>();
            if (tmpText != null)
            {
                tmpText.text = value;
                return;
            }
            
            // 尝试 TextMeshPro（3D 世界空间用）
            TextMeshPro tmpText3D = GetComponent<TextMeshPro>();
            if (tmpText3D != null)
            {
                tmpText3D.text = value;
                return;
            }
            
            // 最后尝试传统的 Text 组件
            Text textComponent = GetComponent<Text>();
            if (textComponent != null)
            {
                textComponent.text = value;
                return;
            }
            
            // 如果都没有找到，输出警告
            Debug.LogWarning($"LocalizedText on {gameObject.name} 未找到 Text、TextMeshProUGUI 或 TextMeshPro 组件！", this);
        }
    }
}