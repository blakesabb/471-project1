#pragma once
#include "AudioNode.h"
class CPianoEnvelopes :
    public CAudioNode
{
public:
    CPianoEnvelopes(void);
    ~CPianoEnvelopes(void);

public:
    virtual void Start();
    virtual bool Generate();

    void SetAttack(double a) { m_attack = a; }
    void SetRelease(double r) { m_release = r; }
    void SetDuration(double d) { m_duration = d; }
    void SetSource(CAudioNode* node) { m_source = node; }
    void Sustain(bool s) { m_sustain = s; }


private:
    double m_attack;
    double m_time;
    CAudioNode* m_source;
    double  m_release;
    double m_duration;
    bool m_sustain;
    double g_bin;
    bool m_pedalPressed;
};

