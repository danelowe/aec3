cmake_minimum_required(VERSION 3.25)
project(aec3)

set(CMAKE_CXX_STANDARD 17)
if (APPLE)
    add_definitions(-DWEBRTC_MAC -DWEBRTC_POSIX -DWEBRTC_APM_DEBUG_DUMP=0)
elseif (UNIX)
    add_definitions(-DWEBRTC_LINUX -DWEBRTC_POSIX -DWEBRTC_APM_DEBUG_DUMP=0)
endif ()


include_directories(aec3)
include_directories(abseil-cpp)
include_directories(webrtc)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set (X86 TRUE)
else ()
    set (X86 FALSE)
endif ()

if (X86)
    set(X86_SOURCES
            webrtc/system_wrappers/source/cpu_features.cc
            webrtc/common_audio/third_party/ooura/fft_size_128/ooura_fft_sse2.cc
            webrtc/common_audio/resampler/sinc_resampler_sse.cc
    )
    add_library(aec3_avx2
            webrtc/common_audio/resampler/sinc_resampler_avx2.cc
            webrtc/modules/audio_processing/aec3/adaptive_fir_filter_avx2.cc
            webrtc/modules/audio_processing/aec3/fft_data_avx2.cc
            webrtc/modules/audio_processing/aec3/matched_filter_avx2.cc
            webrtc/modules/audio_processing/aec3/vector_math_avx2.cc
            webrtc/modules/audio_processing/aec3/adaptive_fir_filter_erl_avx2.cc
    )
    target_compile_options(aec3_avx2 PRIVATE "-mavx2")
    target_compile_options(aec3_avx2 PRIVATE "-mfma")
endif ()

