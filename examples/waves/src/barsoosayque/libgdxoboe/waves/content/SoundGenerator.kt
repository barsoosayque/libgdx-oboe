package barsoosayque.libgdxoboe.waves.content

import kotlin.math.PI
import kotlin.math.sin

interface SoundGenerator {
    fun generate(time: Float): Float
}

class SinGenerator(var frequency: Float) : SoundGenerator {
    override fun generate(time: Float): Float = sin(2 * PI.toFloat() * time * frequency)
}