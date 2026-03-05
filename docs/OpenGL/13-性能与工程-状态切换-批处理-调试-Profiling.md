# 13-性能与工程：状态切换、批处理、调试与 Profiling

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 把“能画”升级为“能做项目”：资源生命周期、状态管理、渲染顺序的基本纪律
- 认识性能的主要杀手：过多 draw call、频繁状态切换、CPU/GPU 同步
- 建立调试与 Profiling 的最小工具箱：标记、统计、定位瓶颈

## 先决条件

- 至少完成到 [10-光照基础：Lambert/Phong/Blinn-Phong 与能量直觉](10-光照基础-Lambert-Phong-BlinnPhong-能量直觉.md)

---

## 1. OpenGL 的性能直觉：你在和“状态机 + 命令队列”打交道

OpenGL 很多操作不是“立刻在 GPU 上执行”，而是：

- CPU 记录一串命令（state change + draw call）
- 驱动组织并下发给 GPU

所以性能问题通常来自两端：

- CPU 侧：命令太多、状态切换太频繁、驱动开销大
- GPU 侧：像素太多、shader 太重、带宽太大

---

## 2. 三个最常见瓶颈（先从这里抓）

### 2.1 draw call 太多

现象：

- 场景很简单但帧率很低
- CPU 占用高

最小应对：

- 合并网格（static batching）
- 实例化绘制（instancing）
- 减少材质种类（材质切换 = 状态切换）

### 2.2 状态切换太频繁

典型高成本切换：

- program（shader）切换
- framebuffer 切换
- 大量纹理绑定与采样状态变更

最小应对：

- 按“shader → 材质 → 网格”排序渲染队列（减少切换）
- 把常用状态集中设置，不要每个 draw 都重复设置

### 2.3 CPU/GPU 同步

典型同步点：

- `glGet*`（频繁读取 GPU 状态）
- `glReadPixels`（读回屏幕/纹理）
- 某些 buffer 更新方式导致 pipeline stall

最小应对：

- 避免在渲染主路径里做读回
- 用异步方式（PBO/查询对象）把读回延后

---

## 3. 资源生命周期：你需要一套“不会泄漏也不会悬空”的规则

你会管理的资源大致分三类：

- GPU 对象：VAO/VBO/EBO/Texture/FBO/Program
- CPU 资源：模型数据、图片像素、缓存
- 跨帧状态：相机、灯光、材质参数

建议工程规则：

- 创建与销毁成对出现（谁创建谁销毁）
- 不要在每帧创建/销毁 GPU 对象（会抖动）
- 资源加载与渲染分离（加载线程/加载阶段 vs 渲染阶段）

---

## 4. 渲染队列：用排序控制状态切换

最常见的渲染顺序框架：

- 不透明物体：按 shader/material 排序（减少切换），前到后（降低 overdraw）
- 透明物体：后到前（正确混合）
- 特殊 pass：阴影、后处理等独立队列

你不必一次做成完整引擎，但要把“顺序”当作工程能力。

---

## 5. 调试与 Profiling：最小工具箱

### 5.1 KHR_debug：让 GPU 报错更可读

你在第 02 章已经打开了 debug callback，这会成为后续所有章节的安全网。

建议养成习惯：

- 给关键 pass 打 label（framebuffer、program、texture）
- 在 debug 输出里能直接定位到“哪个 pass 出事”

### 5.2 统计：帧内你到底做了多少事

最低成本统计项：

- draw call 数
- 三角形数
- program 切换次数
- texture 绑定次数
- FBO 切换次数

### 5.3 GPU 时间：不要只看 CPU 计时

CPU `chrono` 只能告诉你“提交命令花了多久”，不一定等于 GPU 真正的渲染耗时。

工程化的下一步是引入 GPU timer query（概念上）：

- 对每个 pass 计 GPU 时间
- 找到真正重的 shader/后处理步骤

---

## 6. Uniform 组织：从散装 uniform 到 UBO（Uniform Buffer）

