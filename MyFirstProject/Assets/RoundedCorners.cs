// 创建脚本 RoundedCorners.cs
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Image))]
public class RoundedCorners : MonoBehaviour
{
    void Start()
    {
        // 设置为圆角需要特殊的 Sprite
        // 我们先用简单的方法：添加阴影效果
        var shadow = gameObject.AddComponent<Shadow>();
        shadow.effectColor = new Color(0, 0, 0, 0.5f);
        shadow.effectDistance = new Vector2(5, -5);
    }
}