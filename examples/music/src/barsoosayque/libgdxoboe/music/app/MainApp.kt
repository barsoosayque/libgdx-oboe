package barsoosayque.libgdxoboe.music.app

import barsoosayque.libgdxoboe.music.content.MusicAsset
import barsoosayque.libgdxoboe.music.content.SkinAsset
import com.badlogic.gdx.ApplicationAdapter
import com.badlogic.gdx.Gdx
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.audio.Music
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
        ui = AppUi(MusicAsset.values(), assetManager)
        Gdx.input.inputProcessor = ui
    }

    override fun render() {
        ui.act()
        Gdx.gl.glClearColor(0.07f, 0.07f, 0.17f, 1f)
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT)
        ui.draw()
    }

    private fun loadAssets() {
        SkinAsset.values().forEach { assetManager.load(it.path, Skin::class.java) }
        MusicAsset.values().forEach { assetManager.load(it.path, Music::class.java) }
        assetManager.finishLoading()
    }
}