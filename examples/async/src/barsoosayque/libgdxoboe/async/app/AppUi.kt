package barsoosayque.libgdxoboe.async.app

import barsoosayque.libgdxoboe.async.content.MusicAsset
import barsoosayque.libgdxoboe.async.content.SoundAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
import com.badlogic.gdx.audio.Sound
import com.badlogic.gdx.math.Interpolation
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.List
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.scene2d.*
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(val assetManager: AssetManager) : Stage(ExtendViewport(480f, 700f)) {
    private var recacheList = false
    private lateinit var progress: ProgressBar
    private lateinit var assetsList: List<String>

    val root = table {
        setFillParent(true)
        pad(30f)
        top()

        label("Libgdx Oboe: Async").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(60f)

            horizontalGroup {
                space(120f)
                container {
                    width(100f).height(50f)
                    textButton("Load").onChange {
                        SoundAsset.values().forEach { asset ->
                            assetManager.load(asset.path, Sound::class.java)
                        }
                        MusicAsset.values().forEach { asset ->
                            assetManager.load(asset.path, Music::class.java)
                        }
                        recacheList = true
                    }
                }
                container {
                    width(100f).height(50f)
                    textButton("Unload").onChange {
                        SoundAsset.values().forEach { asset -> assetManager.unload(asset.path) }
                        MusicAsset.values().forEach { asset -> assetManager.unload(asset.path) }
                        recacheList = true
                    }
                }
            }

            container {
                width(300f)
                progress = progressBar {
                    setAnimateInterpolation(Interpolation.pow2Out)
                    setAnimateDuration(0.2f)
                }
            }

            verticalGroup {
                top().center()
                label("Loaded assets:")
                assetsList = listWidgetOf(assetManager.assetNames)
            }

        }.cell(grow = true)
    }.let(::addActor)

    override fun act(delta: Float) {
        super.act(delta)
        if (assetManager.update() && recacheList) {
            recacheList = false
            assetsList.setItems(assetManager.assetNames)
        }
        progress.value = assetManager.progress
    }
}