package barsoosayque.libgdxoboe

import android.content.res.AssetFileDescriptor
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.util.Log
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import kotlin.math.min


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
                start()
            }
        } else null
    } ?: throw IllegalArgumentException("Can't extract audio from \"$fd\".")
    private val info = MediaCodec.BufferInfo()
    private var cachedBuffer: ByteBuffer? = null

    private fun readSampleData(): Boolean? =
            decoder.dequeueInputBuffer(100).takeIf { it >= 0 }?.let { bufferIndex ->
                extractor.readSampleData(decoder.inputBuffers[bufferIndex], 0).let { size ->
                    if(size > 0) {
                        decoder.queueInputBuffer(bufferIndex, 0, size, extractor.sampleTime, 0)
                        extractor.advance()
                        false
                    } else {
                        decoder.queueInputBuffer(bufferIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM)
                        true
                    }
                }
            }

    private fun parseSamples(destination: ByteArrayOutputStream, maximumBytes: Int? = null): Int {
        var bytesProcessed = 0
        decoder.dequeueOutputBuffer(info, 100).takeIf { it >= 0 }?.also { bufferIndex ->
            when (bufferIndex) {
                MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> Log.d("AudioDecoder", "Buffers changed.")
                MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> Log.d("AudioDecoder", "Format changed.")
                MediaCodec.INFO_TRY_AGAIN_LATER -> Log.d("AudioDecoder", "Timeout. Trying again...")
                else -> {
                    decoder.outputBuffers[bufferIndex].also { buffer ->
                        buffer.position(info.offset)
                        buffer.limit(info.offset + info.size)
                        ByteArray(buffer.remaining()).also { tmp ->
                            val size = maximumBytes?.let { min(info.size, it) } ?: info.size
                            buffer.get(tmp)
                            destination.write(tmp, 0, size)
                            bytesProcessed = size
                            if(size < info.size) {
                                cachedBuffer = ByteBuffer.wrap(tmp)
                                cachedBuffer?.position(size)
                            }
                        }
                    }
                    decoder.releaseOutputBuffer(bufferIndex, false)
                }
            }
        }

        return bytesProcessed
    }

    /** Read more data and decode it.
     * @param samples Amount of new samples to be decoded
     * (negative value to read and decode to eof) */
    fun decode(samples: Int): ByteArray {
        val stream = ByteArrayOutputStream()
        var bytesLeft = samples * 2
        var eofExtractor = false
        var eofDecoder = false

        cachedBuffer?.let {
            bytesLeft -= it.remaining()
            stream.write(it.array(), it.position(), it.remaining())
        }
        cachedBuffer = null

        while (!(eofExtractor && eofDecoder) && bytesLeft != 0) {
            if (!eofExtractor) {
                eofExtractor = readSampleData() ?: eofExtractor
            }

            eofDecoder = eofDecoder || info.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0
            if (!eofDecoder) {
                bytesLeft -= parseSamples(stream, bytesLeft.takeIf { it > 0 })
            }
        }

        return stream.toByteArray()
    }

    /** Read file to EOF and decode it all. */
    fun decode(): ByteArray = decode(-1)

    /** Move head of the decoder to the @position (in seconds) */
    fun seek(position: Float) {
        cachedBuffer = null
        extractor.seekTo((position * 1000).toLong(), MediaExtractor.SEEK_TO_CLOSEST_SYNC)
    }

    /** Release native resources */
    fun dispose() {
        decoder.stop()
        decoder.release()
        extractor.release()
    }
}