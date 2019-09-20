package barsoosayque.libgdxoboe.music.app

import barsoosayque.libgdxoboe.music.content.MusicAsset
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.CheckBox
import com.badlogic.gdx.scenes.scene2d.ui.Label
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar
import com.badlogic.gdx.scenes.scene2d.ui.Slider
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.actors.onClick
import ktx.actors.txt
import ktx.scene2d.*
import kotlin.math.floor
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
        music: Array<MusicAsset>,
        assetManager: AssetManager
) : Stage(ExtendViewport(700f, 480f)) {
    private lateinit var volumeSlider: Slider
    private lateinit var panSlider: Slider
    private lateinit var positionProgressBar: ProgressBar
    private lateinit var positionLabel: Label
    private lateinit var loopingCheck: CheckBox
    private var seekDelta: Float = 0f
    private var newSeek: Float? = null
        set(value) {
            seekDelta = 0f
            field = value
        }
    private var selectedAsset: MusicAsset = music.first()
    private var selectedMusic: Music = selectedAsset.get(assetManager)

    val root = table {
        setFillParent(true)
        pad(30f)
        center()

        label("libGDX Oboe: Music").cell(row = true, padBottom = 60f)
        table {
            label("Songs:")
            label("Controls")
            row()
            listWidgetOf(GdxArray(music)).cell(growY = true).onChangeEvent { _, list ->
                selectedMusic.stop()
                selectedAsset = list.selected
                selectedMusic = selectedAsset.get(assetManager)
                volumeSlider.value = selectedMusic.volume
                panSlider.value = 0.0f
                loopingCheck.isChecked = selectedMusic.isLooping

                selectedMusic.setOnCompletionListener {
                    Gdx.app.log("Music", "Is done")
                }
            }
            verticalGroup {
                space(20f)
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
                horizontalGroup {
                    space(10f)
                    label("Pan: ")
                    panSlider = slider(-1f) {
                        value = 0.0f
                        onChangeEvent { _, slider -> selectedMusic.setPan(slider.value, volumeSlider.value) }
                    }
                }
                loopingCheck = checkBox("Is looping") {
                    isChecked = selectedMusic.isLooping
                    onChange {
                        selectedMusic.isLooping = isChecked
                    }
                }
            }.cell(growX = true)
            row()
            horizontalGroup {
                container {
                    width(600f)
                    positionProgressBar = slider {
                        onClick {
                            val sec = value * selectedAsset.duration
                            Gdx.app.log("Music", "Change sound position to ${sec.time()} seconds")
                            newSeek = sec
                        }
                    }
                }
                positionLabel = label("0:00 / 0:00")
            }.cell(colspan = 2)
        }.cell(grow = true)
    }.let(::addActor)

    override fun act(delta: Float) {
        super.act(delta)
        newSeek?.let {
            seekDelta += delta
            if (seekDelta >= SEEK_TIME) {
                selectedMusic.position = it
                newSeek = null
            }
        }

        positionProgressBar.value = newSeek ?: selectedMusic.position / selectedAsset.duration
        positionLabel.txt = "${selectedMusic.position.time()} / ${selectedAsset.duration.time()}"
    }

    private fun Float.time(): String {
        val minutes = floor(this / 60)
        val seconds = this - minutes * 60
        return "${minutes.toInt().toString().padStart(2, '0')}:" +
                seconds.toInt().toString().padStart(2, '0')
    }

    companion object {
        private const val SEEK_TIME = 0.5f
    }
}
