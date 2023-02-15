package barsoosayque.libgdxoboe

import android.content.res.AssetManager
import com.badlogic.gdx.Audio
import com.badlogic.gdx.Files
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.audio.AudioDevice
import com.badlogic.gdx.audio.AudioRecorder
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.backends.android.AndroidAudio
import com.badlogic.gdx.backends.android.AndroidMusic
import com.badlogic.gdx.files.FileHandle
import com.badlogic.gdx.utils.Disposable
import com.badlogic.gdx.utils.GdxRuntimeException

/** [Audio] implementation which utilize [OboeMusic] and [OboeSound].
 * Returns [GdxRuntimeException] on native errors */
class OboeAudio(private val assetManager: AssetManager) : AndroidAudio {
    private var sharedAudioPlayer: NativeAudioPlayer = NativeAudioPlayer()
    private val disposables: MutableList<Disposable> = mutableListOf()

    init {
        System.loadLibrary("libgdx-oboe")
    }

    private external fun createSoundpoolFromAsset(assetManager: AssetManager, path: String): Long
    private external fun createSoundpoolFromPath(path: String): Long
    private external fun createMusicFromAsset(assetManager: AssetManager, path: String): Long
    private external fun createMusicFromPath(path: String): Long
    private external fun createAudioStream(samplingRate: Int, isMono: Boolean): Long
    private external fun createAudioRecorder(samplingRate: Int, isMono: Boolean): Long

    external override fun resume()
    external override fun pause()
    private external fun disposeEngine()

    override fun notifyMusicDisposed(music: AndroidMusic?) {
        // No need to do anything here since runtime disposal of music/soundpool is
        // well handled in audio_player.cpp
    }

    override fun dispose() {
        disposeEngine()
        disposables.onEach { it.dispose() }.clear()
    }

    override fun newMusic(file: FileHandle): Music? = when (file.type()) {
        Files.FileType.Internal -> createMusicFromAsset(assetManager, file.path())
        Files.FileType.Absolute -> createMusicFromPath(file.path())
        Files.FileType.Local -> createMusicFromPath(Gdx.app.files.localStoragePath + file.path())
        Files.FileType.External -> createMusicFromPath(Gdx.app.files.externalStoragePath + file.path())
        else -> createMusicFromPath(file.file().path) // AndroidFileHandle doesn't implement file(), this will throw a null exception!
    }.takeIf { it != 0L }
        ?.let(::NativeMusic)
        ?.let(::OboeMusic)
        ?.also(disposables::add)

    override fun newSound(file: FileHandle): Sound? = when (file.type()) {
        Files.FileType.Internal -> createSoundpoolFromAsset(assetManager, file.path())
        Files.FileType.Absolute -> createSoundpoolFromPath(file.path())
        Files.FileType.Local -> createSoundpoolFromPath(Gdx.app.files.localStoragePath + file.path())
        Files.FileType.External -> createSoundpoolFromPath(Gdx.app.files.externalStoragePath + file.path())
        else -> createSoundpoolFromPath(file.file().path)
    }.takeIf { it != 0L }
        ?.let(::NativeSoundpool)
        ?.let(::OboeSound)
        ?.also(disposables::add)

    override fun newAudioDevice(samplingRate: Int, isMono: Boolean): AudioDevice =
        createAudioStream(samplingRate, isMono).let(::NativeAudioStream)
            .let(::OboeAudioDevice)
            .also(disposables::add)

    override fun newAudioRecorder(samplingRate: Int, isMono: Boolean): AudioRecorder =
        createAudioRecorder(samplingRate, isMono).let(::NativeAudioRecorder)
            .let(::OboeAudioRecorder)
            .also(disposables::add)
}