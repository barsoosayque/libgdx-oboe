package barsoosayque.libgdxoboe.sounds.app

import barsoosayque.libgdxoboe.sounds.content.SoundAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.CheckBox
import com.badlogic.gdx.scenes.scene2d.ui.Slider
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.scene2d.*
import kotlin.math.pow
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
        sounds: Array<SoundAsset>,
        assetManager: AssetManager
) : Stage(ExtendViewport(480f, 700f)) {
    private lateinit var loopCheckBox: CheckBox
    private lateinit var volumeSlider: Slider
    private lateinit var panSlider: Slider
    private lateinit var pitchSlider: Slider
    private val volume: Float get() = volumeSlider.value
    private val pan: Float get() = panSlider.value
    private val pitch: Float get() = 2f.pow(pitchSlider.value)
    private val isLooping: Boolean get() = loopCheckBox.isChecked

    private var selectedSound: Sound = sounds.first().get(assetManager)

    val root = table {
        setFillParent(true)
        pad(30f)
        top()

        label("Libgdx Oboe: Sounds").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(30f)
            verticalGroup {
                label("Select one of the sounds:")
                selectBoxOf<SoundAsset>(GdxArray(sounds)).onChangeEvent { _, selectBox ->
                    selectedSound = selectBox.selected.get(assetManager)
                }
            }
            table {
                defaults().pad(10f)
                label("Soundpool controls: ")
                horizontalGroup {
                    space(10f)
                    textButton("Play").onChange {
                        if (isLooping) selectedSound.loop(volume, pitch, pan)
                        else selectedSound.play(volume, pitch, pan)
                    }
                    textButton("Resume").onChange { selectedSound.resume() }
                    textButton("Pause").onChange { selectedSound.pause() }
                    textButton("Stop").onChange { selectedSound.stop() }
                }
                row()
                label("Volume: ")
                volumeSlider = slider { value = 1.0f }
                row()
                label("Loop: ")
                loopCheckBox = checkBox("is looping")
                row()
                label("Pan: ")
                panSlider = slider(-1f) { value = 0.0f }
                row()
                label("Pitch: ")
                pitchSlider = slider(-1f) { value = 0.0f }
            }
        }.cell(grow = true)
    }.let(::addActor)
}