## Libgdx Oboe

An audio component replacement for [**Libgdx**][libgdx] library, which allows music to flow without any delay or another nasty Android-related distortion. 

And if you ever asked yourself *"Why is my Libgdx app music is so broken ?"*, then you came to the right place.

### Documentation:

- ⚙️ [Usage][usage]
- 🎹 [Examples][examples]
- 🛠 [Build][build]

*^ Content of `docs` directory.*

### Quick, show me some code !

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

### Progress:

**OboeAudio** class:

- [x] `newSound` method (With *OboeSound*);
- [x] `newMusic` method (With *OboeMusic*);
- [ ] `newAudioDevice` method;
- [ ] `newAudioRecorder` method.

*WARN: not marked methods will result in undefined behaviour. (probably, hard crash)*

[libgdx]: https://github.com/libgdx/libgdx
[usage]: /docs/Usage.md
[examples]: /docs/Examples.md
[build]: /docs/Build.md

