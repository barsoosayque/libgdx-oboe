package barsoosayque.libgdxoboe.recorder

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioManager
import android.os.Build
import android.os.Bundle
import android.widget.Toast
import barsoosayque.libgdxoboe.OboeAudio
import barsoosayque.libgdxoboe.recorder.app.MainApp
import com.badlogic.gdx.backends.android.AndroidApplication
import com.badlogic.gdx.backends.android.AndroidApplicationConfiguration
import com.badlogic.gdx.backends.android.AndroidAudio


class AndroidLauncher : AndroidApplication() {
    override fun createAudio(context: Context, config: AndroidApplicationConfiguration): AndroidAudio =
            OboeAudio(context.assets)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)



        if (requestRecordAudioPermission()) {
            run()
        }
        val manager = getSystemService(Context.AUDIO_SERVICE) as AudioManager;
        manager.mode
    }

    private fun run() {
        val config = AndroidApplicationConfiguration()
        initialize(MainApp(), config)
    }

    private fun requestRecordAudioPermission(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.RECORD_AUDIO) ==
                PackageManager.PERMISSION_GRANTED) {
                true
            } else {
                if (shouldShowRequestPermissionRationale(Manifest.permission.RECORD_AUDIO)) {
                    Toast.makeText(this,
                        "App required access to audio", Toast.LENGTH_SHORT).show();
                }
                requestPermissions(arrayOf(Manifest.permission.RECORD_AUDIO), 100);
                false
            }
        } else {
            true
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 100) {
            if (grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(
                    applicationContext,
                    "Application will now be able to record", Toast.LENGTH_SHORT
                ).show()
            }
        }
        if (requestRecordAudioPermission()) {
            run()
        }
    }
}

