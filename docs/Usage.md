## Usage

- TODO (Maven packages)

Right now, you can only build aar, and then manualy add it to your project.

Since **Libgdx** does not allow you to inherit from an *AndroidAudio* (and this is the only class that can be overloaded in your Android Launcher) class, the only workaround to it is just to mimic defualt *audio* behaviour.

But you don't have to do that. If your intentions to just utilize the library, then you can use special *OboeAndroidApplication* class instead of the default one:

```kotlin
// Note super class.
class AndroidLauncher : OboeAndroidApplication() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Create an app like always
        val config = AndroidApplicationConfiguration()
        initialize(SomeApp(), config)
    }
}
```

And that's basically it. All *Sound* and *Music* instances are "patched" with **Libgdx Oboe** now.
