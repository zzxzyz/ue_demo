using UnityEngine;

public class RotateCube : MonoBehaviour
{
    void Start()
    {
        Debug.Log("立方体开始旋转了！");
    }

    void Update()
    {
        // 每帧旋转立方体
        // 参数：每秒旋转的角度
        transform.Rotate(0, 50 * Time.deltaTime, 0);
    }
}