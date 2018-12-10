package barsoosayque.libgdxoboe.sounds.content

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Sound

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class SoundAsset(val path: String) {
    AM("am.wav"),
    TWO_MOONS("two_moons.wav");

    fun get(assetManager: AssetManager): Sound = assetManager[path]
    override fun toString(): String = "${super.toString()} ($path)"
}