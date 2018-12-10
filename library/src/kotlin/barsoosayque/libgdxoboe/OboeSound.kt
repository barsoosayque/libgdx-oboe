package barsoosayque.libgdxoboe

import com.badlogic.gdx.audio.Sound

class OboeSound(soundpool: Long) : Sound {
    private val soundpool = soundpool

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

    override fun setPitch(soundId: Long, pitch: Float) {
        TODO()
    }
    override fun setPan(soundId: Long, pan: Float, volume: Float) {
        TODO()
    }
    override fun setLooping(soundId: Long, looping: Boolean) {
        TODO()
    }
    override fun play(volume: Float, pitch: Float, pan: Float): Long {
        TODO()
    }
    override fun loop(): Long {
        TODO()
    }
    override fun loop(volume: Float): Long {
        TODO()
    }
    override fun loop(volume: Float, pitch: Float, pan: Float): Long {
        TODO()
    }
}