# 01-安装与工程：vcpkg + CMake + VS（命令行与链接）

目录页：见 [FFmpeg 文档目录](00-目录.md)

## 目标

- 在 Windows + VS 环境把 FFmpeg 安装到工程里（vcpkg）
- 能稳定调用 `ffmpeg/ffprobe` 做命令行处理
- 能在 CMake 工程里链接 FFmpeg 的库（libav*）

---

## 1. 用 vcpkg 安装 FFmpeg（推荐）

FFmpeg 在 vcpkg 里的 port 名称就是 `ffmpeg`：

```
vcpkg install ffmpeg
```

FFmpeg 的能力非常多，vcpkg 里也提供了一堆 feature。初学阶段你可以先接受一个工程口径：

- 先装默认的 `ffmpeg`，把“命令行能用 + 工程能链接 + 运行时 DLL 能找到”跑通
- 之后再按需求加 feature（例如字幕、字体渲染、特定编码器、硬件加速等）

如果你已经在 OpenGL 教程里用过 VS + CMake + vcpkg toolchain，这里保持同一套口径即可：

- 固定 triplet（例如 `x64-windows`）
- CMake 配置时固定 vcpkg toolchain

参考（port 说明与 feature 列表）：https://vcpkg.link/ports/ffmpeg

---

## 2. 命令行可用性：ffmpeg / ffprobe 在哪里

你可能会遇到“库安装了，但找不到 `ffmpeg.exe`”的情况。原因通常是：

- vcpkg 安装的是库本体；命令行工具是否安装/是否在 PATH 上，取决于 port feature 与你的使用方式
- VS 启动调试时 PATH 与终端 PATH 不是一回事

建议工程化口径是：

- **开发期**：在你自己的工程里固定一个 `tools/` 或 `third_party/tools/`，把 `ffmpeg.exe/ffprobe.exe` 作为外部工具管理
- **发布期**：明确把运行时依赖的 DLL 与工具一起打包（避免“只在我电脑能跑”）

如果你是通过 vcpkg 获取命令行工具，通常可以在 vcpkg 的 `installed/<triplet>/tools/ffmpeg/` 一类目录下找到它们（路径以你的 vcpkg 布局为准）。

---

## 3. CMake 链接 FFmpeg（libav*）

FFmpeg 的库通常由这些组件组成：

- `libavformat`：容器/封装（读写 mp4/mkv/flv…）
- `libavcodec`：编解码（H.264/H.265/AAC…）
- `libavutil`：通用工具（时间基、字典、缓冲区、像素格式…）
- `libswscale`：图像缩放与像素格式转换
- `libswresample`：音频重采样与格式转换

vcpkg 的 `ffmpeg` port 提供 CMake 集成，典型用法是：

```cmake
find_package(FFMPEG REQUIRED)
target_include_directories(app PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(app PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(app PRIVATE ${FFMPEG_LIBRARIES})
```

如果你想让“工程可移植 + 配置更稳定”，建议把依赖放进 vcpkg manifest（`vcpkg.json`）并用 CMakePresets/VS 的 CMake 集成固定 toolchain 与 triplet。

### 3.1 链接不等于能跑：运行时 DLL 要能被找到

Windows 下最常见的问题是：

- 编译链接通过，但运行时报 “找不到 xxx.dll”

第一反应检查：

- DLL 是否在可执行文件同目录
- 或者是否在 PATH 可搜索目录中
- Debug/Release、x86/x64 是否混用了不同目录的 DLL

更推荐的工程习惯是：先把“能编译 + 能运行 + 能定位 DLL”跑通，再进入具体的解码/编码流程。因为 FFmpeg 真正难的部分往往不是 API，而是工程环境与数据路径。

---

## 4. License 的工程现实（你至少要知道的）

FFmpeg 里有不同 license 组合（LGPL/GPL/nonfree）对应不同功能组合与可分发性。初学阶段不需要深究条款，但需要知道：

- 你用的 FFmpeg build 是“带了哪些编码器/库”决定了你能不能用 `libx264`、`libx265` 等
- 当你要把程序发布给别人时，license 选择会影响你能否合法分发对应二进制

工程口径：不确定就先用“功能更基础、可分发性更清晰”的组合，等你真的需要某个编码器/滤镜再补齐。

---

## 常见坑

- **Debug/Release 混用依赖**：同一个工程里用到不同 triplet/构建类型的库，会出现运行时找 DLL 或链接符号对不上的问题
- **只链接了 avcodec**：很多符号实际在 avutil/avformat 里，缺一个就会一串 unresolved external
- **把时间戳当成秒**：libav 的时间基是 `AVRational`，很多字段是“以 time_base 为单位”的整数
