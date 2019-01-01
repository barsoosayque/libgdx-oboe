package barsoosayque.libgdxoboe.async.content

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class MusicAsset(val path: String) {
    OEDO_CONTROLLER("oedo_controller.mp3"),
    PEACH_PIT("peach_pit.ogg");
}

enum class SoundAsset(val path: String) {
    TWO_MOONS("two_moons.mp3"),
    AM("am.wav");
}