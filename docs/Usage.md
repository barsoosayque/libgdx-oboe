## Usage

### Package

*TBD*

### Launcher

Simply override *createAudio* method in your android launcher class to create an instance of *OboeAudio*:

Java:
```java
public class AndroidLauncher extends AndroidApplication {
    // Magic happens here:
    @Override
    public AndroidAudio createAudio(Context context, AndroidApplicationConfiguration config) {
        return new OboeAudio(context.getAssets());
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        AndroidApplicationConfiguration config = new AndroidApplicationConfiguration();
        initialize(new SomeApp(), config);
    }
}
```

Kotlin:
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

And that's basically it. All *Sound* and *Music* instances are "patched" with **libGDX Oboe** now.
