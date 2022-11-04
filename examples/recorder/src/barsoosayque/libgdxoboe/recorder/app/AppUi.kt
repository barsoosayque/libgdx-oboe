package barsoosayque.libgdxoboe.recorder.app

import com.badlogic.gdx.Gdx
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.scene2d.*

class AppUi : Stage(ExtendViewport(480f, 700f)) {
    private val player = Gdx.audio.newAudioDevice(DEVICE_HZ, false)
    private val recorder = Gdx.audio.newAudioRecorder(DEVICE_HZ, false)
    private val buffer = ShortArray(DEVICE_HZ * 2)

    val root = scene2d.table {
        setFillParent(true)
        pad(30f)
        top()

        label("libGDX Oboe: Recorder").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(60f)


            container {
                width(200f).height(100f)
                textButton("Record 1 second").onChange {
                    recorder.read(buffer, 0, buffer.size)
                }
            }

            container {
                width(200f).height(100f)
                textButton("Play recording").onChange {
                    player.writeSamples(buffer, 0, buffer.size)
                }
            }

        }.cell(grow = true)
    }.let(::addActor)

    companion object {
        private const val DEVICE_HZ = 44100
    }
}