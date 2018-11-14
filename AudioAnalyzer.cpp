#include "AudioAnalyzer.hpp"

#include "fft.h"

#include <cassert>
#include <cmath>
#include <memory>

AudioAnalyzer::AudioAnalyzer (const AudioParams& params) :
    sliding_avg_conv_factor_base(params.slidingAvgFactor),
    sliding_avg_factor_right_bit(params.slidingAvgFactorRightBit),
    sliding_avg_factor_wrong_bit(params.slidingAvgFactorWrongBit),
    hzPerIndex(0),
    laneIdx(0),
    offsetAdjustment(0),
    signalLane(0),
    signalLaneBand{{0}},
    avgSignalLaneBand{{0}},
    avgSignalOneLaneBand{{0}},
    avgSignalZeroLaneBand{{0}},
    avgSignalStrengthLane{0},
    avg_signal_strength_conv_factor(params.avgSignalStrengthConvFactor),
    currentLaneWeight(params.currentLaneWeight),
    offset_adjustment_factor(params.offsetAdjustmentFactor),
    fadeTable(new float[fft_length/2]),
    reflectionFactor(params.reflectionFactor),

    topBand0MagnitudeWeightMin(params.topBand0MagnitudeWeightMin),
    topBand0MagnitudeWeightFactor(params.topBand0MagnitudeWeightFactor),
    topBand0MagnitudeWeightMax(params.topBand0MagnitudeWeightMax),

    botBand1MagnitudeWeightMin(params.botBand1MagnitudeWeightMin),
    botBand1MagnitudeWeightFactor(params.botBand1MagnitudeWeightFactor),
    botBand1MagnitudeWeightMax(params.botBand1MagnitudeWeightMax),

    topBand1MagnitudeWeightMin(params.topBand1MagnitudeWeightMin),
    topBand1MagnitudeWeightFactor(params.topBand1MagnitudeWeightFactor),
    topBand1MagnitudeWeightMax(params.topBand1MagnitudeWeightMax),

    botBand2MagnitudeWeightMin(params.botBand2MagnitudeWeightMin),
    botBand2MagnitudeWeightFactor(params.botBand2MagnitudeWeightFactor),
    botBand2MagnitudeWeightMax(params.botBand2MagnitudeWeightMax)
{
    hzPerIndex = params.sampleRateHz / fft_length;

    bands[0] = .5 + 20585 / hzPerIndex;
    bands[1] = .5 + 20930 / hzPerIndex;
    bands[2] = .5 + 21274 / hzPerIndex;
    reset();
    
    // Cosine windowing function. (Tested Hamming, Hann, Blackman, Triangle and more...)
    // TODO: Test Nuttall, flat top, Rife-Vincent, triangle/rectangle, combinations...
    for(int i = 0; i < fft_length/2; i++)
        fadeTable[i] = sinf(.5 * M_PI * i / (fft_length/2 - 1));
}

AudioAnalyzer::~AudioAnalyzer ()
{
    delete [] fadeTable;
}

void AudioAnalyzer::analyze (float * samples)
{
    fade(samples);
    double alignhack[fft_length/2];
    float * scratchpad = reinterpret_cast<float *>(alignhack);
    const int fftStart = bands[0];
    const int fftWidth = bands[2] + subBandFftWidth - bands[0];
    fft_fftReal512SparseProcess(samples, scratchpad, fftStart, fftWidth);

    for (int band = 0; band < 3; band++) {
        float magnitudes[subBandFftWidth];
        for (int i = 0; i < subBandFftWidth; i++) {
            const float re = samples[2 * (bands[band] + i)];
            const float im = samples[2 * (bands[band] + i) + 1];
            magnitudes[i] = sqrt(re * re + im * im);
        }

        const float e = fminf(energy(magnitudes, band), 1.0f);
        
        float right = sliding_avg_factor_right_bit;
        float wrong = sliding_avg_factor_wrong_bit;
        
        if(e > avgSignalLaneBand[laneIdx][band]) {
            avgSignalOneLaneBand[laneIdx][band] = avgSignalOneLaneBand[laneIdx][band] * (1.0F - right) + e * right;
            avgSignalZeroLaneBand[laneIdx][band] = avgSignalZeroLaneBand[laneIdx][band] * (1.0F - wrong) + e * wrong;
        }
        else {
            avgSignalOneLaneBand[laneIdx][band] = avgSignalOneLaneBand[laneIdx][band] * (1.0F - wrong) + e * wrong;
            avgSignalZeroLaneBand[laneIdx][band] = avgSignalZeroLaneBand[laneIdx][band] * (1.0F - right) + e * right;
        }

        float factor = sliding_avg_conv_factor_base;
        avgSignalLaneBand[laneIdx][band] = avgSignalLaneBand[laneIdx][band] * (1.0F - factor) + e * factor;

        
        signalLaneBand[laneIdx][band] = e - currentLaneWeight * (avgSignalOneLaneBand[laneIdx][band] + avgSignalZeroLaneBand[laneIdx][band])/2
                                          - (1 - currentLaneWeight) *
                                                (avgSignalLaneBand[(laneIdx+1)%3][band] + avgSignalLaneBand[(laneIdx+2)%3][band]) / 2
                                          - signalLaneBand[laneIdx][band] * reflectionFactor;
    }
    updateSignalStrength();
    updateOffsetAdjustment();
    selectPrimaryLane();
    
    laneIdx = (laneIdx+1)%3;
}

