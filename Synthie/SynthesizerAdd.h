#pragma once
#include "Additive.h"
#include "Instrument.h"

class CSynthesizerAdd :
    public CInstrument
{


public:
	CSynthesizerAdd(void);
	virtual ~CSynthesizerAdd(void);
public:
	virtual void Start();
	virtual bool Generate();

	void SetFreq(double f) { m_sinewave.SetFreq(f); }
	void SetAmplitude(double& a) { m_sinewave.SetAmplitude(a); }
	void SetDuration(double d) { m_duration = d; }
	void SetCrossFadeIn(double d) { attack = d; }
	void SetCrossFadeOut(double d) { release = d; }
	virtual void SetNote(CNote* note);

private:
	CAdditive  m_sinewave;
	double m_duration;
	double m_time;
	double release;
	double attack;
	double sustain;
	double decay;
	double CrossIn;
	double CrossOut;

};

