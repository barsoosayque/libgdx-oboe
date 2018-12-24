package barsoosayque.libgdxoboe.sounds

import android.os.Bundle
import barsoosayque.libgdxoboe.OboeAndroidApplication
import barsoosayque.libgdxoboe.sounds.app.MainApp
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration

class AndroidLauncher : OboeAndroidApplication() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }
}
