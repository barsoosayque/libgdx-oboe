package barsoosayque.sounds

import android.os.Bundle
import com.badlogic.gdx.backends.android.AndroidApplication
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration

class AndroidLauncher : AndroidApplication() {
//    private var focusWasChanged: Boolean = true
//    private var resumeAudio: Boolean = true
//    lateinit var manaudio: Manaudio

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
//        manaudio = Manaudio(assets)
        val config = AndroidApplicationConfiguration()
        config.useAccelerometer = false
        config.useCompass = false
        config.numSamples = 2
        initialize(MainGame(), config)

//        addLifecycleListener(object : LifecycleListener {
//            override fun pause() {
//                manaudio.stop()
//            }
//
//            override fun dispose() {
//                manaudio.dispose()
//            }
//
//            override fun resume() {}
//        })
    }

    // <><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    // "Replace default android audio with custom one" the Hack
    // Basically just mimic base class behavior
    // <><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

//    override fun getAudio(): Audio = manaudio
//
//    override fun onWindowFocusChanged(hasFocus: Boolean) {
//        super.onWindowFocusChanged(hasFocus)
//        focusWasChanged = hasFocus
//        if (hasFocus && resumeAudio) {
//            manaudio.resume()
//            resumeAudio = false
//        }
//    }
//
//    override fun onResume() {
//        resumeAudio = true
//        if (focusWasChanged) {
//            manaudio.resume()
//            resumeAudio = false
//        }
//        super.onResume()
//    }
}
