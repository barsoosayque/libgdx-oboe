package barsoosayque.libgdxoboe.music.content

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music

enum class SkinAsset(val path: String) { DEFAULT("uiskin.json") }
enum class MusicAsset(val path: String) {
    PEACH_PIT("peach_pit.ogg");

    fun get(assetManager: AssetManager): Music = assetManager[path]
    override fun toString(): String = "${super.toString()} ($path)"
}