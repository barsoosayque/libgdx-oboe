package barsoosayque.libgdxoboe.waves.app

import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.scene2d.label
import ktx.scene2d.table
import ktx.scene2d.verticalGroup
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(val assetManager: AssetManager) : Stage(ExtendViewport(480f, 700f)) {
    val root = table {
        setFillParent(true)
        pad(30f)
        top()

        label("libGDX Oboe: Waves").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(60f)


        }.cell(grow = true)
    }.let(::addActor)
}