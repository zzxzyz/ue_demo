using UnityEngine;
using IDbg;  // IDbg库命名空间

public class RotateCube : MonoBehaviour
{
    void Start()
    {
        Debug.Log("立方体开始旋转了！");
        
        // 使用IDbg获取系统信息（IDbgInitializer已自动初始化）
        if (IDbgWrapper.IsAvailable())
        {
            Debug.Log($"CPU核心数: {IDbgWrapper.GetCpuCore()}");
            Debug.Log($"进程ID: {IDbgWrapper.GetProcessId()}");
        }
    }

    void Update()
    {
        // 每帧旋转立方体
        // 参数：每秒旋转的角度
        transform.Rotate(0, 50 * Time.deltaTime, 0);
    }
}