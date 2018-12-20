package barsoosayque.libgdxoboe

import android.content.res.AssetFileDescriptor
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.util.Log
import java.io.ByteArrayOutputStream


/** Decoder class which can process audio files to PCM.
 * Uses 16-bit Little-Endian samples.
 * @throws IllegalArgumentException If given [fd] can't be processed */
class AudioDecoder(fd: AssetFileDescriptor) {
    private val extractor = MediaExtractor().apply {
        setDataSource(fd.fileDescriptor, fd.startOffset, fd.declaredLength)
        fd.close()
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
    fun decode(samples: Int): ByteArray {
        TODO()
    }

    /** Read file to EOF and decode it all. */
    fun decode(): ByteArray {
        val stream = ByteArrayOutputStream()
        val info = MediaCodec.BufferInfo()
        var eofExtractor = false
        var eofDecoder = false

        decoder.start()
        while (!eofExtractor && !eofDecoder) {
            decoder.dequeueInputBuffer(-1).takeIf { it >= 0 }?.also { bufferIndex ->
                extractor.readSampleData(decoder.inputBuffers[bufferIndex], 0).also { size ->
                    eofExtractor = !extractor.advance()
                    if (eofExtractor) {
                        decoder.queueInputBuffer(bufferIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM)
                    } else {
                        decoder.queueInputBuffer(bufferIndex, 0, size, extractor.sampleTime, 0)
                    }
                }
            }

            eofDecoder = info.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0
            decoder.dequeueOutputBuffer(info, -1).takeIf { it >= 0 }?.also { bufferIndex ->
                when(bufferIndex) {
                    MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> Log.d("AudioDecoder", "Buffers changed.")
                    MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> Log.d("AudioDecoder", "Format changed.")
                    MediaCodec.INFO_TRY_AGAIN_LATER -> Log.d("AudioDecoder", "Timeout. Trying again...")
                    else -> if(!eofDecoder) {
                        decoder.outputBuffers[bufferIndex].also { buffer ->
                            buffer.position(info.offset)
                            buffer.limit(info.offset + info.size)
                            val tmp = ByteArray(buffer.remaining())
                            buffer.get(tmp)
                            stream.write(tmp)
                        }
                        decoder.releaseOutputBuffer(bufferIndex, false)
                    }
                }
            }
        }
        decoder.stop()

        return stream.toByteArray()
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