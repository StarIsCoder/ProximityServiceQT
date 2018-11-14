#include "recordslotmac.h"
#include <sstream>
#include <iomanip>

void audioInputCallback(void *cbData, AudioQueueRef, AudioQueueBufferRef buffer,
                        const AudioTimeStamp *, UInt32, const AudioStreamPacketDescription *)
{
    AudioRecorderMac *recorder = reinterpret_cast<AudioRecorderMac *>(cbData);
    recorder->processBuffer(buffer);
}

AudioRecorderMac::AudioRecorderMac():
     format{}
{
    format.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mBitsPerChannel = 32;
    format.mChannelsPerFrame = 1;
    format.mBytesPerFrame = (format.mBitsPerChannel / 8) * format.mChannelsPerFrame;
    format.mFramesPerPacket = 1;
    format.mBytesPerPacket = format.mFramesPerPacket * format.mBytesPerFrame;
    format.mReserved = 0;
    format.mSampleRate = 44100;

    mMessageRetainer = new MessageRetainer;
    AudioParams params(44000.0f);
    mAudioMessageReader = new AudioMessageReader(mMessageRetainer, params);
}

void AudioRecorderMac::startRecord(){
    OSStatus err = noErr;
        initializeQueue();

        err = enqueueBuffers();
        if (err != noErr) {
            AudioQueueDispose(queue, true);
            return;
        }

        err = AudioQueueStart(queue, nullptr);
        if (err != noErr) {
            AudioQueueStop(queue, true);
            AudioQueueDispose(queue, true);

            if (err == kAudioQueueErr_InvalidDevice)
                ; // NoMic
            else
                ; // Unknow Error
            return;
        }
}


void AudioRecorderMac::initializeQueue() {
    auto ret = AudioQueueNewInput(&format, audioInputCallback, this, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &queue);
    if (ret != noErr) {
        return;
    }

    for (size_t i = 0; i != (sizeof buffers / sizeof *buffers); ++i) {
        if (AudioQueueAllocateBuffer(queue, buffer_size, buffers + i) != noErr)
            return;
    }
}

//void RecordSlot::processCFRunLoop() {
//    while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true) == kCFRunLoopRunHandledSource)
//        ;
//}

OSStatus AudioRecorderMac::enqueueBuffers() {
    for (size_t i = 0; i != (sizeof buffers / sizeof *buffers); ++i) {
        OSStatus err = AudioQueueEnqueueBuffer(queue, buffers[i], 0, nullptr);
        if (err != noErr) {
            return err;
        }
    }
    return noErr;
}



void AudioRecorderMac::processBuffer(AudioQueueBufferRef buffer) {
    if (!buffer->mAudioData) {
        ;
    } else if (buffer->mAudioDataByteSize > buffer_size || (buffer->mAudioDataByteSize % sizeof(float))) {
        ;
    } else {
        mAudioMessageReader->receiveAudioData(buffer->mAudioData,buffer->mAudioDataByteSize);
        if(mMessageRetainer->get_and_reset_message_flag()) {
            qDebug() << "get buffer success ";
                const unsigned char* bytes = mMessageRetainer->message();
                if(bytes[6] == 0) {
                    // Webex Teams Token
                    return;
                }
                std::stringstream ipSteam;
                ipSteam << (int)bytes[0] << '.' << (int)bytes[1] << '.'
                << (int)bytes[2] << '.' << (int)bytes[3];
                QString ip = QString::fromStdString(ipSteam.str());

                std::stringstream tokenSteam;
                tokenSteam << std::hex << std::setfill('0')
                << std::setw(2) << int(bytes[4])
                << std::setw(2) << int(bytes[5]);
                QString token = QString::fromStdString(tokenSteam.str());
                qDebug() << "ip :" << ip;
                qDebug() << "token :" << token;
            }
    }

    OSStatus err = AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
    if (err != noErr)
        ;
}
