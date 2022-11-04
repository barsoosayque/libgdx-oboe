<table align="center"><tr><td align="center" width="9999">
<img src="/icon.png" align="center" width="150" alt="Icon">

# libGDX Oboe

[![libGDX](https://img.shields.io/badge/libGDX-1.11.0-yellow.svg)](https://github.com/libgdx/libgdx/releases/tag/1.11.0)
[![oboe](https://img.shields.io/badge/oboe-1.6.1-yellow.svg)](https://github.com/google/oboe/releases/tag/1.6.1)

Reimplementation of the Audio interface for **libGDX** framework using Oboe and FFmpeg, which allows music to flow without any delay or another unpleasant distortion.

And if you ever asked yourself *"Why is my libGDX app music is so broken ?"*, then you've come to the right place.

</td></tr></table>

<div align="center">

[🔰 How to Use][usage] ・ [🎺 Explore Examples][examples] ・ [🚜 How to Build][build]

*Content of `docs` directory.*
</div>

## Quick, show me some code !

Here is how do one essentially "use" this library (code in Kotlin):

```kotlin
class AndroidLauncher : AndroidApplication() {
    // Magic happens here:
    override fun createAudio(context: Context, config: AndroidApplicationConfiguration): AndroidAudio =
            OboeAudio(context.assets)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Create an app like always
        val config = AndroidApplicationConfiguration()
        initialize(SomeApp(), config)
    }
}
```

## How does this works ?

Using [Oboe library][oboe] from Google, you can create high-performance audio streams which may plug in and tweak some features in runtime to boost speed even further.
This streams are native, so there is no GC that will slow things down.
For audio decoding, this library also provide native tools: libavformat and friends (FFmpeg) with MP3, OGG and WAV support only.
Such audio decoder is blazing fast, so combining that with oboe streams, we get ourselves responsive and fast Audio implementation.

## State of features

Here is a table of known goodies and problems for certain features per file format (decoder):

|Feature|State|Description|
|-------|-----|-----------|
|Sounds|⭐|Per-sound soundpools. All features should be working. Although max sounds isn't read from android config, the number is infinite at the moment.
|Music playing|⭐|Precise position, starts and pauses exactly when requested.|
|Music Seek|👌|_WAV_ is precise, no content lost. _OGG_ and _MP3_ is different: bad initial seek, but dropping frames until PTS is equal to desired seek TS. May lose some content if PTS isn't precise enough.|
|Audio Device|👌|Repetitive writes to audio device might produce audio artifacts (cracks).|
|Audio Recorder|👌|Repetitive reads bleed a little bit of cache from the last read to the buffer. Omit a few first frames when using recorded PCM.|

[oboe]: https://github.com/google/oboe
[libgdx]: https://github.com/libgdx/libgdx
[usage]: /docs/Usage.md
[examples]: /docs/Examples.md
[build]: /docs/Build.md