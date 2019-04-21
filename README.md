<table align="center"><tr><td align="center" width="9999">
<img src="/icon.png" align="center" width="150" alt="Icon">

# - libGDX Oboe -

An audio component replacement for the **libGDX** framework, which allows music to flow without any delay or another unpleasant Android-related distortion. 

And if you ever asked yourself *"Why is my libGDX app music is so broken ?"*, then you came to the right place.

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

## How is this working ?

You better ask amazing people behind [Oboe library][oboe]. In two words, Oboe uses either AAudio or OpenSL ES depending on the device API level. And library is actually trying to ensure that there will be no delay by adjusting buffers and other options. This repository is nothing but a mere Oboe wrapper for libGDX (hence, the name). 

## Progress:

**OboeAudio** class:

- [x] `newSound` method (With *OboeSound*);
- [x] `newMusic` method (With *OboeMusic*);
- [ ] `newAudioDevice` method;
- [ ] `newAudioRecorder` method.

*WARN: not marked methods will result in undefined behaviour. (probably, hard crash)*

[oboe]: https://github.com/google/oboe
[libgdx]: https://github.com/libgdx/libgdx
[usage]: /docs/Usage.md
[examples]: /docs/Examples.md
[build]: /docs/Build.md