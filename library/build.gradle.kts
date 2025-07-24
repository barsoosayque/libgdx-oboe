plugins {
    id("kotlin-android")
    id("maven-publish")
    id("com.android.library")
}

dependencies {
    implementation(Dependencies.GDX)
    implementation(Dependencies.GDX_BACKEND_ANDROID)
    implementation(Dependencies.KOTLIN_STDLIB)
}

android {
    buildToolsVersion = AndroidConfig.BUILD_TOOLS
    compileSdkVersion = "android-${AndroidConfig.SDK}"
    ndkVersion = AndroidConfig.NDK
    namespace = AppConfig.PUBLISHED_GROUP_ID

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    sourceSets {
        getByName("main") {
            manifest.srcFile("AndroidManifest.xml")
            java.srcDir("src/kotlin")
        }
    }

    defaultConfig {
        minSdk = AndroidConfig.MIN_SDK

        ndk {
            abiFilters.addAll(listOf("x86", "x86_64", "armeabi-v7a", "arm64-v8a"))

            externalNativeBuild {
                cmake {
                    cppFlags("-std=c++17", "-O3", "-DCMAKE_BUILD_TYPE=Release", "-DNDEBUG")
                }
            }

            consumerProguardFile("proguard-rules.pro")
        }
    }

    externalNativeBuild {
        cmake {
            path("CMakeLists.txt")
        }
    }
}
