
//by xsh 
//2022/03/17
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <string>

#include "api/audio/audio_frame.h"
#include "api/audio/echo_canceller3_config.h"
#include "api/audio/echo_canceller3_factory.h"
#include "print_tool.h"
#include "modules/audio_processing/audio_buffer.h"
#include "modules/audio_processing/high_pass_filter.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "wavreader.h"
#include "wavwriter.h"

using namespace webrtc;
using namespace std;

void print_wav_information(const char* fn, int format, int channels, int sample_rate, int bits_per_sample, int length)
{
    cout << "=====================================" << endl
        << fn << " information:" << endl
        << "format: " << format << endl
        << "channels: " << channels << endl
        << "sample_rate: " << sample_rate << endl
        << "bits_per_sample: " << bits_per_sample << endl
        << "length: " << length << endl
        << "total_samples: " << length / bits_per_sample * 8 << endl
        << "======================================" << endl;
}

int main(int argc, char* argv[])
{
    
    if (argc != 4)
    {
        cerr << "usage: ./aec3 near.wav far.wav out.wav" << endl;
        return -1;
    }
    cout << "======================================" << endl
        << "near: " << argv[1] << endl
        << "far:  " << argv[2] << endl
        << "out:  " << argv[3] << endl
        << "======================================" << endl;
        

    void* h_ref = wav_read_open(argv[2]);
    void* h_rec = wav_read_open(argv[1]);

    int ref_format, ref_channels, ref_sample_rate, ref_bits_per_sample;
    int rec_format, rec_channels, rec_sample_rate, rec_bits_per_sample;
    unsigned int ref_data_length, rec_data_length;

    int res = wav_get_header(h_ref, &ref_format, &ref_channels, &ref_sample_rate, &ref_bits_per_sample, &ref_data_length);
    if (!res)
    {
        cerr << "get ref header error: " << res << endl;
        return -1;
    }
    int ref_samples = ref_data_length * 8 / ref_bits_per_sample;
    print_wav_information(argv[2], ref_format, ref_channels, ref_sample_rate, ref_bits_per_sample, ref_data_length);

    res = wav_get_header(h_rec, &rec_format, &rec_channels, &rec_sample_rate, &rec_bits_per_sample, &rec_data_length);
    if (!res)
    {
        cerr << "get rec header error: " << res << endl;
        return -1;
    }
    int rec_samples = rec_data_length * 8 / rec_bits_per_sample;
    print_wav_information(argv[1], rec_format, rec_channels, rec_sample_rate, rec_bits_per_sample, rec_data_length);

    if (ref_format != rec_format ||
        ref_channels != rec_channels ||
        ref_sample_rate != rec_sample_rate ||
        ref_bits_per_sample != rec_bits_per_sample)
    {
        cerr << "ref file format != rec file format" << endl;
        return -1;
    }

    EchoCanceller3Config aec_config;
    aec_config.filter.export_linear_aec_output = true;
    aec_config.delay.use_external_delay_estimator = true;
    EchoCanceller3Factory aec_factory = EchoCanceller3Factory(aec_config);
    std::unique_ptr<EchoControl> echo_controler = aec_factory.Create(ref_sample_rate, ref_channels, rec_channels);
    std::unique_ptr<HighPassFilter> hp_filter = std::make_unique<HighPassFilter>(rec_sample_rate, rec_channels);

    int sample_rate = rec_sample_rate;
    int channels = rec_channels;
    int bits_per_sample = rec_bits_per_sample;
    StreamConfig config = StreamConfig(sample_rate, channels);

    std::unique_ptr<AudioBuffer> ref_audio = std::make_unique<AudioBuffer>(
        config.sample_rate_hz(), config.num_channels(),
        config.sample_rate_hz(), config.num_channels(),
        config.sample_rate_hz(), config.num_channels());
    std::unique_ptr<AudioBuffer> aec_audio = std::make_unique<AudioBuffer>(
        config.sample_rate_hz(), config.num_channels(),
        config.sample_rate_hz(), config.num_channels(),
        config.sample_rate_hz(), config.num_channels());
    constexpr int kLinearOutputRateHz = 16000;
    std::unique_ptr<AudioBuffer> aec_linear_audio = std::make_unique<AudioBuffer>(
        kLinearOutputRateHz, config.num_channels(),
        kLinearOutputRateHz, config.num_channels(),
        kLinearOutputRateHz, config.num_channels());

    AudioFrame ref_frame, aec_frame;

    void* h_out = wav_write_open(argv[3], rec_sample_rate, rec_bits_per_sample, rec_channels);
    void* h_linear_out = wav_write_open("linear.wav", kLinearOutputRateHz, rec_bits_per_sample, rec_channels);

    int samples_per_frame = sample_rate / 100;
    int bytes_per_frame = samples_per_frame * bits_per_sample / 8;
    int total = rec_samples < ref_samples ? rec_samples / samples_per_frame : rec_samples / samples_per_frame;

    int current = 0;
    unsigned char* ref_tmp = new unsigned char[bytes_per_frame];
    unsigned char* aec_tmp = new unsigned char[bytes_per_frame];
    cout << "processing audio frames ..." << endl;
    ProgressBar bar;
    while (current++ < total) 
    {
        wav_read_data(h_ref, ref_tmp, bytes_per_frame);
        wav_read_data(h_rec, aec_tmp, bytes_per_frame);

        ref_frame.UpdateFrame(0, reinterpret_cast<int16_t*>(ref_tmp), samples_per_frame, sample_rate, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);
        aec_frame.UpdateFrame(0, reinterpret_cast<int16_t*>(aec_tmp), samples_per_frame, sample_rate, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);

        ref_audio->CopyFrom(reinterpret_cast<int16_t*>(ref_tmp), config);
        aec_audio->CopyFrom(reinterpret_cast<int16_t*>(aec_tmp), config);

        ref_audio->SplitIntoFrequencyBands();
        echo_controler->AnalyzeRender(ref_audio.get());
        ref_audio->MergeFrequencyBands();
        echo_controler->AnalyzeCapture(aec_audio.get());
        aec_audio->SplitIntoFrequencyBands();
        hp_filter->Process(aec_audio.get(), true);
        echo_controler->SetAudioBufferDelay(0);
        echo_controler->ProcessCapture(aec_audio.get(), aec_linear_audio.get(), false);
        aec_audio->MergeFrequencyBands();

        aec_audio->CopyTo(config, reinterpret_cast<int16_t* const>(aec_tmp));
        wav_write_data(h_out, aec_tmp, bytes_per_frame);

        aec_frame.UpdateFrame(0, nullptr, kLinearOutputRateHz / 100, kLinearOutputRateHz, AudioFrame::kNormalSpeech, AudioFrame::kVadActive, 1);
        aec_linear_audio->CopyTo(config, reinterpret_cast<int16_t* const>(aec_tmp));
        wav_write_data(h_linear_out, aec_tmp, bytes_per_frame);
        bar.print_bar(current * 1.f / total);
    }
    std::cout << std::endl;
    delete[] ref_tmp;
    delete[] aec_tmp;

    wav_read_close(h_ref);
    wav_read_close(h_rec);
    wav_write_close(h_out);
    wav_write_close(h_linear_out);

    return 0;
}
