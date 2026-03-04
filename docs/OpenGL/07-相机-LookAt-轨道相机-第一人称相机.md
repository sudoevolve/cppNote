# 07-相机：LookAt、轨道相机、第一人称相机

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 把第 04 章的 View 矩阵变成“可操作的相机”
- 实现两种最常用交互：轨道相机（看物体）与第一人称相机（走场景）
- 讲清相机三件套：位置、朝向、FOV/投影

## 先决条件

- 已完成 [04-坐标与变换：NDC、矩阵、MVP、右手系](04-坐标与变换-NDC-矩阵-MVP-右手系.md)

---

## 1. 相机是什么：你不移动相机，你在移动整个世界

OpenGL 没有一个“相机对象”。你看到的相机效果来自 View 矩阵：

- View 矩阵把世界变换到相机坐标系里
- 所以“相机往前走”，数学上通常是“世界往后挪”

工程上你只需要维护几项状态：

- 相机位置 `pos`
- 相机朝向（用 `front`/`up` 表示，或用 yaw/pitch 表示）
- 投影参数（FOV、near/far、宽高比）

---

## 2. LookAt：最小可用相机

GLM 的 `lookAt` 接受三件事：

- 相机位置 `eye`
- 看向的目标点 `center`
- 世界上方向 `up`

```cpp
glm::vec3 eye    = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view = glm::lookAt(eye, center, up);
```

你可以把它理解为：把一个“人拿着相机看原点”的直觉，翻译成矩阵。

---

## 3. 轨道相机（Orbit）：绕着目标转

轨道相机的本质是：相机位置在一个球面（或半球）上变化，目标点固定。

用两角 + 半径描述：

- `yaw`：水平绕 Y 轴旋转
- `pitch`：垂直角度（注意不要到 ±90°）
- `radius`：与目标的距离

```cpp
glm::vec3 target(0.0f, 0.0f, 0.0f);
float radius = 2.5f;

float yaw   = ...;   // 由鼠标拖拽累积
float pitch = ...;   // 由鼠标拖拽累积

glm::vec3 eye;
eye.x = target.x + radius * std::cos(pitch) * std::sin(yaw);
eye.y = target.y + radius * std::sin(pitch);
eye.z = target.z + radius * std::cos(pitch) * std::cos(yaw);

glm::mat4 view = glm::lookAt(eye, target, glm::vec3(0,1,0));
```

交互建议：

- 左键拖拽：改变 yaw/pitch
- 滚轮：改变 radius（缩放）

---

## 4. 第一人称相机（FPS）：用 yaw/pitch 决定朝向

第一人称相机更像游戏控制：

- WASD 移动位置
- 鼠标移动改变 yaw/pitch（朝向）

最常用的做法是：用 yaw/pitch 算一个 `front` 方向向量，然后 `lookAt(pos, pos+front, up)`。

```cpp
float yaw = ...;
float pitch = ...;

glm::vec3 front;
front.x = std::cos(pitch) * std::sin(yaw);
front.y = std::sin(pitch);
front.z = std::cos(pitch) * std::cos(yaw);
front = glm::normalize(front);

glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));
glm::vec3 up    = glm::normalize(glm::cross(right, front));

glm::mat4 view = glm::lookAt(pos, pos + front, up);
```

移动（按住按键按帧更新）：

```cpp
float speed = 2.0f * dt;
if (keyW) pos += front * speed;
if (keyS) pos -= front * speed;
if (keyA) pos -= right * speed;
if (keyD) pos += right * speed;
```

---

## 5. 投影参数：FOV/near/far 是“看起来像不像”的关键

透视投影：

```cpp
glm::mat4 proj = glm::perspective(glm::radians(fovDeg),
                                  aspect,
                                  0.1f, 100.0f);
```

工程建议：

- FOV 常用 45~70°
- near 不要太小（例如 0.001），会带来深度精度问题
- far 不要太大（例如 100000），同样会损失精度

---

## 常见坑

- **pitch 到 ±90° 附近抖动/翻转**：需要 clamp（比如限制到 ±89°）
- **移动速度随帧率变化**：必须用 `dt`（上一帧到这一帧的时间差）做速度缩放
- **鼠标灵敏度不稳定**：建议把“像素移动”乘一个固定系数，再累积到 yaw/pitch

---

## 小练习

### 练习 1：实现轨道相机的滚轮缩放

- 题目：滚轮改变 radius，并 clamp 在合理范围（比如 1.0~10.0）
- 目标：做出“看模型”的最小交互体验

### 练习 2：在 FPS 相机里加一个“加速键”

- 题目：按住 Shift 时 speed 乘 3
- 目标：让你的相机更像真实工具

---

## 小结

- 相机 = 你维护的一组状态 + 生成 View 矩阵的规则
- 轨道相机适合“看物体”，FPS 相机适合“走场景”
- near/far 的选择会影响后续深度精度，别乱填

下一篇：见 [08-深度与裁剪：depth test、face culling、clip space](08-深度与裁剪-depth-test-face-culling-clip-space.md)

