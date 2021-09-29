package barsoosayque.libgdxoboe

import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Music.OnCompletionListener

/** [Music] implementation using ffmpeg and oboe stream. */
class OboeMusic(private val music: NativeMusic) : Music {
    private var listener: OnCompletionListener? = null

    init {
        init()
    }

    private external fun init()

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
    override fun setOnCompletionListener(listener: OnCompletionListener?) {
        this.listener = listener
    }
}