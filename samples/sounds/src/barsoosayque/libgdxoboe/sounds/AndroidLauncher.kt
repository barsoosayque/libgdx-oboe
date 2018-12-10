package barsoosayque.libgdxoboe.sounds

import android.os.Bundle
import barsoosayque.libgdxoboe.OboeAudio
import barsoosayque.libgdxoboe.sounds.app.MainApp
import com.badlogic.gdx.Audio
import com.badlogic.gdx.LifecycleListener
import com.badlogic.gdx.backends.android.AndroidApplication
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration

class AndroidLauncher : AndroidApplication() {
    private var focusWasChanged: Boolean = true
    private var resumeAudio: Boolean = true
    lateinit var oboeAudio: OboeAudio

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        oboeAudio = OboeAudio(assets)
        oboeAudioListener()

        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }

    private fun oboeAudioListener() {
        addLifecycleListener(object : LifecycleListener {
            override fun pause() { oboeAudio.stop() }
            override fun dispose() { oboeAudio.dispose() }
            override fun resume() {}
        })
    }

    override fun getAudio(): Audio = oboeAudio

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        focusWasChanged = hasFocus
        if (hasFocus && resumeAudio) {
            oboeAudio.resume()
            resumeAudio = false
        }
    }

    override fun onResume() {
        resumeAudio = true
        if (focusWasChanged) {
            oboeAudio.resume()
            resumeAudio = false
        }
        super.onResume()
    }
}
