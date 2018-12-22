## Libgdx Oboe

An audio component replacement for [**Libgdx**][libgdx] library, which allows music to flow without any delay or another nasty Android-related distortion. 

And if you ever asked yourself *"Why is my Libgdx app music is so broken ?"*, then you came to the right place.

(Well, you also have to wait for it, because this library is in "work in progress" stage) 

### Documentation:

- ⚙️ [Usage][usage]
- 🎹 [Examples][examples]
- 🛠 [Build][build]

*^ Content of `docs` directory.*

### Progress:

**OboeAudio** class:

- [x] `newSound` method;
- [x] `newMusic` method;
- [ ] `newAudioDevice` method;
- [ ] `newAudioRecorder` method.

**OboeSound** class:

- [x] `play` methods (except pan and pitch);
- [x] `loop` methods (except pan and pitch);
- [x] `pause` methods;
- [x] `resume` methods;
- [x] `stop` methods;
- [x] `dispose` method;
- [x] `setLooping` method;
- [x] `setVolume` method;
- [ ] `setPitch` method;
- [ ] `setPan` method.

**OboeMusic** class:

- [x] `play` method;
- [x] `pause` method;
- [x] `stop` method;
- [x] `getPosition` method;
- [x] `setPosition` method;
- [x] `getVolume` method;
- [x] `setVolume` method;
- [ ] `isLooping` method;
- [ ] `setLooping` method;
- [x] `isPlaying` method;
- [ ] `setPan` method;
- [ ] `setOnCompletionListener` method;
- [x] `dispose` method.

*WARN: not marked methods will throw an exception.*

[libgdx]: https://github.com/libgdx/libgdx
[usage]: /docs/Usage.md
[examples]: /docs/Examples.md
[build]: /docs/Build.md

