package barsoosayque.libgdxoboe.benchmark

import android.content.Context
import androidx.benchmark.junit4.BenchmarkRule
import androidx.benchmark.junit4.measureRepeated
import androidx.test.core.app.ApplicationProvider
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.filters.LargeTest
import barsoosayque.libgdxoboe.OboeAudio
import barsoosayque.libgdxoboe.benchmark.common.MusicAsset
import com.badlogic.gdx.Audio
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration
import com.badlogic.gdx.backends.android.AndroidAudio
import com.badlogic.gdx.backends.android.AndroidFiles
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@LargeTest
@RunWith(AndroidJUnit4::class)
class MusicBenchmark {
    @get:Rule
    val benchmarkRule = BenchmarkRule()

    lateinit var files: AndroidFiles
    lateinit var gdxAudio: AndroidAudio
    lateinit var oboeAudio: OboeAudio

    @Before
    fun prepare() {
        val context = ApplicationProvider.getApplicationContext<Context>()
        files = AndroidFiles(context.assets)
        if (::gdxAudio.isInitialized) {
            gdxAudio.dispose()
        }
        gdxAudio = AndroidAudio(context, AndroidApplicationConfiguration())

        if (::oboeAudio.isInitialized) {
            oboeAudio.dispose()
        }
        oboeAudio = OboeAudio(context.assets)
    }

    private fun loadMusicGeneric(audio: Audio, extension: String? = null) {
        val music = MusicAsset.values().filter { it.path.endsWith(extension ?: "") }
        benchmarkRule.measureRepeated {
            music.forEach {
                val instance = audio.newMusic(files.internal(it.path))
                runWithTimingDisabled { instance.dispose() }
            }
        }
    }

    private fun seekMusicGeneric(audio: Audio, extension: String? = null) {
        val music = MusicAsset.values().filter { it.path.endsWith(extension ?: "") }
        benchmarkRule.measureRepeated {
            music.forEach {
                val instance = runWithTimingDisabled { audio.newMusic(files.internal(it.path)).also { it.volume = 0f } }
                for (i in 0 until ((it.duration / 5f).toInt())) {
                    instance.position = (i * 5f).coerceIn(0f, it.duration)
                    instance.play()
                    instance.pause()
                }
                for (i in 0 until ((it.duration / 5f).toInt())) {
                    instance.position = (it.duration - i * 5f).coerceIn(0f, it.duration)
                    instance.play()
                    instance.pause()
                }
                runWithTimingDisabled { instance.dispose() }
            }
        }
    }

    @Test
    fun loadMusicGDX() = loadMusicGeneric(gdxAudio)

    @Test
    fun loadMusicOboe() = loadMusicGeneric(oboeAudio)

    @Test
    fun seekMusicGDX() = seekMusicGeneric(gdxAudio)

    @Test
    fun seekMusicOboe() = seekMusicGeneric(oboeAudio)
}