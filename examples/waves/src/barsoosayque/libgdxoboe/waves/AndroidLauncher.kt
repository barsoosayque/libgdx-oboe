package barsoosayque.libgdxoboe.waves

import android.content.Context
import android.os.Bundle
import barsoosayque.libgdxoboe.OboeAudio
import barsoosayque.libgdxoboe.waves.app.MainApp
import com.badlogic.gdx.backends.android.AndroidApplication
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration
import com.badlogic.gdx.backends.android.AndroidAudio

class AndroidLauncher : AndroidApplication() {
    override fun createAudio(context: Context, config: AndroidApplicationConfiguration): AndroidAudio =
            OboeAudio(context.assets)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }
}

