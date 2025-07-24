plugins {
    id("kotlin-android")
    id("com.android.application")
}

val natives by configurations.creating
val libDir = "../common/libs"

dependencies {
    implementation(project(":library"))
    implementation(Dependencies.GDX_BACKEND_ANDROID)
    implementation(Dependencies.GDX)
    implementation(Dependencies.KOTLIN_STDLIB)
    implementation(Dependencies.KTX_SCENE2D)
    implementation(Dependencies.KTX_ACTORS)
    natives(Dependencies.GDX_NATIVES_ARMEABI_V7A)
    natives(Dependencies.GDX_NATIVES_ARM64_V8A)
    natives(Dependencies.GDX_NATIVES_X86)
    natives(Dependencies.GDX_NATIVES_X86_64)
}

android {
    buildToolsVersion = AndroidConfig.BUILD_TOOLS
    compileSdkVersion = "android-${AndroidConfig.SDK}"

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    sourceSets {
        getByName("main") {
            assets.srcDir("../common/assets")
            jniLibs.srcDir(libDir)
            manifest.srcFile("AndroidManifest.xml")
            java.srcDir("src")
            res.srcDir("res")
        }
    }

    defaultConfig {
        minSdk = AndroidConfig.MIN_SDK
        targetSdk = AndroidConfig.SDK

    }
}

tasks {
    // called every time gradle gets executed, takes the native dependencies of
    // the natives configuration, and extracts them to the proper libs folders
    // so they get packed with the APK.
    val copyAndroidNatives by registering {
        doFirst {
            file("${libDir}/armeabi-v7a/").mkdirs()
            file("${libDir}/arm64-v8a/").mkdirs()
            file("${libDir}/x86_64/").mkdirs()
            file("${libDir}/x86/").mkdirs()

            configurations.getByName("natives").files.forEach { jar ->
                val outputDir: String? =
                    if (jar.name.endsWith("natives-arm64-v8a.jar")) "${libDir}/arm64-v8a"
                    else if (jar.name.endsWith("natives-armeabi-v7a.jar"))
                        "${libDir}/armeabi-v7a"
                    else if (jar.name.endsWith("natives-x86_64.jar")) "${libDir}/x86_64"
                    else if (jar.name.endsWith("natives-x86.jar")) "${libDir}/x86"
                    else null

                if (outputDir != null) {
                    copy {
                        from(zipTree(jar))
                        into(outputDir)
                        include("*.so")
                    }
                }
            }
        }
    }

    val cleanNatives by registering {
        file("${libDir}/armeabi-v7a/").delete()
        file("${libDir}/arm64-v8a/").delete()
        file("${libDir}/x86_64/").delete()
        file("${libDir}/x86/").delete()
    }

    whenTaskAdded {
        if (name.contains("package")) {
            dependsOn(copyAndroidNatives)
        }
        if (name.contains("clean")) {
            dependsOn(cleanNatives)
        }
    }
}
