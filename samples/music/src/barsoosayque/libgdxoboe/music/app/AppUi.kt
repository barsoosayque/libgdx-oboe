package barsoosayque.libgdxoboe.music.app

import barsoosayque.libgdxoboe.music.content.MusicAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.actors.onChangeEvent
import ktx.scene2d.*
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
        music: Array<MusicAsset>,
        assetManager: AssetManager
) : Stage(ExtendViewport(700f, 480f)) {
    private lateinit var positionProgressBar: ProgressBar
    private var selectedMusic: Music = music.first().get(assetManager)

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
                selectedMusic = list.selected.get(assetManager)
            }
            verticalGroup {
                horizontalGroup {
                    space(10f)
                    label("Playback: ")
                    textButton("Play").onChange { selectedMusic.play() }
                    textButton("Pause").onChange { selectedMusic.pause() }
                    textButton("Stop").onChange { selectedMusic.stop() }
                }
            }.cell(growX = true)
            row()
            container {
                width(600f)
                positionProgressBar = progressBar()
            }.cell(colspan = 2)
        }.cell(grow = true)
    }.let(::addActor)
}
