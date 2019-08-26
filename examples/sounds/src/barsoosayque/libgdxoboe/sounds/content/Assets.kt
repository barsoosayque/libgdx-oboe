package barsoosayque.libgdxoboe.sounds.content

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Sound

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class SoundAsset(val path: String) {
    AM("am.wav"),
    AM_MONO_WAV("am_mono.wav"),
    AM_MONO_OGG("am_mono.ogg"),
    AM_MONO_MP3("am_mono.mp3"),
    TWO_MOONS("two_moons.mp3");

    fun get(assetManager: AssetManager): Sound = assetManager[path]
    override fun toString(): String = "${super.toString()} ($path)"
}