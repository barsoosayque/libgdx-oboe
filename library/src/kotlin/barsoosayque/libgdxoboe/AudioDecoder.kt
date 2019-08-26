package barsoosayque.libgdxoboe

import android.content.res.AssetFileDescriptor
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.media.MediaFormat.KEY_CHANNEL_COUNT
import com.badlogic.gdx.utils.GdxRuntimeException
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer

/** Decoder class which can process audio files to PCM.
 * Uses 16-bit Little-Endian samples.
 * @throws IllegalArgumentException If given fd can't be processed */
@Suppress("UNUSED", "DEPRECATION")
class AudioDecoder(fd: AssetFileDescriptor) {
    private val extractor = MediaExtractor().apply {
        setDataSource(fd.fileDescriptor, fd.startOffset, fd.declaredLength)
        fd.close()
    }
    private var isMono = false
    private val decoder = let {
        if (extractor.trackCount > 1) throw GdxRuntimeException("Wrong track count in $fd")
        val mediaFormat = extractor.getTrackFormat(0)
        val mime = mediaFormat.getString(MediaFormat.KEY_MIME)
        if (mime?.startsWith("audio/") == true) {
            isMono = mediaFormat.getInteger(KEY_CHANNEL_COUNT) == 1
            extractor.selectTrack(0)
            MediaCodec.createDecoderByType(mime).apply {
                configure(mediaFormat, null, null, 0)
                start()
            }
        } else null
    } ?: throw GdxRuntimeException("Can't extract audio from \"$fd\".")
    private var info = MediaCodec.BufferInfo()
    private var cachedBuffer: ByteBuffer? = null
    private var cachedIndex: Int = 0
    private var outputBuffer = ByteBuffer.allocateDirect(65536)

    private fun readSampleData(): Boolean? =
            decoder.dequeueInputBuffer(100).takeIf { it >= 0 }?.let { bufferIndex ->
                extractor.readSampleData(decoder.inputBuffers[bufferIndex], 0).let { size ->
                    if (size > 0) {
                        decoder.queueInputBuffer(bufferIndex, 0, size, extractor.sampleTime, 0)
                        extractor.advance()
                        false
                    } else {
                        decoder.queueInputBuffer(bufferIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM)
                        true
                    }
                }
            }

    private fun processSamples(from: ByteBuffer, requestedBytes: Int? = null, processSamples: (Byte) -> Unit): Int =
            if (requestedBytes == null || requestedBytes > 0) {
                when {
                    isMono && (requestedBytes == null || requestedBytes >= 4) -> {
                        val monoFrameSize = (requestedBytes?.let { minOf(from.remaining(), it / 2) }
                                ?: from.remaining()) / 2
                        repeat(monoFrameSize) {
                            val pcmPart1 = from.get()
                            val pcmPart2 = from.get()

                            repeat(2) {
                                processSamples(pcmPart1)
                                processSamples(pcmPart2)
                            }
                        }
                        monoFrameSize * 4
                    }
                    isMono && requestedBytes == 2 -> {
                        if (from.remaining() >= 1) {
                            val pcm = from.get()
                            processSamples(pcm)
                            processSamples(pcm)
                            requestedBytes
                        } else 0
                    }
                    else -> {
                        val size = requestedBytes?.let { minOf(it, from.remaining()) } ?: from.remaining()
                        repeat(size) {
                            val pcm = from.get()
                            processSamples(pcm)
                        }
                        size
                    }
                }
            } else 0

    private fun writeSamples(destination: ByteArrayOutputStream) {
        decoder.dequeueOutputBuffer(info, 100).takeIf { it >= 0 }?.also { bufferIndex ->
            decoder.outputBuffers[bufferIndex].also { buffer ->
                buffer.position(info.offset)
                buffer.limit(info.offset + info.size)
                processSamples(buffer) {
                    destination.write(it.toInt())
                }
            }
            decoder.releaseOutputBuffer(bufferIndex, false)
        }
    }

    private fun writeSamples(destination: ByteBuffer, maximumBytes: Int): Int {
        var bytesProcessed = 0

        val cache = cachedBuffer
        if (cache != null) {
            bytesProcessed = processSamples(cache, maximumBytes) {
                destination.put(it)
            }

            if (cache.remaining() == 0) {
                cachedBuffer = null
                decoder.releaseOutputBuffer(cachedIndex, false)
            }
        } else {
            decoder.dequeueOutputBuffer(info, 100).takeIf { it >= 0 }?.also { bufferIndex ->
                decoder.outputBuffers[bufferIndex].also { buffer ->
                    buffer.position(info.offset)
                    buffer.limit(info.offset + info.size)
                    bytesProcessed = processSamples(buffer, maximumBytes) {
                        destination.put(it)
                    }
                    if (buffer.remaining() > 0) {
                        cachedBuffer = buffer
                        cachedIndex = bufferIndex
                    } else {
                        decoder.releaseOutputBuffer(bufferIndex, false)
                    }
                }
            }
        }

        return bytesProcessed
    }

    private val MediaCodec.BufferInfo.isEof: Boolean
        get() = flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0

    /** Read more data and decode it.
     * @param samples Amount of new samples to be decoded
     * (negative value to read and decode to eof) */
    fun decode(samples: Int): Pcm {
        var stream: ByteArrayOutputStream? = null
        var bytesLeft = samples * 2

        when {
            bytesLeft > outputBuffer.capacity() -> outputBuffer = ByteBuffer.allocateDirect(bytesLeft)
            bytesLeft < 0 -> stream = ByteArrayOutputStream()
        }
        outputBuffer.clear()

        var eofExtractor = false
        var eofDecoder = false

        while (!(eofExtractor && eofDecoder) && bytesLeft != 0) {
            if (!eofExtractor && cachedBuffer == null) {
                eofExtractor = readSampleData() ?: eofExtractor
            }

            eofDecoder = eofDecoder || info.isEof
            if (!eofDecoder) {
                if (stream != null) {
                    writeSamples(stream)
                } else {
                    bytesLeft -= writeSamples(outputBuffer, bytesLeft)
                }
            }
        }

        return stream?.let { ByteBuffer.allocateDirect(it.size()).put(it.toByteArray()) }?.let { Pcm(it, it.limit()) }
                ?: Pcm(outputBuffer, outputBuffer.position())
    }

    /** Read file to EOF and decode it all. */
    fun decode(): Pcm = decode(-1)

    /** Move head of the decoder to the [position] (in seconds) */
    fun seek(position: Float) {
        cachedBuffer = null
        extractor.seekTo((position * 1000).toLong(), MediaExtractor.SEEK_TO_CLOSEST_SYNC)
        info = MediaCodec.BufferInfo()
        decoder.flush()
    }

    /** Release native resources */
    fun dispose() {
        decoder.stop()
        decoder.release()
        extractor.release()
    }

    class Pcm(val data: ByteBuffer, val size: Int)
}