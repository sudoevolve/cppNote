# 07-ffprobe 与排错：流选择（map）、同步、常见报错

目录页：见 [FFmpeg 文档目录](00-目录.md)

FFmpeg 真正“劝退新手”的地方不是转码命令，而是：

- 输入文件不规整（多音轨、多字幕、VFR、时间戳异常）
- 输出容器/编码要求多（播放器/平台限制）
- 报错信息看不懂

这章给你一套可复用的排错顺序。

---

## 1. 先用 ffprobe 把输入“看清楚”

### 1.1 最常用：直接看流信息

```bash
ffprobe -hide_banner -i input.mp4
```

你要重点看：

- 有几条视频/音频/字幕流
- 编码是什么（h264/hevc/aac/opus…）
- 帧率是 CFR 还是 VFR（观察 avg_frame_rate / r_frame_rate）
- 像素格式（yuv420p / yuv444p / nv12…）

### 1.2 结构化输出（更适合“看字段”）

```bash
ffprobe -v error -show_streams -show_format -of json input.mp4
```

---

## 2. -map：把“选流”变成可控的

### 2.1 只要视频 + 第一条音频

```bash
ffmpeg -i input.mkv -map 0:v:0 -map 0:a:0 -c copy out.mkv
```

### 2.2 只要视频，不要音频/字幕

```bash
ffmpeg -i input.mkv -map 0:v:0 -c copy out.mp4
```

### 2.3 选指定语言的音轨

```bash
ffmpeg -i input.mkv -map 0:v:0 -map 0:a:m:language:eng -c copy out.mkv
```

工程建议：

- 只要你发现文件里有多条流，就别赌默认策略，直接写 `-map`

---

## 3. 同步问题：先判断是不是 VFR

“音画不同步/剪切后错位/抽帧不均匀”常见根因之一是 VFR。

处理思路：

1. `ffprobe` 看帧率字段与时长是否异常
2. 需要稳定剪辑就转 CFR：

```bash
ffmpeg -i input.mp4 -vf "fps=30" -c:v libx264 -crf 20 -c:a aac out_cfr.mp4
```

当你只想转封装（`-c copy`）但又遇到同步坑时，通常意味着：

- 你需要接受“重编码一次换稳定”

---

## 4. 常见报错与第一反应

### 4.1 Non-monotonous DTS / Invalid DTS

直觉：时间戳顺序不对或容器里写得很怪。

第一反应：

- 先转码一次（让 FFmpeg 重新生成时间戳）
- 或者转封装到更宽容的容器（例如 mkv）

### 4.2 moov atom not found（MP4）

直觉：文件不完整/不是标准 mp4。

第一反应：

- 确认文件是否下载完整
- 如果是你自己生成 mp4 时中断了，通常救不了

### 4.3 Could not find codec parameters

直觉：容器信息不足/需要更多探测数据。

第一反应（增加探测）：

```bash
ffmpeg -probesize 50M -analyzeduration 50M -i input.xxx -f null -
```

### 4.4 Unknown encoder 'libx264'

直觉：你的 FFmpeg build 没带这个编码器（或是受 license/feature 影响）。

第一反应：

- `ffmpeg -encoders | findstr x264`
- 换成你现有 build 支持的编码器

---

## 5. 日志与报告：让问题可复现

### 5.1 让输出更“可读”

```bash
ffmpeg -hide_banner -loglevel info -i input.mp4 ...
```

常用 loglevel：

- `error`：只看错误
- `warning`：警告 + 错误
- `info`：默认偏详细
- `verbose` / `debug`：排错用

### 5.2 生成报告文件

```bash
ffmpeg -report -i input.mp4 ...
```

会在当前目录生成日志文件，适合你把问题贴出来或自己回看。

---

## 6. 排错清单（按顺序做，效率最高）

1. 用 `ffprobe` 看清楚流：有几条视频/音频/字幕，编码是什么
2. 明确你要“转封装”还是“转码”
3. 涉及多流就写 `-map`
4. 遇到同步坑先怀疑 VFR，再决定是否转 CFR
5. 兼容性问题优先试 `-pix_fmt yuv420p`
6. 遇到诡异报错先开日志与 `-report`，保证可复现

