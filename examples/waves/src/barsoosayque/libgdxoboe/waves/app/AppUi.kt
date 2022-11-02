package barsoosayque.libgdxoboe.waves.app

import barsoosayque.libgdxoboe.waves.content.SinGenerator
import barsoosayque.libgdxoboe.waves.content.SoundGenerator
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.actors.txt
import ktx.scene2d.*
import kotlin.math.roundToInt

class AppUi : Stage(ExtendViewport(480f, 700f)) {
    private val audioDevice = Gdx.audio.newAudioDevice(DEVICE_HZ, true)
    private val sin = SinGenerator(500f)

    val root = scene2d.table {
        setFillParent(true)
        pad(30f)
        top()

        label("libGDX Oboe: Waves").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(60f)

            label("Device latency: ${audioDevice.latency}")
            label("Device isMono: ${audioDevice.isMono}")

            verticalGroup {
                val lbl = label("Wave frequency: 500 Hz")
                slider().onChange {
                    sin.frequency = 500f + value * 500f
                    lbl.txt = "Wave frequency: ${sin.frequency.toInt()} Hz"
                }
            }

            container {
                width(400f).height(100f)
                textButton("Generate 1 second of sin").onChange {
                    repeat(4) {
                        val payload = sin.generatePayload(0.25f)
                        audioDevice.writeSamples(payload, 0, payload.size)
                    }
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