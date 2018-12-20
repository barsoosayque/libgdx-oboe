package barsoosayque.libgdxoboe

import android.content.res.AssetFileDescriptor
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import java.nio.ByteBuffer

/** Decoder class which can process audio files to PCM.
 * Uses 16-bit Little-Endian samples.
 * @throws IllegalArgumentException If given [fd] can't be processed */
class AudioDecoder(fd: AssetFileDescriptor) {
    private val extractor = MediaExtractor().apply {
        setDataSource(fd.fileDescriptor, fd.startOffset, fd.declaredLength)
    }
    private val decoder = let {
        if (extractor.trackCount > 1) throw IllegalArgumentException("Wrong track count in $fd")
        val mediaFormat = extractor.getTrackFormat(0)
        val mime = mediaFormat.getString(MediaFormat.KEY_MIME)
        if (mime.startsWith("audio/")) {
            extractor.selectTrack(0)
            MediaCodec.createDecoderByType(mime).apply {
                configure(mediaFormat, null, null, 0)
            }
        } else null
    } ?: throw IllegalArgumentException("Can't extract audio from \"$fd\".")

    /** Read more data and decode it.
     * @param samples Amount of new samples to be decoded */
    fun decode(samples: Int): ByteBuffer {
        return ByteBuffer.allocateDirect(10)
    }

    /** Read file to EOF and decode it all. */
    fun decode(): ByteBuffer {
        return ByteBuffer.allocateDirect(10)
    }

    /** Move head of the decoder to the @position (in seconds) */
    fun seek(position: Float) {
        extractor.seekTo((position * 1000).toLong(), MediaExtractor.SEEK_TO_CLOSEST_SYNC)
    }

    /** Release native resources */
    fun dispose() {
        decoder.release()
        extractor.release()
    }
}