set(SOURCES
        aec3/main.cc
        aec3/wavreader.c
        aec3/wavwriter.c
        aec3/print_tool.cc

        abseil-cpp/absl/strings/string_view.cc
        abseil-cpp/absl/strings/string_view.cc
        webrtc/api/audio/audio_frame.cc
        webrtc/api/audio/channel_layout.cc
        webrtc/api/audio/echo_canceller3_config.cc
        webrtc/api/audio/echo_canceller3_factory.cc
        webrtc/common_audio/audio_util.cc
        webrtc/common_audio/include/audio_util.h
        webrtc/common_audio/resampler/push_sinc_resampler.cc
        webrtc/common_audio/resampler/sinc_resampler.cc
        webrtc/common_audio/signal_processing/include/signal_processing_library.h
        webrtc/common_audio/signal_processing/splitting_filter.c
        webrtc/common_audio/third_party/ooura/fft_size_128/ooura_fft.cc
        webrtc/modules/audio_processing/aec3/adaptive_fir_filter.cc
        webrtc/modules/audio_processing/aec3/adaptive_fir_filter_erl.cc
        webrtc/modules/audio_processing/aec3/aec3_common.cc
        webrtc/modules/audio_processing/aec3/aec3_fft.cc
        webrtc/modules/audio_processing/aec3/aec_state.cc
        webrtc/modules/audio_processing/aec3/alignment_mixer.cc
        webrtc/modules/audio_processing/aec3/api_call_jitter_metrics.cc
        webrtc/modules/audio_processing/aec3/block_buffer.cc
        webrtc/modules/audio_processing/aec3/block_delay_buffer.cc
        webrtc/modules/audio_processing/aec3/block_framer.cc
        webrtc/modules/audio_processing/aec3/block_processor.cc
        webrtc/modules/audio_processing/aec3/block_processor_metrics.cc
        webrtc/modules/audio_processing/aec3/clockdrift_detector.cc
        webrtc/modules/audio_processing/aec3/coarse_filter_update_gain.cc
        webrtc/modules/audio_processing/aec3/comfort_noise_generator.cc
        webrtc/modules/audio_processing/aec3/config_selector.cc
        webrtc/modules/audio_processing/aec3/decimator.cc
        webrtc/modules/audio_processing/aec3/dominant_nearend_detector.cc
        webrtc/modules/audio_processing/aec3/downsampled_render_buffer.cc
        webrtc/modules/audio_processing/aec3/echo_audibility.cc
        webrtc/modules/audio_processing/aec3/echo_canceller3.cc
        webrtc/modules/audio_processing/aec3/echo_path_delay_estimator.cc
        webrtc/modules/audio_processing/aec3/echo_path_variability.cc
        webrtc/modules/audio_processing/aec3/echo_remover.cc
        webrtc/modules/audio_processing/aec3/echo_remover_metrics.cc
        webrtc/modules/audio_processing/aec3/erl_estimator.cc
        webrtc/modules/audio_processing/aec3/erle_estimator.cc
        webrtc/modules/audio_processing/aec3/fft_buffer.cc
        webrtc/modules/audio_processing/aec3/filter_analyzer.cc
        webrtc/modules/audio_processing/aec3/frame_blocker.cc
        webrtc/modules/audio_processing/aec3/fullband_erle_estimator.cc
        webrtc/modules/audio_processing/aec3/matched_filter.cc
        webrtc/modules/audio_processing/aec3/matched_filter_lag_aggregator.cc
        webrtc/modules/audio_processing/aec3/moving_average.cc
        webrtc/modules/audio_processing/aec3/multi_channel_content_detector.cc
        webrtc/modules/audio_processing/aec3/refined_filter_update_gain.cc
        webrtc/modules/audio_processing/aec3/render_buffer.cc
        webrtc/modules/audio_processing/aec3/render_delay_buffer.cc
        webrtc/modules/audio_processing/aec3/render_delay_controller.cc
        webrtc/modules/audio_processing/aec3/render_delay_controller_metrics.cc
        webrtc/modules/audio_processing/aec3/render_signal_analyzer.cc
        webrtc/modules/audio_processing/aec3/residual_echo_estimator.cc
        webrtc/modules/audio_processing/aec3/reverb_decay_estimator.cc
        webrtc/modules/audio_processing/aec3/reverb_frequency_response.cc
        webrtc/modules/audio_processing/aec3/reverb_model.cc
        webrtc/modules/audio_processing/aec3/reverb_model_estimator.cc
        webrtc/modules/audio_processing/aec3/signal_dependent_erle_estimator.cc
        webrtc/modules/audio_processing/aec3/spectrum_buffer.cc
        webrtc/modules/audio_processing/aec3/stationarity_estimator.cc
        webrtc/modules/audio_processing/aec3/subband_erle_estimator.cc
        webrtc/modules/audio_processing/aec3/subband_nearend_detector.cc
        webrtc/modules/audio_processing/aec3/subtractor.cc
        webrtc/modules/audio_processing/aec3/subtractor_output.cc
        webrtc/modules/audio_processing/aec3/subtractor_output_analyzer.cc
        webrtc/modules/audio_processing/aec3/suppression_filter.cc
        webrtc/modules/audio_processing/aec3/suppression_gain.cc
        webrtc/modules/audio_processing/aec3/transparent_mode.cc
        webrtc/modules/audio_processing/audio_buffer.cc
        webrtc/modules/audio_processing/high_pass_filter.cc
        webrtc/modules/audio_processing/logging/apm_data_dumper.cc
        webrtc/modules/audio_processing/splitting_filter.cc
        webrtc/modules/audio_processing/three_band_filter_bank.cc
        webrtc/modules/audio_processing/utility/cascaded_biquad_filter.cc
        webrtc/rtc_base/checks.cc
        webrtc/rtc_base/experiments/field_trial_parser.cc
        webrtc/rtc_base/logging.cc
        webrtc/rtc_base/memory/aligned_malloc.cc
        webrtc/rtc_base/platform_thread_types.cc
        webrtc/rtc_base/race_checker.cc
        webrtc/rtc_base/string_encode.cc
        webrtc/rtc_base/string_utils.cc
        webrtc/rtc_base/strings/string_builder.cc
        webrtc/rtc_base/system_time.cc
        webrtc/rtc_base/time_utils.cc
        webrtc/system_wrappers/source/field_trial.cc
        webrtc/system_wrappers/source/metrics.cc

        ${X86_SOURCES}

)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation")
endif()

if (X86)
    target_link_libraries(aec3 aec3_avx2)
endif()
