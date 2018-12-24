package barsoosayque.libgdxoboe

import com.badlogic.gdx.audio.Music

class OboeMusic(private val music: NativeMusic) : Music {
    private var onComplete: NativeCallback = NativeCallback(0)
    private external fun setCompletionCallback(callback: () -> Unit)

    external override fun isPlaying(): Boolean
    external override fun isLooping(): Boolean
    external override fun pause()
    external override fun setPan(pan: Float, volume: Float)
    external override fun getPosition(): Float
    external override fun setLooping(isLooping: Boolean)
    external override fun getVolume(): Float
    external override fun play()
    external override fun stop()
    external override fun setVolume(volume: Float)
    external override fun setPosition(position: Float)
    external override fun dispose()

    override fun setOnCompletionListener(listener: Music.OnCompletionListener?) {
        setCompletionCallback { listener?.onCompletion(this) }
    }
}