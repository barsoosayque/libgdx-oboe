package barsoosayque.libgdxoboe.sounds.app

import com.badlogic.gdx.ApplicationAdapter
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.graphics.GL20
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.Skin
import ktx.scene2d.Scene2DSkin

class MainApp : ApplicationAdapter() {
    private val assetManager = AssetManager()
    private lateinit var ui: Stage

    override fun create() {
        loadAssets()
        Scene2DSkin.defaultSkin = assetManager["uiskin.json"]
        ui = AppUi()
    }

    override fun render() {
        Gdx.gl.glClearColor(0.07f, 0.07f, 0.17f, 1f)
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT)
        ui.act()
        ui.draw()
    }

    private fun loadAssets() {
        assetManager.load("uiskin.json", Skin::class.java)
        assetManager.finishLoading()
    }
}