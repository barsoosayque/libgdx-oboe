package barsoosayque.libgdxoboe.music.content

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class MusicAsset(val path: String, val duration: Float) {
    OEDO_CONTROLLER("oedo_controller.mp3", 255.6f),
    TWO_MOONS("two_moons.mp3", 7.75f),
    PEACH_PIT("peach_pit.ogg", 287f),
    PEACH_PIT_MONO("peach_pit_mono.ogg", 287f),
    BEAT("beat.ogg", 1.85f);

    fun get(assetManager: AssetManager): Music = assetManager[path]
    override fun toString(): String = "${super.toString()} ($path)"
}