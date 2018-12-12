package barsoosayque.libgdxoboe

import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.files.FileHandle

class OboeAudio(private val assetManager: AssetManager) : Audio {
    private var audioEngine: Long = 0

    init {
        System.loadLibrary("libgdx-oboe")
        init(assetManager)
    }

    /** @see <a>https://developer.android.com/guide/topics/media/media-formats</a> */
    private fun checkFileFormat(file: FileHandle) = when (file.extension().toLowerCase()) {
        "flac", "mp3", "wav", "aac", "ogg" -> file
        else -> throw IllegalArgumentException("Unknow file format (\"$file\"). Only FLAC, MP3, WAV, AAC and OGG is allowed here.")
    }

    private external fun init(assetManager: AssetManager)
    private external fun createSoundpool(path: String): NativeSoundpool

    external fun resume()
    external fun stop()
    external fun dispose()

    override fun newMusic(file: FileHandle): Music = TODO()
    override fun newSound(file: FileHandle): Sound =
            checkFileFormat(file).path()
                    .let(::createSoundpool)
                    .let(::OboeSound)

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice {
        TODO()
    }

    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder {
        TODO()
    }
}