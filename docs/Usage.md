## Usage

- TODO (Maven packages)

Right now, you can only build aar, and then manualy add it to your project.

Since **Libgdx** does not allow you to inherit from an *AndroidAudio* (and this is the only class that can be overloaded in your Android Launcher) class, the only workaround to it is just to mimic defualt *audio* behaviour:

```kotlin
class AndroidLauncher : AndroidApplication() {
    // Audio replacement
    lateinit var oboeAudio: OboeAudio
    
    // Android lifecycle related variables
    private var focusWasChanged: Boolean = true
    private var resumeAudio: Boolean = true

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // NOTE: You can create OboeAudio only after or during onCreate
        oboeAudio = OboeAudio(assets)
        oboeAudioListener()
        
        // Create an app like always
        val config = AndroidApplicationConfiguration()
        initialize(SomeApp(), config)
    }

    // <><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    // "Replace default android audio with the custom one" the Hack
    // Basically just mimic base class behavior
    // <><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    
    private fun oboeAudioListener() {
        addLifecycleListener(object : LifecycleListener {
            override fun pause() { oboeAudio.stop() }
            override fun dispose() { oboeAudio.dispose() }
            override fun resume() {}
        })
    }
    

    override fun getAudio(): Audio = oboeAudio

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        focusWasChanged = hasFocus
        if (hasFocus && resumeAudio) {
            oboeAudio.resume()
            resumeAudio = false
        }
    }

    override fun onResume() {
        resumeAudio = true
        if (focusWasChanged) {
            oboeAudio.resume()
            resumeAudio = false
        }
        super.onResume()
    }
}
```

And that's basically it. All *Sound* and *Music* instances are "patched" with **Libgdx Oboe** now.
