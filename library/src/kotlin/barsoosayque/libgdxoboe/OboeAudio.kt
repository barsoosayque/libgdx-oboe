package barsoosayque.libgdxoboe

import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.Files
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.files.FileHandle

/** [Audio] implementation which utilize [OboeMusic] and [OboeSound] */
// TODO: delegate errors from c++ to GdxRuntimeException
class OboeAudio(private val assetManager: AssetManager) : Audio {
    private var audioEngine: Long = 0
    private val audioDevices: MutableList<AudioDevice> = mutableListOf()

    init {
        System.loadLibrary("libgdx-oboe")
        init()
    }

    private external fun init()
    private external fun createSoundpoolFromAsset(assetManager: AssetManager, path: String): NativeSoundpool
    private external fun createSoundpoolFromPath(path: String): NativeSoundpool
    private external fun createMusicFromAsset(assetManager: AssetManager, path: String): NativeMusic
    private external fun createMusicFromPath(path: String): NativeMusic
    private external fun createAudioEngine(samplingRate: Int, isMono: Boolean): NativeAudioEngine

    external fun resume()
    external fun stop()
    private external fun disposeEngine()

    fun dispose() {
        // also disposes sounds and music
        disposeEngine()

        audioDevices.onEach { it.dispose() }.clear()
    }

    override fun newMusic(file: FileHandle): Music = when (file.type()) {
        Files.FileType.Internal -> createMusicFromAsset(assetManager, file.path())
        else -> createMusicFromPath(file.file().path)
    }.let(::OboeMusic)

    override fun newSound(file: FileHandle): Sound = when (file.type()) {
        Files.FileType.Internal -> createSoundpoolFromAsset(assetManager, file.path())
        else -> createSoundpoolFromPath(file.file().path)
    }.let(::OboeSound)

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice =
            createAudioEngine(samplingRate, isMono)
                    .let(::OboeAudioDevice)
                    .also { audioDevices.add(it) }

    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder =
            createAudioEngine(samplingRate, isMono)
                    .let(::OboeAudioRecorder)
}