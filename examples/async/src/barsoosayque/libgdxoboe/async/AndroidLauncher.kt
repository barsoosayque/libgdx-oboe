package barsoosayque.libgdxoboe.async

import android.os.Bundle
import barsoosayque.libgdxoboe.OboeAndroidApplication
import barsoosayque.libgdxoboe.async.app.MainApp
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration

class AndroidLauncher : OboeAndroidApplication() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }
}