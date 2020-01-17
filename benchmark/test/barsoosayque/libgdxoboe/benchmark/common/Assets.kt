package barsoosayque.libgdxoboe.benchmark.common

enum class MusicAsset(val path: String, val duration: Float) {
    MP3("sin_wave.mp3", 30f),
    OGG("sin_wave.ogg", 30f),
    WAV("sin_wave.wav", 30f);
}

enum class SoundAsset(val path: String, val isMono: Boolean) {
    MP3_MONO("chirp_mono.mp3", true),
    OGG_MONO("chirp_mono.ogg", true),
    WAV_MONO("chirp_mono.wav", true),
    MP3_STEREO("chirp_stereo.mp3", false),
    OGG_STEREO("chirp_stereo.ogg", false),
    WAV_STEREO("chirp_stereo.wav", false);
}