当你开始画多个物体、多个 shader 时，会遇到两个现实问题：

- 同一份数据（相机矩阵、时间、分辨率）要给很多 program 设置
- `glUniform*` 分散调用，很难管理，也容易漏

UBO 的工程价值是：把“一组经常一起更新的数据”作为一个 buffer 绑定给多个 shader 使用。

### 6.1 GLSL：定义一个 uniform block

```glsl
layout(std140, binding = 0) uniform Camera
{
    mat4 uView;
    mat4 uProj;
    vec3 uCamPos;
    float _pad0;
};
```

几个工程要点：

- `binding = 0` 给了一个固定插槽（避免每个 program 都去查 block index）
- `std140` 是最常见的布局规则（可移植、好踩坑也好排）
- `vec3` 后面通常需要补一个 `float` 做对齐

### 6.2 C++：创建 UBO 并绑定到 binding point

初始化（一次即可）：

```cpp
GLuint uboCamera = 0;
glGenBuffers(1, &uboCamera);
glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboCamera);
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

每帧更新（只更新一次，然后所有 shader 都能读到）：

```cpp
glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &cameraData);
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

这条路径让“相机/全局参数更新”从 N 次 `glUniformMatrix4fv` 变成 1 次 buffer 更新。

---

## 7. Instancing：用一次 draw 画 N 个物体

当你需要画大量重复物体（草、碎石、子弹、树），instancing 往往是性价比最高的提速点：

- 一次 draw call 画出 N 个实例
- 每个实例有自己的 transform/颜色等参数

### 7.1 最小落地：instance buffer + divisor

你可以把每个实例的模型矩阵作为 per-instance attribute 传进 shader。

概念示例（每个实例一个 `mat4`，拆成 4 个 `vec4` attribute）：

```cpp
glBindVertexArray(vao);

glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::mat4), matrices, GL_DYNAMIC_DRAW);

std::size_t vec4Size = sizeof(float) * 4;
for (int i = 0; i < 4; ++i) {
    glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
    glEnableVertexAttribArray(2 + i);
    glVertexAttribDivisor(2 + i, 1);
}
```

绘制时：

```cpp
glBindVertexArray(vao);
glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
```

工程口径：

- divisor=1 表示“每个实例前进一次”，而不是每个顶点前进一次
- instance 数据的更新频率要控制（尽量批量更新，而不是每帧碎片化更新）

---

## 8. RenderDoc：把“猜哪里慢/哪里错”变成“抓一帧看清楚”

当你开始做阴影、后处理、PBR，很多问题靠肉眼和日志不够：

- 某个 pass 根本没画到
- 纹理内容不对（全黑/全白/维度错/格式错）
- blend/depth/stencil 某个状态没恢复

RenderDoc 的价值是：抓一帧，把每个 draw call 的输入/输出/状态都展开给你看。

### 8.1 最小抓帧流程（工程口径）

- 用 Debug 构建运行你的程序
- 启动 RenderDoc，Launch 或 Attach 到进程
- 在合适时机触发 capture（RenderDoc 热键或 UI 按钮）
- 在事件列表里按 pass 结构查：阴影 pass → 主场景 pass → screen pass

### 8.2 让抓帧更好读：给对象打 label

你已经用过 KHR_debug 回调；进一步的工程习惯是：

- 给 framebuffer/program/texture 标注名字
- 让抓帧里看到的资源列表有语义

例如（OpenGL 4.3 或 KHR_debug）：

```cpp
glObjectLabel(GL_FRAMEBUFFER, fbo, -1, "SceneFBO");
glObjectLabel(GL_TEXTURE, sceneTex, -1, "SceneColor");
glObjectLabel(GL_PROGRAM, program, -1, "PBR");
```

### 8.3 抓帧时优先看的三样东西

- Pipeline State：blend/depth/stencil/cull/viewport 是否如你预期
- Resource Inspector：你采样的贴图内容是不是你以为的那张
- Mesh Viewer：顶点属性/索引/绕序是否正确

