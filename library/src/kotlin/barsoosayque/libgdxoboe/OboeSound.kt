package barsoosayque.libgdxoboe

import com.badlogic.gdx.audio.Sound

/** [Sound] implementation using native soundpool and oboe stream. */
class OboeSound(private val soundpool: NativeSoundpool) : Sound {
    external override fun pause()
    external override fun pause(soundId: Long)
    external override fun play(): Long
    external override fun play(volume: Float): Long
    external override fun stop()
    external override fun stop(soundId: Long)
    external override fun resume()
    external override fun resume(soundId: Long)
    external override fun setVolume(soundId: Long, volume: Float)
    external override fun dispose()
    external override fun loop(): Long
    external override fun loop(volume: Float): Long
    external override fun setLooping(soundId: Long, looping: Boolean)
    external override fun setPitch(soundId: Long, pitch: Float)
    external override fun setPan(soundId: Long, pan: Float, volume: Float)
    external override fun play(volume: Float, pitch: Float, pan: Float): Long
    external override fun loop(volume: Float, pitch: Float, pan: Float): Long
}