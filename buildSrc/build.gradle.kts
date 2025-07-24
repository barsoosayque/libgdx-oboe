plugins {
    `java-gradle-plugin`
    `kotlin-dsl`
    `kotlin-dsl-precompiled-script-plugins`
}

repositories {
    mavenLocal()
    mavenCentral()
    google()
    maven { url = uri("https://oss.sonatype.org/content/repositories/releases/") }
}

object PluginsVersions {
    const val ANDROID = "7.2.1"
    const val KOTLIN = "1.7.20"
}

dependencies {
    implementation("com.android.tools.build:gradle:${PluginsVersions.ANDROID}")
    implementation("org.jetbrains.kotlin:kotlin-gradle-plugin:${PluginsVersions.KOTLIN}")
}
