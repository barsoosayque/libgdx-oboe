package barsoosayque.libgdxoboe

import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.Files
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.files.FileHandle
import com.badlogic.gdx.utils.GdxRuntimeException

/** [Audio] implementation which utilize [OboeMusic] and [OboeSound] */
// TODO: delegate errors from c++ to GdxRuntimeException
class OboeAudio(private val assetManager: AssetManager) : Audio {
    private var audioEngine: Long = 0

    init {
        System.loadLibrary("libgdx-oboe")
        init()
    }

    // FIXME: remove in favour of magic numbers in header
    private fun checkFileFormat(file: FileHandle) = when (file.extension().toLowerCase()) {
        "mp3", "wav", "ogg" -> file
        else -> throw GdxRuntimeException("Unknown file format (\"$file\"). Only MP3, WAV, and OGG is allowed here.")
    }

    private external fun init()
    private external fun createSoundpoolFromAsset(assetManager: AssetManager, path: String): NativeSoundpool
    private external fun createSoundpoolFromPath(path: String): NativeSoundpool
    private external fun createMusicFromAsset(assetManager: AssetManager, path: String): NativeMusic
    private external fun createMusicFromPath(path: String): NativeMusic
    private external fun createAudioEngine(samplingRate: Int, isMono: Boolean): NativeAudioEngine

    external fun resume()
    external fun stop()
    external fun dispose()

    override fun newMusic(file: FileHandle): Music =
            checkFileFormat(file).path()
                    .let(::createMusicFromPath)
                    .let(::OboeMusic)

    override fun newSound(file: FileHandle): Sound =
            checkFileFormat(file)
                    .let {
                        when (it.type()) {
                            Files.FileType.Internal -> createSoundpoolFromAsset(assetManager, it.path())
                            else -> createSoundpoolFromPath(it.file().path)
                        }
                    }
                    .let(::OboeSound)

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice =
            createAudioEngine(samplingRate, isMono)
                    .let(::OboeAudioDevice)

    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder =
            createAudioEngine(samplingRate, isMono)
                    .let(::OboeAudioRecorder)
}