package barsoosayque.libgdxoboe

import com.badlogic.gdx.audio.AudioDevice

/** [AudioDevice] implementation using oboe stream. */
class OboeAudioDevice(private val audioDevice: NativeAudioDevice) : AudioDevice {
    external override fun getLatency(): Int
    external override fun setVolume(volume: Float)
    external override fun writeSamples(samples: ShortArray, offset: Int, numSamples: Int)
    external override fun writeSamples(samples: FloatArray, offset: Int, numSamples: Int)
    external override fun dispose()
    external override fun isMono(): Boolean
}