---

## 9. VS + vcpkg：让“配置/构建/调试”稳定可复用

教程的终点是“在 VS 里做一个小型渲染器”，所以你需要把下面几件事固定成工程口径：

### 9.1 依赖固定：triplet 与 toolchain

- 统一 triplet（例如 `x64-windows`），避免 Debug/Release 或不同机器切换时依赖散掉
- CMake 配置时固定 vcpkg toolchain

概念示例：

```
-DCMAKE_TOOLCHAIN_FILE=.../vcpkg/scripts/buildsystems/vcpkg.cmake
-DVCPKG_TARGET_TRIPLET=x64-windows
```

### 9.2 调试体验：工作目录与资源路径

渲染器必然会加载资源（shader、贴图、模型）。最常见的“在 VS 里一调试就全黑/找不到文件”来自：

- working directory 不对（相对路径失效）
- 资源没被复制到输出目录

建议把资源路径策略统一成两种之一：

- A：运行时以“工程根目录”为基准（working directory 指向工程根目录）
- B：运行时以“输出目录”为基准（build/bin/xxx），并在构建时把 `shaders/`、`assets/` 复制过去

只要你统一了口径，后续章节的所有示例就能在 VS 里稳定跑起来。

### 9.3 Debug/Release 的区别要显式对待

- Debug：更重的校验（KHR_debug、断言、更多日志）
- Release：更少同步、更少日志、更多批处理

工程习惯是：先在 Debug 把渲染链路调通，再去 Release 看真实性能。

---

## 10. 到这里你应该拥有的“可持续迭代”底座

把前 1~9 章的输出收敛成一个渲染器底座，最低应当具备：

- 一个可反复复用的帧循环：输入 → 更新 → 渲染 → 交换缓冲
- 一个明确的渲染顺序：不透明 → 透明 → 特殊 pass（阴影/后处理）
- 一套全局参数通路：Camera/Time/Resolution 不散落在各处
- 一套可抓帧可定位的可观测性：KHR_debug + label + RenderDoc

后面的 14 章会在这个底座之上把材质与光照升级到“更像真实世界”的表达。

---

## 常见坑

- **把 debug callback 当成唯一真相**：有些性能问题不会报错，只能靠统计与计时
- **每帧 new/delete GPU 资源**：会造成卡顿与碎片化
- **透明物体渲染顺序乱**：性能/正确性都会受影响
- **全局参数到处 glUniform**：迟早会漏；把相机/时间/灯光等收敛进 UBO
- **想减少 draw call 却只会合并网格**：优先考虑 instancing（重复物体的最优解）
- **抓帧看不懂**：先按 pass 切块，再用 label 把资源起名

---

## 小练习

### 练习 1：做一套渲染统计面板（先用控制台）

- 题目：每秒打印一次：draw call 数、program 切换次数
- 目标：让优化从“猜”变成“量化”

### 练习 2：把不透明物体排序

- 题目：按 program→材质→网格排序绘制，并比较状态切换次数
- 目标：练一次“渲染队列”的工程思维

### 练习 3：用 UBO 管相机矩阵

- 题目：把 view/proj 从散装 uniform 迁移到一个 Camera UBO，并让两个 shader 共享
- 目标：建立“全局参数”的可复用组织方式

### 练习 4：用 instancing 画 1000 个立方体

- 题目：用 `glDraw*Instanced` 画大量重复物体，并对比 draw call 数
- 目标：把“批处理”从概念变成可量化收益

---

## 小结

- 性能优化先抓：draw call、状态切换、CPU/GPU 同步
- 资源生命周期与渲染队列是“能做项目”的分水岭
- 调试要可观测：debug 输出 + 统计 + GPU 计时
- 工程化的数据通路：UBO 管全局参数，instancing 管重复物体
- RenderDoc 抓帧是从“猜”到“确认”的关键工具

下一篇：见 [14-PBR 入门：BRDF 的直觉与实现骨架](14-PBR入门-BRDF直觉-实现骨架.md)
