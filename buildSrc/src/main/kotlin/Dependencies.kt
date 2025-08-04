object DependenciesVersions {
    const val KOTLIN = "2.2.0"
    const val KTX = "1.13.1-rc1"
    const val GDX = "1.13.1"
}

object Dependencies {
    const val GDX_BACKEND_ANDROID = "com.badlogicgames.gdx:gdx-backend-android:${DependenciesVersions.GDX}"
    const val GDX = "com.badlogicgames.gdx:gdx:${DependenciesVersions.GDX}"
    const val KOTLIN_STDLIB = "org.jetbrains.kotlin:kotlin-stdlib:${DependenciesVersions.KOTLIN}"
    const val KTX_SCENE2D = "io.github.libktx:ktx-scene2d:${DependenciesVersions.KTX}"
    const val KTX_ACTORS = "io.github.libktx:ktx-actors:${DependenciesVersions.KTX}"
    const val GDX_NATIVES_ARMEABI_V7A = "com.badlogicgames.gdx:gdx-platform:${DependenciesVersions.GDX}:natives-armeabi-v7a"
    const val GDX_NATIVES_ARM64_V8A = "com.badlogicgames.gdx:gdx-platform:${DependenciesVersions.GDX}:natives-arm64-v8a"
    const val GDX_NATIVES_X86 = "com.badlogicgames.gdx:gdx-platform:${DependenciesVersions.GDX}:natives-x86"
    const val GDX_NATIVES_X86_64 = "com.badlogicgames.gdx:gdx-platform:${DependenciesVersions.GDX}:natives-x86_64"
}
