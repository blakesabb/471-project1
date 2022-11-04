#pragma once
#include "Instrument.h"
#include "PianoSampWave.h"
#include "PianoEnvelopes.h"
#include "Notes.h"


class CPianoInstrument :
    public CInstrument
{
 
public:
    CPianoInstrument(void);
    ~CPianoInstrument(void);
    std::wstring Name() const{return L"PianoInstrument"; }
public:
    virtual void Start();
    virtual bool Generate();
    void SetFreq(double f) { m_pianonote.SetFreq(f); }
    void SetAmplitude(double a) { m_pianonote.SetAmplitude(a); }
    void SetDuration(double d) {m_envelope.SetDuration(d);}
    void SetNote(CNote* note);
    void Sustain(bool s) {m_envelope.Sustain(s);}
    void GetFrame(double frame[2]) {frame[0] = m_frame[0];frame[1] = m_frame[1]; }
    CPianoSampWave* GetPlayer() { return &m_pianonote; }

private:
    CPianoSampWave m_pianonote;
    double m_duration;
    double m_time;
    CPianoEnvelopes m_envelope;



    };