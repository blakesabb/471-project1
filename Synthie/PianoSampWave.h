#pragma once
#include "AudioNode.h"
#include "audio/DirSoundSource.h"
#include <vector>

class CPianoSampWave :
    public CAudioNode
{

public:
    CPianoSampWave(void);
    ~CPianoSampWave(void);

    virtual void Start();
    virtual bool Generate();
    void SetFreq(double f) {
        m_freq = f;
    }
    void SetAmplitude(double a) {
        m_amp = a;
    }
    void SetSamples(std::vector<short>* s, int n) {
        m_samplewave = s;
        m_numsamples = n;
    }
    void Pedal(std::vector<short>*);

private:
    std::vector<short> *m_samplewave,  *m_pedalwave;
    int m_pedalCount;
    int m_numsamples;
    double m_freq;
    double m_amp;
    double m_sample;
    double m_rate;
    double tmp;
    int count;


};

