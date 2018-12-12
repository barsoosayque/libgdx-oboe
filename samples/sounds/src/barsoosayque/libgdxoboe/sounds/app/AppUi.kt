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
            space(60f)
            verticalGroup {
                label("Select one of the sounds:")
                selectBoxOf<SoundAsset>(GdxArray(sounds)).onChangeEvent { _, selectBox ->
                    selectedSound = selectBox.selected.get(assetManager)
                }
            }
            verticalGroup {
                space(30f)
                label("Playback controls:")
                horizontalGroup {
                    textButton("Play").onChange { selectedSound.play(volume) }
                }
                horizontalGroup {
                    label("Volume: ")
                    volumeSlider = slider { value = 1.0f }
                }
            }
        }.cell(grow = true)

    }.let(::addActor)
}