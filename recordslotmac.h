#ifndef RECORDSLOT_H
#define RECORDSLOT_H

#include <QObject>
#include <QDebug>
#include <QAudioRecorder>
#include <QAudioProbe>
#include "MessageRetainer.hpp"
#include "AudioMessageReader.hpp"
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioFormat.h>

static const int buffer_size = 10 * 1024;
class AudioRecorderMac
{
public:
    AudioRecorderMac();
    void startRecord();
private:
    void initializeQueue();
    void processCFRunLoop();
    OSStatus enqueueBuffers();

    QAudioRecorder *audioRecorder;
    QAudioProbe *probe;
    MessageRetainer* mMessageRetainer;
    AudioMessageReader* mAudioMessageReader;

    AudioStreamBasicDescription format;
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[3];
protected:
    void processBuffer(AudioQueueBufferRef buffer);
    friend void audioInputCallback(void *cbData, AudioQueueRef,
                                       AudioQueueBufferRef buffer,
                                       const AudioTimeStamp *, UInt32,
                                       const AudioStreamPacketDescription *);
};

#endif // RECORDSLOT_H
