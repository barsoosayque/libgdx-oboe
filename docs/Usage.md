## Usage

### Package

You can use *jcenter* repository to resolve this library and use it as a dependency:

```groovy
ext {
    libgdxoboe_version = "0.2.3"
}

repositories {
    jcenter()
    ...
}

dependencies {
    implementation "com.github.barsoosayque:libgdxoboe:$libgdxoboe_version"
}
```

Note *natives* configuration. This is libGDX specific configuration for native libraries, it's used by `copyAndroidNatives` task.

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
