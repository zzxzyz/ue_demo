using UnityEngine;
using TMPro;  // TextMeshPro 命名空间
using UnityEngine.UI;  // UI 命名空间
using System.Collections;  // 协程命名空间
using IDbg;  // IDbg库命名空间（参考IDbgExample.cs的用法）

public class LoginManager : MonoBehaviour
{
    // 在 Inspector 中拖拽关联这些组件
    public TMP_InputField usernameInput;
    public TMP_InputField passwordInput;
    public Button loginButton;
    public TextMeshProUGUI titleText;
    public GameObject loginPanel;  // 登录面板，登录成功后隐藏

    void Start()
    {
        // 初始化IDbg（参考IDbgExample.cs）
        IDbgLog.Initialize();

        // 给登录按钮添加点击事件
        loginButton.onClick.AddListener(OnLoginButtonClicked);

        // 如果loginPanel未赋值，尝试自动查找
        if (loginPanel == null)
        {
            GameObject foundPanel = GameObject.Find("LoginPanel");
            if (foundPanel != null)
            {
                loginPanel = foundPanel;
                Debug.Log("自动找到LoginPanel: " + loginPanel.name);
            }
            else
            {
                Debug.LogWarning("未找到LoginPanel，请确保在Inspector中手动赋值loginPanel字段！");
            }
        }
        else
        {
            Debug.Log("LoginPanel已赋值: " + loginPanel.name);
        }

        // 使用IDbg获取系统信息（参考IDbgExample.cs）
        if (IDbgWrapper.IsAvailable())
        {
            Debug.Log($"CPU核心数: {IDbgWrapper.GetCpuCore()}");
            Debug.Log($"进程ID: {IDbgWrapper.GetProcessId()}");
        }

        Debug.Log("登录界面已加载");
    }

    // 登录按钮点击时调用
    void OnLoginButtonClicked()
    {
        string username = usernameInput.text;
        string password = passwordInput.text;

        Debug.Log("点击了登录按钮");
        Debug.Log("用户名：" + username);
        Debug.Log("密码：" + password);

        // 简单的验证逻辑
        if (string.IsNullOrEmpty(username))
        {
            Debug.LogWarning("用户名不能为空！");
            titleText.text = "请输入用户名！";
            return;
        }

        if (string.IsNullOrEmpty(password))
        {
            Debug.LogWarning("密码不能为空！");
            titleText.text = "请输入密码！";
            return;
        }

        // 模拟登录验证（实际项目中需要连接服务器）
        if (username == "admin" && password == "123456")
        {
            Debug.Log("登录成功！");
            titleText.text = "登录成功！";
            titleText.color = Color.green;
            
            // 使用IDbg获取性能信息（参考IDbgExample.cs）
            if (IDbgWrapper.IsAvailable())
            {
                float appMemory = IDbgWrapper.GetAppMemory();
                float appCpu = IDbgWrapper.GetAppCpu();
                Debug.Log($"[IDbg-登录成功] 应用CPU: {appCpu}%, 应用内存: {appMemory} MB");
            }
            
            // 登录成功后延迟隐藏登录界面（让用户看到成功提示）
            StartCoroutine(HideLoginPanelAfterDelay(1.5f));
        }
        else
        {
            Debug.LogError("用户名或密码错误！");
            titleText.text = "登录失败！";
            titleText.color = Color.red;
        }
    }

    // 延迟隐藏登录面板的协程
    IEnumerator HideLoginPanelAfterDelay(float delay)
    {
        yield return new WaitForSeconds(delay);
        
        // 如果loginPanel未赋值，再次尝试查找
        if (loginPanel == null)
        {
            loginPanel = GameObject.Find("LoginPanel");
        }
        
        if (loginPanel != null)
        {
            Debug.Log("正在隐藏登录界面: " + loginPanel.name);
            loginPanel.SetActive(false);
            Debug.Log("登录界面已成功隐藏");
        }
        else
        {
            Debug.LogError("无法隐藏登录界面：loginPanel为null！请检查场景中是否存在名为'LoginPanel'的GameObject，或在Inspector中手动赋值loginPanel字段。");
        }
    }

    void OnDestroy()
    {
        // 清理IDbg资源（参考IDbgExample.cs）
        IDbgLog.Cleanup();
    }
}