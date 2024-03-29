package barsoosayque.libgdxoboe

import android.os.Bundle
import com.badlogic.gdx.Audio
import com.badlogic.gdx.LifecycleListener
import com.badlogic.gdx.backends.android.AndroidApplication

/** Utility class which mimics behaviour of default [audio] in [AndroidApplication],
 * but does this with a custom [OboeAudio]. */
@Deprecated("Deprecated in favour of libGDX 1.9.11 AndroidAudio interface w/ createAudio method in AndroidApplication")
open class OboeAndroidApplication : AndroidApplication() {
    private var focusWasChanged: Boolean = true
    private var resumeAudio: Boolean = true
    lateinit var oboeAudio: OboeAudio

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        oboeAudio = OboeAudio(assets)
        oboeAudioListener()
    }

    private fun oboeAudioListener() {
        addLifecycleListener(object : LifecycleListener {
            override fun pause() {
                oboeAudio.pause()
            }

            override fun dispose() {
                oboeAudio.dispose()
            }

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
