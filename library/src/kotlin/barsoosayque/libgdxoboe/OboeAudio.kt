package barsoosayque.libgdxoboe

import android.content.res.AssetFileDescriptor
import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.files.FileHandle
import com.badlogic.gdx.utils.GdxRuntimeException

/** [Audio] implementation which utilize [OboeMusic] and [OboeSound] */
class OboeAudio(private val assetManager: AssetManager) : Audio {
    private var audioEngine: Long = 0

    init {
        System.loadLibrary("libgdx-oboe")
        init()
    }

    /** @see <a>https://developer.android.com/guide/topics/media/media-formats</a> */
    private fun checkFileFormat(file: FileHandle) = when (file.extension().toLowerCase()) {
        "flac", "mp3", "wav", "aac", "ogg" -> file
        else -> throw GdxRuntimeException("Unknown file format (\"$file\"). Only FLAC, MP3, WAV, AAC and OGG is allowed here.")
    }

    private external fun init()
    private external fun createSoundpool(fd: AssetFileDescriptor): NativeSoundpool
    private external fun createMusic(fd: AssetFileDescriptor): NativeMusic
    private external fun createMusic(samplingRate: Int, isMono: Boolean): NativeAudioDevice

    external fun resume()
    external fun stop()
    external fun dispose()

    override fun newMusic(file: FileHandle): Music =
            checkFileFormat(file).path()
                    .let(assetManager::openFd)
                    .let(::createMusic)
                    .let(::OboeMusic)

    override fun newSound(file: FileHandle): Sound =
            checkFileFormat(file).path()
                    .let(assetManager::openFd)
                    .let(::createSoundpool)
                    .let(::OboeSound)

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice =
            createMusic(samplingRate, isMono)
                    .let(::OboeAudioDevice)

    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder {
        TODO()
    }
}