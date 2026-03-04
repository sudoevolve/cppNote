# 10-光照基础：Lambert/Phong/Blinn-Phong 与能量直觉

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 写出一个“能看出立体感”的最小光照（漫反射 + 高光）
- 搞清楚光照计算的空间选择：世界空间 vs 视图空间
- 建立能量直觉：为什么光照看起来“太亮/太灰/太塑料”

## 先决条件

- 已完成 [09-模型与网格：OBJ/GLTF（简化版）加载、法线、切线](09-模型与网格-OBJ-GLTF加载-法线-切线.md)

---

## 1. 光照最小闭环：你需要哪些输入

要在 fragment 里算光，你至少要有：

- 表面位置 `pos`
- 表面法线 `normal`
- 相机位置 `camPos`
- 光源信息（方向光或点光）
- 材质参数（颜色、粗糙程度/高光强度等）

为了减少变量，本章先用点光：

- `lightPos`：光源位置
- `lightColor`：光颜色/强度

---

## 2. 选一个空间：建议用世界空间（入门更直观）

你有两个常见选择：

- 在世界空间做光照：vertex 输出世界空间 `vWorldPos`、`vWorldNormal`
- 在视图空间做光照：把一切都变换到相机空间

本章建议世界空间：直觉更强，调试时也更容易打印/理解。

---

## 3. Vertex Shader：输出世界位置与世界法线

核心点：

- 位置：`worldPos = model * localPos`
- 法线：如果存在非均匀缩放，需要用“法线矩阵”（inverse transpose）

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldPos;
out vec3 vWorldNormal;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    mat3 normalMat = transpose(inverse(mat3(uModel)));
    vWorldNormal = normalize(normalMat * aNormal);

    gl_Position = uProj * uView * worldPos;
}
```

如果你暂时保证 model 不做非均匀缩放，`mat3(uModel) * aNormal` 也能先跑通，但建议尽早养成正确习惯。

---

## 4. Fragment Shader：Lambert + Blinn-Phong

### 4.1 漫反射（Lambert）

直觉：光打在面上，亮度取决于夹角。

```glsl
float NdotL = max(dot(N, L), 0.0);
diffuse = albedo * lightColor * NdotL;
```

### 4.2 高光（Blinn-Phong）

Blinn-Phong 用半程向量 `H = normalize(L + V)`：

```glsl
float spec = pow(max(dot(N, H), 0.0), shininess);
specular = lightColor * specStrength * spec;
```

组合：

```glsl
#version 330 core
in vec3 vWorldPos;
in vec3 vWorldNormal;
out vec4 FragColor;

uniform vec3 uCamPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uAlbedo;

uniform float uAmbient;      // 0.0~0.2
uniform float uSpecStrength; // 0.0~1.0
uniform float uShininess;    // 8~128

void main()
{
    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 V = normalize(uCamPos - vWorldPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = uAlbedo * uLightColor * NdotL;

    float spec = pow(max(dot(N, H), 0.0), uShininess);
    vec3 specular = uLightColor * (uSpecStrength * spec);

    vec3 ambient = uAlbedo * uAmbient;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
```

---

## 5. 能量直觉：为什么会“太亮/太塑料”

当你把 `ambient + diffuse + specular` 直接相加时，很容易出现：

- 亮部过曝（因为你在把能量越加越多）
- 高光像“白色贴片”（shininess/specStrength 不合理）

入门阶段的工程口径：

- 先让光照参数可调（GUI 以后再做，先用键盘/配置）
- 先把 `ambient` 控制得很小（0.02~0.1）
- `specStrength` 不要上来就 1.0
- 观察“视角变化”对高光的影响是否符合直觉

等你做到 HDR/色调映射，再系统解决“亮度范围”与“能量守恒”。

---

## 常见坑

- **法线没归一化**：亮度会怪，尤其是插值后法线长度不再是 1
- **空间混用**：位置在世界空间、法线在模型空间，结果必错
- **非均匀缩放没用法线矩阵**：光照会“像歪了”
- **光源距离没处理**：点光如果不做衰减，远近看起来不真实（下一步再加）

---

## 小练习

### 练习 1：加点光衰减

- 题目：按距离做衰减（例如 `1/(a + b*d + c*d^2)`）
- 目标：让光更像真实世界，训练参数调试能力

### 练习 2：把法线可视化作为 debug 模式

- 题目：按键切换：正常光照 vs `normal*0.5+0.5`
- 目标：让数据问题变得“可见”

---

## 小结

- 光照计算的核心输入：位置、法线、相机、光源、材质
- 先统一空间再算光（本章用世界空间）
- Lambert + Blinn-Phong 足够让你进入“真实渲染的调参世界”

下一篇：见 [11-阴影入门：Shadow Map 的最小实现与常见伪影](11-阴影入门-ShadowMap最小实现-常见伪影.md)

