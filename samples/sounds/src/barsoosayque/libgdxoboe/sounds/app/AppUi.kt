package barsoosayque.libgdxoboe.sounds.app

import barsoosayque.libgdxoboe.sounds.content.SoundAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.Slider
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.scene2d.*
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
        sounds: Array<SoundAsset>,
        assetManager: AssetManager
) : Stage(ExtendViewport(480f, 700f)) {
    private lateinit var volumeSlider: Slider
    private val volume: Float get() = volumeSlider.value

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
                    textButton("Play").onChange { selectedSound.play(volume) }
                    textButton("Resume").onChange { selectedSound.resume() }
                    textButton("Pause").onChange { selectedSound.pause() }
                    textButton("Stop").onChange { selectedSound.stop() }
                }
                row()
                label("Volume: ")
                horizontalGroup {
                    volumeSlider = slider { value = 1.0f }
                }
            }
        }.cell(grow = true)
    }.let(::addActor)
}