#include <iostream>
#include <chrono>
#include <thread>
#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

AVFormatContext *i_fmt_ctx;
AVStream *i_video_stream;
AVFormatContext *o_fmt_ctx;
AVStream *o_video_stream;
bool bStop = false;
int frame_nums = 0;

int Record(std::string in, std::string out)
{
    // avcodec_register_all();
    // av_register_all();
    avformat_network_init();

    /* should set to NULL so that avformat_open_input() allocate a new one */
    i_fmt_ctx = NULL;

    if (avformat_open_input(&i_fmt_ctx, in.c_str(), NULL, NULL) != 0)
    {
        fprintf(stderr, " = could not open input file\n");
        return -1;
    }

    if (avformat_find_stream_info(i_fmt_ctx, NULL) < 0)
    {
        fprintf(stderr, " = could not find stream info\n");
        return -1;
    }

    /* find first video stream */
    for (unsigned i = 0; i < i_fmt_ctx->nb_streams; i++)
    {
        if (i_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            i_video_stream = i_fmt_ctx->streams[i];
            break;
        }
    }
    if (i_video_stream == NULL)
    {
        fprintf(stderr, " = didn't find any video stream\n");
        return -1;
    }

    avformat_alloc_output_context2(&o_fmt_ctx, NULL, NULL, out.c_str());

    o_video_stream = avformat_new_stream(o_fmt_ctx, NULL);
    avio_open(&o_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
    avformat_write_header(o_fmt_ctx, NULL);

    while (!bStop)
    {
        AVPacket *i_pkt = av_packet_alloc();
        i_pkt->size = 0;
        i_pkt->data = NULL;
        if (av_read_frame(i_fmt_ctx, i_pkt) < 0)
            break;
        /*
         * pts and dts should increase monotonically
         * pts should be >= dts
         */
        i_pkt->flags |= AV_PKT_FLAG_KEY;
        i_pkt->stream_index = 0;

        static int num = 1;
        printf(" = frame %d\n", num++);
        av_interleaved_write_frame(o_fmt_ctx, i_pkt);
        if (frame_nums > 2000)
        {
            bStop = true;
        }
        frame_nums++;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20ms);
        av_packet_free(&i_pkt);
    }

    avformat_close_input(&i_fmt_ctx);

    av_write_trailer(o_fmt_ctx);

    avcodec_close(o_fmt_ctx->streams[0]->codec);
    av_freep(&o_fmt_ctx->streams[0]->codec);
    av_freep(&o_fmt_ctx->streams[0]);

    avio_close(o_fmt_ctx->pb);
    av_free(o_fmt_ctx);
    return 0;
}

int main()
{
    Record("rtsp://admin:hk123456@192.168.104.72:554/Streaming/Channels/101",
           "111.mp4");
    return 0;
}
