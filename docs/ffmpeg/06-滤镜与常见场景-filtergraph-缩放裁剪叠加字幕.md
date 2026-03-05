# 06-滤镜与常见场景：filtergraph、缩放裁剪、叠加、水印、字幕

目录页：见 [FFmpeg 文档目录](00-目录.md)

当你想“改变画面内容”时，你大概率需要滤镜（filter）。这章讲清楚：

- `-vf`/`-af` 的语法直觉
- 常用滤镜怎么组合
- 初学者最常见的 20 个场景怎么写

---

## 1. filtergraph 心智模型：把处理写成一条流水线

你可以把滤镜理解成：

- 输入 →（一系列变换）→ 输出

### 1.1 视频滤镜：-vf

```bash
ffmpeg -i input.mp4 -vf "scale=-2:720" -c:v libx264 -crf 20 -c:a copy out.mp4
```

### 1.2 音频滤镜：-af

```bash
ffmpeg -i input.mp4 -af "volume=0.8" -c:v copy -c:a aac out.mp4
```

### 1.3 多个滤镜的连接

多个滤镜一般用逗号串起来：

```bash
-vf "crop=iw:ih-80:0:40,scale=-2:720"
```

---

## 2. 缩放/裁剪/补边：scale / crop / pad

### 2.1 等比缩放到 720p

```bash
ffmpeg -i input.mp4 -vf "scale=-2:720" -c:v libx264 -crf 20 -c:a copy out_720p.mp4
```

`-2` 的意义：自动算出能被 2 整除的宽度（很多编码器要求偶数尺寸）。

### 2.2 裁掉上下黑边（示例）

```bash
ffmpeg -i input.mp4 -vf "crop=iw:ih-140:0:70" -c:v libx264 -crf 20 -c:a copy out_crop.mp4
```

### 2.3 补边做成 16:9（示例）

```bash
ffmpeg -i input.mp4 -vf "scale=1280:-2,pad=1280:720:(ow-iw)/2:(oh-ih)/2" -c:v libx264 -crf 20 -c:a copy out_pad.mp4
```

---

## 3. 旋转与翻转：transpose / hflip / vflip

旋转 90°：

```bash
ffmpeg -i input.mp4 -vf "transpose=1" -c:v libx264 -crf 20 -c:a copy out_rot.mp4
```

水平翻转：

```bash
ffmpeg -i input.mp4 -vf "hflip" -c:v libx264 -crf 20 -c:a copy out_hflip.mp4
```

---

## 4. 帧率/时间轴：fps / setpts / atempo

### 4.1 把输出统一成 30fps

```bash
ffmpeg -i input.mp4 -vf "fps=30" -c:v libx264 -crf 20 -c:a aac out_30fps.mp4
```

### 4.2 倍速（视频 + 音频）

2 倍速（视频时间轴缩短为 0.5 倍，音频用 atempo）：

```bash
ffmpeg -i input.mp4 -vf "setpts=0.5*PTS" -af "atempo=2.0" -c:v libx264 -crf 20 -c:a aac out_2x.mp4
```

---

## 5. 水印/画中画：overlay

把 `logo.png` 叠到右上角：

```bash
ffmpeg -i input.mp4 -i logo.png -filter_complex "overlay=W-w-20:20" -c:v libx264 -crf 20 -c:a copy out_logo.mp4
```

关键点：

- 多输入就优先用 `-filter_complex`
- `W/H` 是主视频尺寸，`w/h` 是 overlay 输入尺寸

---

## 6. 文字水印：drawtext（需要字体）

```bash
ffmpeg -i input.mp4 -vf "drawtext=text='Hello':x=20:y=20:fontsize=36:fontcolor=white" -c:v libx264 -crf 20 -c:a copy out_text.mp4
```

实际工程里你通常需要指定字体文件（Windows 字体路径或自带字体文件），避免机器差异导致找不到字体。

---

## 7. 字幕：软字幕 vs 硬字幕

### 7.1 软字幕（封装进容器，不烧进画面）

```bash
ffmpeg -i input.mp4 -i sub.srt -c:v copy -c:a copy -c:s srt out_with_sub.mkv
```

`mkv` 对字幕更友好，mp4 对字幕封装限制更多。

### 7.2 硬字幕（烧进画面）

```bash
ffmpeg -i input.mp4 -vf "subtitles=sub.srt" -c:v libx264 -crf 20 -c:a copy out_hardsub.mp4
```

---

## 8. 抽帧/缩略图：fps / select

每秒 1 张图：

```bash
ffmpeg -i input.mp4 -vf "fps=1" thumbs/%04d.jpg
```

只抽关键帧（常用于“快速预览”）：

```bash
ffmpeg -i input.mp4 -vf "select='eq(pict_type\,I)'" -vsync vfr keyframes/%04d.jpg
```

---

## 9. 合成与拼接（滤镜版 concat）

当 `-c copy` 拼接不好使（编码参数不一致/音画不同步）时，用滤镜 concat 强制重编码更稳。

示例（两段视频拼接为一段）：

```bash
ffmpeg -i a.mp4 -i b.mp4 -filter_complex "[0:v][0:a][1:v][1:a]concat=n=2:v=1:a=1[v][a]" -map "[v]" -map "[a]" -c:v libx264 -crf 20 -c:a aac out.mp4
```

---

## 10. 你该什么时候用滤镜

- 改分辨率/裁剪/补边/旋转：用滤镜
- 加水印/加字幕/画中画：用滤镜
- 改帧率/做倍速：用滤镜
- 只换容器：不要用滤镜，直接 `-c copy`

