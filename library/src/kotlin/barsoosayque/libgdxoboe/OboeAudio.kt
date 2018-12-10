package barsoosayque.libgdxoboe

import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.files.FileHandle

class OboeAudio(private val assetManager: AssetManager): Audio {
    private var audioEngine: Long = 0

    init {
        System.loadLibrary("libgdx-oboe")
        init(assetManager)
    }

    private external fun init(assetManager: AssetManager)
    private external fun createSoundpool(path: String): Long

    external fun resume()
    external fun stop()
    external fun dispose()

    override fun newMusic(file: FileHandle): Music = TODO()
    override fun newSound(file: FileHandle): Sound = OboeSound(createSoundpool(file.path()))

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice {
        TODO()
    }
    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder {
        TODO()
    }
}