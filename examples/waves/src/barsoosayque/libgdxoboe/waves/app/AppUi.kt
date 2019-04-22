package barsoosayque.libgdxoboe.waves.app

import barsoosayque.libgdxoboe.waves.content.SinGenerator
import barsoosayque.libgdxoboe.waves.content.SoundGenerator
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.CheckBox
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.scene2d.*
import kotlin.math.roundToInt
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(val assetManager: AssetManager) : Stage(ExtendViewport(480f, 700f)) {
    private val audioDevice = Gdx.audio.newAudioDevice(DEVICE_HZ, true)
    private val sin = SinGenerator(500f)

    val root = table {
        setFillParent(true)
        pad(30f)
        top()

        label("libGDX Oboe: Waves").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(60f)

            verticalGroup {
                label("Wave frequency:")
                slider().onChangeEvent { _, actor ->
                    sin.frequency = 500f + actor.value * 21500f
                }
            }

            container {
                width(400f).height(100f)
                textButton("Generate 1 second of sin").onChange {
                    val payload = sin.generatePayload(1f)
                    audioDevice.writeSamples(payload, 0, payload.size)
                }
            }

        }.cell(grow = true)
    }.let(::addActor)

    private fun SoundGenerator.generatePayload(duration: Float) = FloatArray((DEVICE_HZ * duration).roundToInt()) {
        val time: Float = DEVICE_HZ_REV * it
        generate(time)
    }

    companion object {
        private const val DEVICE_HZ = 44100
        private const val DEVICE_HZ_REV = 1f / 44100f
    }
}