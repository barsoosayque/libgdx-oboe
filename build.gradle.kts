allprojects {
    apply(plugin = "idea")

    repositories {
        mavenLocal()
        mavenCentral()
        google()
        maven { url = uri("https://oss.sonatype.org/content/repositories/releases/") }
    }
}