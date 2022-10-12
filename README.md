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

<table><thead><tr><th rowspan="2">Feature</th><th colspan="2">WAV</th><th colspan="2">OGG</th><th colspan="2">MP3</th></tr><tr><td>🧮</td><td><em>State</em></td><td>🧮</td><td><em>State</em></td><td>🧮</td><td><em>State</em></td></tr></thead><tbody><tr><td><em>Seek</em></td><td>⭐️</td><td>Precise, no content lost.</td><td>👌</td><td>Bad initial seek, but dropping frames until PTS is equal to desired seek TS. May lose some content if PTS isn't precise enough.</td><td>👌</td><td>Same as OGG.</td></tr></tbody></table>

## Progress:

**OboeAudio** class:

- [x] `newSound` method (With *OboeSound*);
- [x] `newMusic` method (With *OboeMusic*);
- [x] `newAudioDevice` method (With *OboeAudioDevice*);
- [ ] `newAudioRecorder` method.

*WARN: not marked methods will result in undefined behaviour. (actually, just a crash)*

[oboe]: https://github.com/google/oboe
[libgdx]: https://github.com/libgdx/libgdx
[usage]: /docs/Usage.md
[examples]: /docs/Examples.md
[build]: /docs/Build.md
