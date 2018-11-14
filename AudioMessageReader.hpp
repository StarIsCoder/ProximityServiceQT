#ifndef __Alto__AudioMessageReader__
#define __Alto__AudioMessageReader__

#include "AudioAnalyzer.hpp"
#include "RingBuffer.hpp"
#include "MessageListener.hpp"
#include <cstddef>

class MessageReceiver;
class MessageAssembler;

class AudioMessageReader : public MessageListener {
public:
    AudioMessageReader(MessageReceiver * messageReceiver);
    AudioMessageReader(MessageReceiver * messageReceiver, const AudioParams& params);
    ~AudioMessageReader();
    void setErrorCorrection(int bitFlipCount);
    void receiveAudioData(const void * data, size_t length);
    virtual void onMessage(const unsigned char * message, int length);
    float getUltrasoundSignalLevel () const;
    float getUltrasoundNoiseLevel () const;

private:
    void init(const AudioParams& params);
    AudioAnalyzer * audioAnalyzer;
    RingBuffer ringBuffer;
    MessageAssembler * messageAssembler;
    MessageReceiver * messageReceiver;
    float sampleRateHz;
};
#endif /* defined(__Alto__AudioMessageReader__) */
