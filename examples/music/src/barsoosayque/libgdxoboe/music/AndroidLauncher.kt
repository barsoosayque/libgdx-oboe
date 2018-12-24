package barsoosayque.libgdxoboe.music

import android.os.Bundle
import barsoosayque.libgdxoboe.OboeAndroidApplication
import barsoosayque.libgdxoboe.music.app.MainApp
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration

class AndroidLauncher : OboeAndroidApplication() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }
}
