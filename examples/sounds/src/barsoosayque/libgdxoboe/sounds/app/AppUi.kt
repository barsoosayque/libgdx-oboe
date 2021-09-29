package barsoosayque.libgdxoboe.sounds.app

import barsoosayque.libgdxoboe.sounds.content.SoundAsset
import com.badlogic.gdx.assets.AssetManager
import com.badlogic.gdx.scenes.scene2d.Stage
import com.badlogic.gdx.scenes.scene2d.ui.CheckBox
import com.badlogic.gdx.scenes.scene2d.ui.Slider
import com.badlogic.gdx.scenes.scene2d.ui.VerticalGroup
import com.badlogic.gdx.utils.viewport.ExtendViewport
import ktx.actors.onChange
import ktx.scene2d.*
import kotlin.concurrent.thread
import kotlin.math.pow
import com.badlogic.gdx.utils.Array as GdxArray

class AppUi(
    sounds: Array<SoundAsset>,
    private val assetManager: AssetManager
) : Stage(ExtendViewport(480f, 700f)) {
    private var loopCheckBox: CheckBox
    private var volumeSlider: Slider
    private var panSlider: Slider
    private var pitchSlider: Slider
    private var soundsGrp: VerticalGroup
    private val volume: Float get() = volumeSlider.value
    private val pan: Float get() = panSlider.value
    private val pitch: Float get() = 2f.pow(pitchSlider.value)
    private val isLooping: Boolean get() = loopCheckBox.isChecked
    private val playingSounds = mutableListOf<Pair<SoundAsset, Long>>()

    private var selectedSoundAsset: SoundAsset = sounds.first()

    val root = scene2d.table {
        setFillParent(true)
        pad(30f)
        top()

        label("libGDX Oboe: Sounds").cell(row = true, padBottom = 60f)
        verticalGroup {
            center()
            space(30f)
            horizontalGroup {
                space(30f)
                verticalGroup {
                    label("Select one of the sounds:")
                    selectBoxOf<SoundAsset>(GdxArray(sounds)).onChange {
                        selectedSoundAsset = selected
                    }
                }
                textButton("Play").onChange {
                    val selectedSound = selectedSoundAsset.get(assetManager)
                    val id = if (isLooping) selectedSound.loop(volume, pitch, pan)
                    else selectedSound.play(volume, pitch, pan)

                    playingSounds.add(selectedSoundAsset to id)
                    if (!isLooping) {
                        thread {
                            Thread.sleep((selectedSoundAsset.duration * 1000).toLong())
                            playingSounds.removeAll { (_, soundId) -> soundId == id }
                            updateSounds()
                        }
                    }
                    updateSounds()
                }
            }
            table {
                defaults().pad(10f)
                row()
                label("Volume: ")
                volumeSlider = slider { value = 1.0f }
                row()
                label("Loop: ")
                loopCheckBox = checkBox("is looping")
                row()
                label("Pan: ")
                panSlider = slider(-1f) { value = 0.0f }
                row()
                label("Pitch: ")
                pitchSlider = slider(-1f) { value = 0.0f }
            }
            verticalGroup {
                space(10f)
                label("Current sounds")
                horizontalGroup {
                    space(10f)
                    textButton("Resume all").onChange {
                        sounds.forEach { it.get(assetManager).resume() }
                    }
                    textButton("Pause all").onChange {
                        sounds.forEach {
                            it.get(assetManager).pause()
                        }
                    }
                    textButton("Stop all").onChange {
                        sounds.forEach { it.get(assetManager).stop() }
                        playingSounds.clear()
                        updateSounds()
                    }
                }
                soundsGrp = verticalGroup {
                    space(10f)
                }
            }

        }.cell(grow = true)
    }.let(::addActor)

    private fun updateSounds() {
        soundsGrp.clear()
        playingSounds.forEach { (asset, id) ->
            soundsGrp.addActor(scene2d.horizontalGroup {
                space(10f)
                label("${asset.name} (${id})")
                textButton("Resume").onChange { asset.get(assetManager).resume(id) }
                textButton("Pause").onChange { asset.get(assetManager).pause(id) }
                textButton("Stop").onChange {
                    asset.get(assetManager).stop(id)
                    playingSounds.removeAll { (_, soundId) -> soundId == id }
                    updateSounds()
                }
            })
        }
    }
}