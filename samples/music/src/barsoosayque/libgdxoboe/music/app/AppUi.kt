package barsoosayque.libgdxoboe.music.app

import barsoosayque.libgdxoboe.music.content.MusicAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.Label
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar
import com.badlogic.gdx.scenes.scene2d.ui.Slider
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.actors.txt
import ktx.scene2d.*
import kotlin.math.floor
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
        music: Array<MusicAsset>,
        assetManager: AssetManager
) : Stage(ExtendViewport(700f, 480f)) {
    private lateinit var volumeSlider: Slider
    private lateinit var positionProgressBar: ProgressBar
    private lateinit var positionLabel: Label
    private var selectedAsset: MusicAsset = music.first()
    private var selectedMusic: Music = selectedAsset.get(assetManager)

    val root = table {
        setFillParent(true)
        pad(30f)
        center()

        label("Libgdx Oboe: Music").cell(row = true, padBottom = 60f)
        table {
            label("Songs:")
            label("Controls")
            row()
            listWidgetOf(GdxArray(music)).cell(growY = true).onChangeEvent { _, list ->
                selectedMusic.stop()
                selectedAsset = list.selected
                selectedMusic = selectedAsset.get(assetManager)
                volumeSlider.value = selectedMusic.volume
            }
            verticalGroup {
                space(30f)
                horizontalGroup {
                    space(10f)
                    label("Playback: ")
                    textButton("Play").onChange { selectedMusic.play() }
                    textButton("Pause").onChange { selectedMusic.pause() }
                    textButton("Stop").onChange { selectedMusic.stop() }
                }
                horizontalGroup {
                    space(10f)
                    label("Volume: ")
                    volumeSlider = slider {
                        value = selectedMusic.volume
                        onChangeEvent { _, slider -> selectedMusic.volume = slider.value }
                    }
                }
            }.cell(growX = true)
            row()
            horizontalGroup {
                container {
                    width(600f)
                    positionProgressBar = progressBar()
                }
                positionLabel = label("0:00 / 0:00")
            }.cell(colspan = 2)
        }.cell(grow = true)
    }.let(::addActor)

    override fun act(delta: Float) {
        super.act(delta)
        positionProgressBar.value = selectedMusic.position / selectedAsset.duration
        positionLabel.txt = "${selectedMusic.position.time()} / ${selectedAsset.duration.time()}"
    }

    private fun Float.time(): String {
        val minutes = floor(this / 60)
        val seconds = this - minutes * 60
        return "${minutes.toInt()}:${seconds.toInt()}"
    }
}
