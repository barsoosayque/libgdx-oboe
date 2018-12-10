package barsoosayque.libgdxoboe.sounds.app

import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.scene2d.label
import ktx.scene2d.table

class AppUi: Stage(ExtendViewport(300f, 400f)) {
    val root = table {
        setFillParent(true)
        center()
        label("Hello")
    }.let(::addActor)
}