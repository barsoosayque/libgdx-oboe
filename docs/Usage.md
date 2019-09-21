## Usage

### Package

You can use *jcenter* repository to find this library and use it as a dependency:

```groovy
ext {
    libgdxoboe_version = "0.1.4"
}

repositories {
    jcenter()
    ...
}

dependencies {
    implementation "com.gitlab.barsoosayque:libgdxoboe:$libgdxoboe_version"
}
```

### Launcher

Since **libGDX** does not allow you to inherit from an *AndroidAudio* (and this is the only class that can be overloaded in your Android Launcher) class, the only workaround to it is just to mimic defualt *audio* behaviour.

But you don't have to do that. If your intentions to just utilize the library, then you can use special *OboeAndroidApplication* class instead of the default one:

Java:
```java
public class AndroidLauncher extends OboeAndroidApplication {
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

And that's basically it. All *Sound* and *Music* instances are "patched" with **libGDX Oboe** now.
