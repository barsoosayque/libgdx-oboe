package barsoosayque.libgdxoboe.benchmark

import android.content.Context
import androidx.benchmark.junit4.BenchmarkRule
import androidx.benchmark.junit4.measureRepeated
import androidx.test.core.app.ApplicationProvider
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.filters.LargeTest
import androidx.test.platform.app.InstrumentationRegistry
import barsoosayque.libgdxoboe.OboeAudio
import barsoosayque.libgdxoboe.benchmark.common.SoundAsset
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
class SoundBenchmark {
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

    private fun loadSoundsGeneric(audio: Audio, mono: Boolean? = null, extension: String? = null) {
        val sounds = SoundAsset.values().filter {
            (mono?.equals(it.isMono) ?: true) && (it.path.endsWith(extension ?: ""))
        }
        benchmarkRule.measureRepeated {
            sounds.forEach {
                val instance = audio.newSound(files.internal(it.path))
                runWithTimingDisabled { instance.dispose() }
            }
        }
    }

    @Test
    fun loadSoundsGDX() = loadSoundsGeneric(gdxAudio)

    @Test
    fun loadSoundsOboe() = loadSoundsGeneric(oboeAudio)
}