allprojects {
    apply(plugin = "idea")

    repositories {
        mavenLocal()
        mavenCentral()
        google()
        maven { url = uri("https://oss.sonatype.org/content/repositories/releases/") }
        maven { url = uri("https://kotlin.bintray.com/kotlinx") }
    }
}