# 03-libav 入门：从打开文件到拿到帧（最小解码链路）

目录页：见 [FFmpeg 文档目录](00-目录.md)

这章给一条“最短可跑通”的解码骨架：打开文件 → 找视频流 → 打开解码器 → 循环读包并解出帧。

---

## 0. 你要先搞清楚的三个名词：packet / frame / codec

- `AVPacket`：压缩数据包（来自容器/网络），通常对应“若干压缩数据”
- `AVFrame`：解码后的原始帧（视频：像素平面；音频：采样平面）
- `AVCodecContext`：编解码器的工作状态（参数 + 内部缓冲 + 参考帧等）

初学者常见误区：

- 以为 “1 packet = 1 frame”。实际可能是：多个 packet 才能解出 1 帧；也可能 1 packet 解出多帧。

## 1. 解码链路里每个库负责什么

- `libavformat`：打开输入（文件/网络）、解析容器、读出 packet
- `libavcodec`：把 packet 解成 frame（或把 frame 编成 packet）
- `libavutil`：frame/时间基/像素格式等基础设施
- `libswscale`：像素格式转换与缩放（例如 YUV → RGB）

---

## 2. 最小解码骨架（C++）

```cpp
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <cstdio>

static void fail(int err, const char* what)
{
    char buf[AV_ERROR_MAX_STRING_SIZE]{};
    av_strerror(err, buf, sizeof(buf));
    std::fprintf(stderr, "%s: %s\n", what, buf);
}

int main()
{
    const char* path = "input.mp4";

    AVFormatContext* fmt = nullptr;
    int err = avformat_open_input(&fmt, path, nullptr, nullptr);
    if (err < 0) { fail(err, "avformat_open_input"); return 1; }

    err = avformat_find_stream_info(fmt, nullptr);
    if (err < 0) { fail(err, "avformat_find_stream_info"); return 1; }

    int videoStream = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoStream < 0) { fail(videoStream, "av_find_best_stream"); return 1; }

    AVStream* st = fmt->streams[videoStream];
    const AVCodec* dec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!dec) { std::fprintf(stderr, "avcodec_find_decoder failed\n"); return 1; }

    AVCodecContext* decCtx = avcodec_alloc_context3(dec);
    if (!decCtx) { std::fprintf(stderr, "avcodec_alloc_context3 failed\n"); return 1; }

    err = avcodec_parameters_to_context(decCtx, st->codecpar);
    if (err < 0) { fail(err, "avcodec_parameters_to_context"); return 1; }

    err = avcodec_open2(decCtx, dec, nullptr);
    if (err < 0) { fail(err, "avcodec_open2"); return 1; }

    AVPacket* pkt = av_packet_alloc();
    AVFrame* frm = av_frame_alloc();
    if (!pkt || !frm) { std::fprintf(stderr, "alloc packet/frame failed\n"); return 1; }

    int frameCount = 0;
    while ((err = av_read_frame(fmt, pkt)) >= 0) {
        if (pkt->stream_index != videoStream) {
            av_packet_unref(pkt);
            continue;
        }

        err = avcodec_send_packet(decCtx, pkt);
        av_packet_unref(pkt);
        if (err < 0) { fail(err, "avcodec_send_packet"); break; }

        while (true) {
            err = avcodec_receive_frame(decCtx, frm);
            if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) break;
            if (err < 0) { fail(err, "avcodec_receive_frame"); goto done; }

            ++frameCount;
            std::printf("frame %d: w=%d h=%d format=%d pts=%lld\n",
                        frameCount, frm->width, frm->height, frm->format,
                        static_cast<long long>(frm->pts));

            av_frame_unref(frm);
        }
    }

    err = avcodec_send_packet(decCtx, nullptr);
    if (err >= 0) {
        while (true) {
            err = avcodec_receive_frame(decCtx, frm);
            if (err == AVERROR_EOF || err == AVERROR(EAGAIN)) break;
            if (err < 0) { fail(err, "avcodec_receive_frame(flush)"); break; }
            ++frameCount;
            av_frame_unref(frm);
        }
    }

done:
    av_frame_free(&frm);
    av_packet_free(&pkt);
    avcodec_free_context(&decCtx);
    avformat_close_input(&fmt);
    return 0;
}
```

---

## 3. 下一步：把 AVFrame 变成可显示的数据

解码拿到的 `AVFrame` 常见是 YUV 平面格式（例如 `yuv420p`）。如果你要：

- 在 CPU 侧保存 PNG/JPEG：通常需要转成 RGB
- 上传给 GPU 贴图：要么用 YUV 采样着色器，要么先转 RGB 再上传

对应的工具库就是 `libswscale`。工程里先把“解码帧计数正常”跑通，再做像素格式转换，定位问题会简单很多。

---

## 4. 时间戳：把 pts 变成“秒”才好调试

你打印出来的 `frm->pts` 不是秒，它要结合 `time_base`。

一般从 `AVStream` 拿 `time_base`（解码阶段很多字段都用它）：

```
seconds = pts * stream_time_base
```

排错口径：

- 一旦你要做 seek、同步、抽帧、音画对齐，请先把时间戳统一成“秒”再思考
- 不要只看 `pts` 的整数值

---

## 5. 工程化的下一步（你可以按这个路线继续扩）

- **像素格式转换**：用 `libswscale` 把 YUV 转成 RGB(A)，用于保存或上传 GPU
- **音频解码**：找音频流，用同样的 send/receive 拿到采样帧
- **封装输出**：`libavformat` 创建输出容器，`libavcodec` 编码，再写出 packet

对应的命令行直觉可以先在这两章里建立起来，再回到 libav 实现：

- [04-概念总览：容器、编码、流、时间基、像素格式](04-概念总览-容器编码流时间基像素格式.md)
- [07-ffprobe 与排错：流选择（map）、同步、常见报错](07-ffprobe与排错-流选择map-同步-常见报错.md)
