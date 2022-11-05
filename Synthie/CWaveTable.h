#pragma once
#include "Instrument.h"
#include "SineWave.h"

class CWaveTable :
    public CInstrument
{

public:

    CWaveTable();
    CWaveTable(float bpm);
    virtual ~CWaveTable();

    // Setter and Getter for Frequency 
    void SetFreq(double f) { m_sinewave.SetFreq(f); }



    void SetAmplitude(double a) { m_sinewave.SetAmplitude(a); }
    void SetDuration(double d) { m_duration = d; }

    // Setter for ADSR

    void SetAttack(double a) { m_attack = a; }
    void SetDecay(double d) { m_decay = d; }
    void SetSustain(double s) { m_sustain = s; }
    void SetRelease(double r) { m_release = r; }


    virtual void Start();
    virtual bool Generate();

    virtual void SetNote(CNote* note);


private:
    CSineWave   m_sinewave;
    double m_duration;
    double m_time;

    double m_attack = 0.05;
    double m_decay = m_attack;
    double m_sustain;
    double m_release = 0.1;

    // Getting frequency 
    double m_freq;

    double m_amplitute = 1;

};