bool AudioAnalyzer::is_data_ready () const
{
    return ((signalLane +1)%3) == laneIdx;
}

float AudioAnalyzer::get_bit_in_band (int band) const
{
    return signalLaneBand[(signalLane+3-band)%3][band];
}

int AudioAnalyzer::get_offset_adjustment () const
{
    return offsetAdjustment;
}

void AudioAnalyzer::reset()
{
    laneIdx = 0;
    offsetAdjustment = 0;
    signalLane = 0;
    for(int lane = 0; lane < 3; lane++) {
        avgSignalStrengthLane[lane] = 0.0f;
        for(int b = 0; b < 3; b++) {
            signalLaneBand[lane][b] = 0.0f;
            avgSignalLaneBand[lane][b] = 0.0f;
            avgSignalOneLaneBand[lane][b] = 0.0f;
            avgSignalZeroLaneBand[lane][b] = 0.0f;
        }
    }
}

float AudioAnalyzer::get_ultrasound_signal_level () const
{
    float signal = 0.0f;
    for(int band = 0; band < 3; band++) {
        signal +=
        avgSignalLaneBand[(signalLane+3-band)%3][band]*2
        -avgSignalLaneBand[(signalLane+4-band)%3][band]
        -avgSignalLaneBand[(signalLane+5-band)%3][band];
    }
    return signal;
}

float AudioAnalyzer::get_ultrasound_noise_level () const
{
    float signal = 0.0f;
    for(int band = 0; band < 3; band++) {
        signal += avgSignalLaneBand[0][band];
        signal += avgSignalLaneBand[1][band];
        signal += avgSignalLaneBand[2][band];
    }
    return signal;
}

void AudioAnalyzer::fade(float*data)
{
// TODO: Adapt window function when frequency bands are contaminating each other,
//    rather than adapting energy function?
    for(int i = 0; i < fft_length/2; i++) {
        data[i] *= fadeTable[i];
        data[-i + fft_length - 1] *= fadeTable[i];
    }
}

void AudioAnalyzer::updateSignalStrength()
{
    const float FACTOR = avg_signal_strength_conv_factor;
    
    float signalStrength = 0.0f;
    signalStrength += fabsf(signalLaneBand[laneIdx][0]);
    signalStrength += fabsf(signalLaneBand[(laneIdx+2)%3][1]);
    signalStrength += fabsf(signalLaneBand[(laneIdx+1)%3][2]);
    avgSignalStrengthLane[laneIdx] = avgSignalStrengthLane[laneIdx] * (1-FACTOR) + signalStrength * FACTOR;
}

void AudioAnalyzer::updateOffsetAdjustment()
{
     // TODO: improve adaption algorithm. Fast and precise adaption.
     // Predict optimal adjustment based on all 3 strength variables?
     // Modify avgSignalStrengthLane when adjusting offset to reflect what the vaues would have been if the adjustment was made earlier?

    if(laneIdx == signalLane) {
        // TODO: Can we use warmup to improve offset adjustment? (reset offset adjustment warmup counter on signal lane change?)
        float diff = avgSignalStrengthLane[(signalLane+1)%3] - avgSignalStrengthLane[(signalLane+2)%3];
        offsetAdjustment = diff * offset_adjustment_factor;

        if(offsetAdjustment > MAX_OFFSET_ADJUSTMENT)
            offsetAdjustment = MAX_OFFSET_ADJUSTMENT;
        else if(offsetAdjustment < -MAX_OFFSET_ADJUSTMENT)
            offsetAdjustment = -MAX_OFFSET_ADJUSTMENT;
    }
}

void AudioAnalyzer::selectPrimaryLane ()
{
    if(laneIdx == signalLane) {
        if(avgSignalStrengthLane[0] < avgSignalStrengthLane[1]) {
            if(avgSignalStrengthLane[1] < avgSignalStrengthLane[2])
                signalLane = 2;
            else
                signalLane = 1;
        }
        else {
            if(avgSignalStrengthLane[0]<avgSignalStrengthLane[2])
                signalLane = 2;
            else
                signalLane = 0;
        }
    }
}

float AudioAnalyzer::energy(float * data, int band)
{
    const float band0 = avgSignalOneLaneBand[(signalLane+3-0)%3][0];
    const float band1 = avgSignalOneLaneBand[(signalLane+3-1)%3][1];
    const float band2 = avgSignalOneLaneBand[(signalLane+3-2)%3][2];

    switch (band) {
        case 0: {
            float f3 = fminf(topBand0MagnitudeWeightMin + topBand0MagnitudeWeightFactor * band0/band1, topBand0MagnitudeWeightMax);
            return (data[0] + data[1] + data[2] + f3 * data[3]) / (3 + f3);
        }
        case 1: {
            float f0 = fminf(botBand1MagnitudeWeightMin + botBand1MagnitudeWeightFactor * band1/band0, botBand1MagnitudeWeightMax);
            float f3 = fminf(topBand1MagnitudeWeightMin + topBand1MagnitudeWeightFactor * band1/band2, topBand1MagnitudeWeightMax);
            return (f0 * data[0] + data[1] + data[2] + f3 * data[3]) / (f0 + 2 + f3);
        }
        case 2: {
            float f0 = fminf(botBand2MagnitudeWeightMin + botBand2MagnitudeWeightFactor * band2/band1, botBand2MagnitudeWeightMax);
            return (f0 * data[0] + data[1] + data[2] + data[3]) / (f0 + 3);
        }
    }
    assert(false);
    return 0;
}
