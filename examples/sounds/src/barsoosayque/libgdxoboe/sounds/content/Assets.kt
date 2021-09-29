package barsoosayque.libgdxoboe.sounds.content

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Sound

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class SoundAsset(val path: String, val duration: Float) {
    AM("am.wav", 0.39f),
    AM_MONO_WAV("am_mono.wav", 0.39f),
    AM_MONO_OGG("am_mono.ogg", 0.39f),
    AM_MONO_MP3("am_mono.mp3", 0.39f),
    TWO_MOONS("two_moons.mp3", 7.70f);

    fun get(assetManager: AssetManager): Sound = assetManager[path]
    override fun toString(): String = "${super.toString()} ($path)"
}