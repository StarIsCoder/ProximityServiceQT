#ifndef __Alto__AudioAnalyzer2__
#define __Alto__AudioAnalyzer2__

struct AudioParams {

public:
    AudioParams(float sampleRateHz = 44100.0f) :
    sampleRateHz(sampleRateHz),
    
    slidingAvgFactor(0.47f),
    slidingAvgFactorRightBit(0.108f),
    slidingAvgFactorWrongBit(0.0024f),
    avgSignalStrengthConvFactor(0.0205f),
    currentLaneWeight(0.7f),
    offsetAdjustmentFactor(1013.0f),
    reflectionFactor(0.178f),
    topBand0MagnitudeWeightMin(0.4f),
    topBand0MagnitudeWeightFactor(1.1f),
    topBand0MagnitudeWeightMax(1.02f),
    botBand1MagnitudeWeightMin(0.4f),
    botBand1MagnitudeWeightFactor(1.0f),
    botBand1MagnitudeWeightMax(1.0f),
    topBand1MagnitudeWeightMin(0.4f),
    topBand1MagnitudeWeightFactor(0.8f),
    topBand1MagnitudeWeightMax(1.0f),
    botBand2MagnitudeWeightMin(0.4f),
    botBand2MagnitudeWeightFactor(1.0f),
    botBand2MagnitudeWeightMax(1.0f),
    ma_averageBitsConvFactor(.69f)
    {
    }

    float sampleRateHz;
    float slidingAvgFactor;
    float slidingAvgFactorRightBit;
    float slidingAvgFactorWrongBit;
    float avgSignalStrengthConvFactor;
    float currentLaneWeight;
    float offsetAdjustmentFactor;
    float reflectionFactor;
    
    float topBand0MagnitudeWeightMin;
    float topBand0MagnitudeWeightFactor;
    float topBand0MagnitudeWeightMax;
    
    float botBand1MagnitudeWeightMin;
    float botBand1MagnitudeWeightFactor;
    float botBand1MagnitudeWeightMax;

    float topBand1MagnitudeWeightMin;
    float topBand1MagnitudeWeightFactor;
    float topBand1MagnitudeWeightMax;

    float botBand2MagnitudeWeightMin;
    float botBand2MagnitudeWeightFactor;
    float botBand2MagnitudeWeightMax;
    
    float ma_averageBitsConvFactor;
};

class AudioAnalyzer {
public:
    AudioAnalyzer (const AudioParams& audioParams);
    virtual ~AudioAnalyzer();
    void analyze (float * data);
    bool is_data_ready () const;
    float get_bit_in_band (int band) const;
    int get_offset_adjustment () const;
    void reset();
    float get_ultrasound_signal_level () const;
    float get_ultrasound_noise_level () const;

private:
    void fade(float * samples);
    void updateSignalStrength();
    void updateOffsetAdjustment();
    void selectPrimaryLane();
    float energy(float * data, int band);

    static const int fft_length = 512;
    static const int subBandFftWidth = 4;
    const float sliding_avg_conv_factor_base;
    const float sliding_avg_factor_right_bit;
    const float sliding_avg_factor_wrong_bit;
    float hzPerIndex;
    int bands[3];
    int laneIdx;
    int offsetAdjustment;
    int signalLane;
    float signalLaneBand[3][3];
    float avgSignalLaneBand[3][3];
    float avgSignalOneLaneBand[3][3];
    float avgSignalZeroLaneBand[3][3];
    float avgSignalStrengthLane[3];

    const float avg_signal_strength_conv_factor;
    const float currentLaneWeight;
    const float offset_adjustment_factor;
    float * fadeTable;
    float reflectionFactor;
    static const int MAX_OFFSET_ADJUSTMENT = 10;

    
    float topBand0MagnitudeWeightMin;
    float topBand0MagnitudeWeightFactor;
    float topBand0MagnitudeWeightMax;
    
    float botBand1MagnitudeWeightMin;
    float botBand1MagnitudeWeightFactor;
    float botBand1MagnitudeWeightMax;
    
    float topBand1MagnitudeWeightMin;
    float topBand1MagnitudeWeightFactor;
    float topBand1MagnitudeWeightMax;
    
    float botBand2MagnitudeWeightMin;
    float botBand2MagnitudeWeightFactor;
    float botBand2MagnitudeWeightMax;
};
#endif /* defined(__Alto__AudioAnalyzer2__) */
