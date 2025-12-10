using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// UI 管理器 - 单例模式
/// 用于管理跨场景的 UI 元素，如登录对话框、设置面板等
/// </summary>
public class UIManager : MonoBehaviour
{
    private static UIManager _instance;
    
    [Header("UI Prefabs")]
    [Tooltip("登录面板预制体")]
    public GameObject loginPanelPrefab;
    
    [Tooltip("设置面板预制体（可选）")]
    public GameObject settingsPanelPrefab;
    
    [Header("UI Canvas Settings")]
    [Tooltip("UI Canvas 根对象")]
    public Canvas uiCanvas;
    
    // 存储已创建的 UI 实例
    private Dictionary<string, GameObject> uiInstances = new Dictionary<string, GameObject>();
    
    public static UIManager Instance
    {
        get
        {
            if (_instance == null)
            {
                // 尝试查找已存在的实例
                _instance = FindObjectOfType<UIManager>();
                
                if (_instance == null)
                {
                    // 创建新的 UIManager
                    GameObject go = new GameObject("UIManager");
                    _instance = go.AddComponent<UIManager>();
                    DontDestroyOnLoad(go);
                    
                    // 创建 UI Canvas
                    _instance.CreateUICanvas();
                }
            }
            return _instance;
        }
    }
    
    void Awake()
    {
        // 确保只有一个实例
        if (_instance == null)
        {
            _instance = this;
            DontDestroyOnLoad(gameObject);
            CreateUICanvas();
        }
        else if (_instance != this)
        {
            // 如果已存在实例，销毁当前对象
            Destroy(gameObject);
        }
    }
    
    /// <summary>
    /// 创建独立的 UI Canvas
    /// </summary>
    void CreateUICanvas()
    {
        if (uiCanvas == null)
        {
            // 创建 Canvas GameObject
            GameObject canvasGO = new GameObject("PersistentUICanvas");
            canvasGO.transform.SetParent(transform);
            
            // 添加 Canvas 组件
            uiCanvas = canvasGO.AddComponent<Canvas>();
            uiCanvas.renderMode = RenderMode.ScreenSpaceOverlay;
            uiCanvas.sortingOrder = 100; // 设置较高的排序顺序，确保在其他 UI 之上
            
            // 添加 CanvasScaler 组件（可选，用于适配不同分辨率）
            UnityEngine.UI.CanvasScaler scaler = canvasGO.AddComponent<UnityEngine.UI.CanvasScaler>();
            scaler.uiScaleMode = UnityEngine.UI.CanvasScaler.ScaleMode.ScaleWithScreenSize;
            scaler.referenceResolution = new Vector2(1920, 1080);
            scaler.matchWidthOrHeight = 0.5f;
            
            // 添加 GraphicRaycaster 组件（用于 UI 交互）
            canvasGO.AddComponent<UnityEngine.UI.GraphicRaycaster>();
            
            Debug.Log("UIManager: 创建了独立的 UI Canvas");
        }
    }
    
    /// <summary>
    /// 显示登录面板
    /// </summary>
    public void ShowLoginPanel()
    {
        ShowUI("LoginPanel", loginPanelPrefab);
    }
    
    /// <summary>
    /// 隐藏登录面板
    /// </summary>
    public void HideLoginPanel()
    {
        HideUI("LoginPanel");
    }
    
    /// <summary>
    /// 显示指定的 UI
    /// </summary>
    /// <param name="uiName">UI 名称</param>
    /// <param name="prefab">UI 预制体</param>
    public void ShowUI(string uiName, GameObject prefab)
    {
        if (prefab == null)
        {
            Debug.LogError($"UIManager: {uiName} 的预制体未设置！");
            return;
        }
        
        // 如果 UI 已存在，直接显示
        if (uiInstances.ContainsKey(uiName) && uiInstances[uiName] != null)
        {
            uiInstances[uiName].SetActive(true);
            return;
        }
        
        // 创建新的 UI 实例
        GameObject uiInstance = Instantiate(prefab, uiCanvas.transform);
        uiInstance.name = uiName;
        uiInstances[uiName] = uiInstance;
        
        Debug.Log($"UIManager: 显示 {uiName}");
    }
    
    /// <summary>
    /// 隐藏指定的 UI
    /// </summary>
    /// <param name="uiName">UI 名称</param>
    public void HideUI(string uiName)
    {
        if (uiInstances.ContainsKey(uiName) && uiInstances[uiName] != null)
        {
            uiInstances[uiName].SetActive(false);
            Debug.Log($"UIManager: 隐藏 {uiName}");
        }
    }
    
    /// <summary>
    /// 销毁指定的 UI
    /// </summary>
    /// <param name="uiName">UI 名称</param>
    public void DestroyUI(string uiName)
    {
        if (uiInstances.ContainsKey(uiName))
        {
            if (uiInstances[uiName] != null)
            {
                Destroy(uiInstances[uiName]);
            }
            uiInstances.Remove(uiName);
            Debug.Log($"UIManager: 销毁 {uiName}");
        }
    }
    
    /// <summary>
    /// 获取指定的 UI 实例
    /// </summary>
    /// <param name="uiName">UI 名称</param>
    /// <returns>UI GameObject，如果不存在返回 null</returns>
    public GameObject GetUI(string uiName)
    {
        if (uiInstances.ContainsKey(uiName))
        {
            return uiInstances[uiName];
        }
        return null;
    }
    
    /// <summary>
    /// 检查 UI 是否已显示
    /// </summary>
    /// <param name="uiName">UI 名称</param>
    /// <returns>如果 UI 存在且激活返回 true</returns>
    public bool IsUIVisible(string uiName)
    {
        if (uiInstances.ContainsKey(uiName) && uiInstances[uiName] != null)
        {
            return uiInstances[uiName].activeSelf;
        }
        return false;
    }
    
    void OnDestroy()
    {
        // 清理所有 UI 实例
        foreach (var kvp in uiInstances)
        {
            if (kvp.Value != null)
            {
                Destroy(kvp.Value);
            }
        }
        uiInstances.Clear();
    }
}

