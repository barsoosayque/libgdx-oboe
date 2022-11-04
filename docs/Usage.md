## Usage

### Package

As for now, there is no place to get the library artifact for your app. So you would need to build it locally.
First, set up NDK in your development environment (Android Studio can do that for you, for example).
Second, clone this repository and add it to your build:
```groovy
// settings.gradle.kts
include(":app")
include(":libgdx-oboe")
project(":libgdx-oboe").projectDir = file("/path/to/cloned/library")

// build.gradle.kts
dependenices {
    implementations(":libgdx-oboe")
}
```

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
