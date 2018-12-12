package barsoosayque.libgdxoboe.music.app

import barsoosayque.libgdxoboe.music.content.MusicAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar
import com.badlogic.gdx.utils.viewport.ExtendViewport
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
        top()

        label("Libgdx Oboe: Music").cell(row = true, padBottom = 60f)
        horizontalGroup {
            listWidgetOf(GdxArray(music)).onChangeEvent { _, list ->
                selectedMusic = list.selected.get(assetManager)
            }
        }
        positionProgressBar = progressBar()
    }.let(::addActor)
}
