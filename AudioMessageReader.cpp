#include "AudioMessageReader.hpp"
#include "MessageReceiver.hpp"
#include "MessageAssembler.hpp"

AudioMessageReader::AudioMessageReader (MessageReceiver * messageReceiver) :
    audioAnalyzer(nullptr),
    ringBuffer(40960),
    messageAssembler(nullptr),
    messageReceiver(messageReceiver)
{
    AudioParams params;
    init(params);
}

AudioMessageReader::AudioMessageReader (MessageReceiver * messageReceiver, const AudioParams& params) :
    audioAnalyzer(nullptr),
    ringBuffer(40960),
    messageAssembler(nullptr),
    messageReceiver(messageReceiver)
{
    init(params);
}

AudioMessageReader::~AudioMessageReader ()
{
    delete messageAssembler;
    delete audioAnalyzer;
}

void AudioMessageReader::setErrorCorrection(int bitFlipCount) {
    messageAssembler->setBitFlips(bitFlipCount);
}

void AudioMessageReader::receiveAudioData(const void * data, size_t length) {
    ringBuffer.write(reinterpret_cast<const char*>(data), length);
    while(ringBuffer.size() >= 512 * sizeof(float)){

        double align[256];
        float * buffer = reinterpret_cast<float*>(align);
        ringBuffer.peak(reinterpret_cast<char*>(buffer), 512*sizeof(float));
        audioAnalyzer->analyze(buffer);
        
        if(audioAnalyzer->is_data_ready()) {
            for(int band = 0; band < 3; band++) {
                float bit = audioAnalyzer->get_bit_in_band(band);
                messageAssembler->addBit(bit);
            }
            messageAssembler->symbolAdded();
        }
        unsigned int symbolLength = (unsigned int)(sampleRateHz / 100);
        unsigned int advance = (symbolLength + audioAnalyzer->get_offset_adjustment()) * sizeof(float);
        if(advance > ringBuffer.size())
            break;
        ringBuffer.advance(advance);
    }
}

void AudioMessageReader::onMessage(const unsigned char * message, int length) {
    
    ReceptionInfo receptionInfo = {};
    receptionInfo.bitsSinceLastMessage = messageAssembler->getBitsSinceLastMessage();
    receptionInfo.errorCorrectionCount = messageAssembler->getErrorCorrectionCount();
    receptionInfo.noiseLevel = audioAnalyzer->get_ultrasound_noise_level();
    receptionInfo.signalLevel = audioAnalyzer->get_ultrasound_signal_level();
    
    audioAnalyzer->reset();
    messageReceiver->onMessage(message, length, receptionInfo);
}

float AudioMessageReader::getUltrasoundSignalLevel () const {
    return audioAnalyzer->get_ultrasound_signal_level();
}

float AudioMessageReader::getUltrasoundNoiseLevel () const {
    return audioAnalyzer->get_ultrasound_noise_level();
}

void AudioMessageReader::init(const AudioParams& params)
{
    messageAssembler = new MessageAssembler(this, 5, params.ma_averageBitsConvFactor);
    audioAnalyzer = new AudioAnalyzer(params);
    audioAnalyzer->reset();
    sampleRateHz = params.sampleRateHz;
}
