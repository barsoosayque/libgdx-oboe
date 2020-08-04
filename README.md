<table align="center"><tr><td align="center" width="9999">
<img src="/icon.png" align="center" width="150" alt="Icon">

# - libGDX Oboe -

[![Download](https://api.bintray.com/packages/barsoosayque/github/libgdx-oboe/images/download.svg)](https://bintray.com/barsoosayque/github/libgdx-oboe/_latestVersion)
[![libGDX](https://img.shields.io/badge/libGDX-1.9.11-yellow.svg)](https://github.com/libgdx/libgdx/releases/tag/gdx-parent-1.9.11)
[![oboe](https://img.shields.io/badge/oboe-1.4.2-yellow.svg)](https://github.com/google/oboe/releases/tag/1.4.2)

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
// Note the super class
class AndroidLauncher : OboeAndroidApplication() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        initialize(AmazingMusicApp(), AndroidApplicationConfiguration())
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

| Feature | WAV | OGG | MP3 |
|-|-|-|-|
| Seek | <table><tr><td>⭐️</td><td>Precise, no content lost.</td></tr></table>  | <table><tr><td>👌</td><td>Bad initial seek, but dropping frames until PTS is equal to desired seek TS. May lose some content if PTS isn't precise enough.</td></tr></table> | <table><tr><td>👌</td><td>Same as OGG.</td></tr></table> |

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
