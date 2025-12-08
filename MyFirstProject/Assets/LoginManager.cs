using UnityEngine;
using TMPro;  // TextMeshPro 命名空间
using UnityEngine.UI;  // UI 命名空间

public class LoginManager : MonoBehaviour
{
    // 在 Inspector 中拖拽关联这些组件
    public TMP_InputField usernameInput;
    public TMP_InputField passwordInput;
    public Button loginButton;
    public TextMeshProUGUI titleText;

    void Start()
    {
        // 给登录按钮添加点击事件
        loginButton.onClick.AddListener(OnLoginButtonClicked);

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
        }
        else
        {
            Debug.LogError("用户名或密码错误！");
            titleText.text = "登录失败！";
            titleText.color = Color.red;
        }
    }
}