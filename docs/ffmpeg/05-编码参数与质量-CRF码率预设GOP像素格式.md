# 05-编码参数与质量：CRF、码率、预设、GOP、像素格式

目录页：见 [FFmpeg 文档目录](00-目录.md)

这章把“你经常照抄但不懂”的参数体系讲清楚：**质量怎么控、体积怎么控、兼容性怎么控**。

---

## 1. 两条主线：要么控质量，要么控码率

对新手最省心的结论：

- **离线文件**（上传网盘、保存、发群）：优先用 CRF（质量更稳定）
- **带宽受限/推流**（直播、限制码率）：优先用目标码率（CBR/ABR）

---

## 2. x264/x265 的“三件套”：CRF + preset + pix_fmt

### 2.1 CRF（质量旋钮）

示例（H.264）：

```bash
ffmpeg -i input.mov -c:v libx264 -crf 20 -c:a aac -b:a 192k out.mp4
```

直觉：

- `-crf` 越小越清晰、体积越大
- 不同片源同一个 `-crf` 体积可能差很多（内容复杂度决定）

### 2.2 preset（速度/体积/质量的折中）

示例：

```bash
ffmpeg -i input.mov -c:v libx264 -crf 20 -preset slow -c:a aac out.mp4
```

直觉：

- `preset` 越慢：同质量下体积更小 / 同体积下质量更好，但编码更慢
- 新手常用：`medium`（默认）或 `slow`

### 2.3 pix_fmt（兼容性旋钮）

如果你遇到“某些播放器不认/颜色怪/手机打不开”，先试：

```bash
ffmpeg -i input.mov -c:v libx264 -crf 20 -pix_fmt yuv420p -c:a aac out.mp4
```

工程直觉：

- `yuv420p` 通常是兼容性最好的输出格式

---

## 3. 码率控制：-b:v / -maxrate / -bufsize

最常见的“想让文件不要超过某个大小”会走码率路线：

```bash
ffmpeg -i input.mp4 -c:v libx264 -b:v 2500k -maxrate 2500k -bufsize 5000k -c:a aac -b:a 160k out.mp4
```

直觉：

- `-b:v` 是目标码率（更像平均）
- `-maxrate` + `-bufsize` 让瞬时码率受控（对流媒体更关键）

---

## 4. GOP / 关键帧：-g 与 -keyint_min

你会在“剪辑、推流、seek”场景里遇到 GOP 参数。

示例（假设 30fps，2 秒一个关键帧）：

```bash
ffmpeg -i input.mp4 -c:v libx264 -crf 20 -g 60 -keyint_min 60 -sc_threshold 0 -c:a aac out.mp4
```

直觉：

- 关键帧更密：拖动进度条更爽，但体积更大
- 推流常常需要固定 keyframe 间隔

---

## 5. profile/level：为了“某些硬件能播”

如果你目标是“老设备/某些硬解器”，就要控制 profile/level。

示例（H.264 baseline 更保守）：

```bash
ffmpeg -i input.mp4 -c:v libx264 -profile:v baseline -level 3.1 -pix_fmt yuv420p -c:a aac out.mp4
```

这不是为了“画质更好”，而是为了“更容易播”。

---

## 6. 帧率与 VFR：-r 不是万能药

常见需求：把可变帧率的录屏转成固定帧率，方便剪辑：

```bash
ffmpeg -i input.mp4 -vf "fps=30" -c:v libx264 -crf 20 -c:a aac out_30fps.mp4
```

建议：

- 需要“重采样时间轴”就用 `-vf fps=...`
- `-r` 更像输出层面的帧率声明/节流，很多情况下不如 `fps` 明确

---

## 7. 音频参数：采样率/声道/码率

### 7.1 统一采样率

```bash
ffmpeg -i input.mp4 -c:v copy -c:a aac -ar 48000 out.mp4
```

### 7.2 统一声道

```bash
ffmpeg -i input.mp4 -c:v copy -c:a aac -ac 2 out.mp4
```

### 7.3 音频码率

```bash
ffmpeg -i input.mp4 -c:v copy -c:a aac -b:a 192k out.mp4
```

---

## 8. 两遍编码（2-pass）：当你必须卡体积/码率

两遍编码的常见用途：

- 你明确要一个目标码率/目标体积，并希望尽可能好看

示例（第一遍不输出音频）：

```bash
ffmpeg -y -i input.mp4 -c:v libx264 -b:v 2500k -pass 1 -an -f mp4 NUL
ffmpeg -i input.mp4 -c:v libx264 -b:v 2500k -pass 2 -c:a aac -b:a 160k out.mp4
```

---

## 9. 初学者常见“参数选型”模板

### 9.1 通用输出（发群/网盘/兼容性优先）

```bash
ffmpeg -i input.mov -c:v libx264 -crf 20 -preset medium -pix_fmt yuv420p -c:a aac -b:a 192k out.mp4
```

### 9.2 更省体积（离线压缩）

```bash
ffmpeg -i input.mov -c:v libx265 -crf 26 -preset slow -pix_fmt yuv420p -c:a aac -b:a 128k out.mp4
```

### 9.3 推流/带宽受限（码率优先）

```bash
ffmpeg -i input.mp4 -c:v libx264 -b:v 2500k -maxrate 2500k -bufsize 5000k -g 60 -pix_fmt yuv420p -c:a aac -b:a 160k out.mp4
```

