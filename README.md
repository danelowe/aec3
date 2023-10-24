# aec3

Wrapper around WebRTC's AEC3 processor, for removing acoustic echo from audio files. 

Currently supports .wav files. 

```bash
./aec3 near.wav far.wav out.wav
```

- `near.wav` is the recording to process.
- `far.wav` is the far side where the echo comes from.
- `out.wav` is the output file with the echo removed.

Based on [webrtc_AEC3](https://github.com/xishaoheng/webrtc_AEC3), [AEC3](https://github.com/ewan-xu/AEC3), and [wav-aec](https://github.com/lschilli/wav-aec)

## Publish to npm

- build the binaries and place in nodejs/bin
- build the node package
 
```bash
npm publish --access public
```