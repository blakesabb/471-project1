#pragma once
#include "Instrument.h"
#include "audionode.h"
class CAdditive :
	public CAudioNode
{
public:
	CAdditive(void);
	~CAdditive(void);
public:
	//! Start audio generation
	virtual void Start();

	//! Generate one frame of audio
	virtual bool Generate();

	//! Set the sine wave frequency
	void SetFreq(double f) { m_freq = f; }

	//! Set the sine wave amplitude
	void SetAmplitude(double& a) { memcpy(&m_amp, &a, sizeof(double) * 8); }

	void SetDuration(double d) { m_duration = d; }

	void SetVibratoRate(double d) { m_vibratoRate = d; }

	void SetVibratoFreq(double d) { m_vibratoFreq = d; }

	void GenerateWaveTable();

private:
	double m_duration;
	double m_freq;
	double m_amp[8];
	double m_amplitude;
	int index;
	short* audio;
	double m_vibratoRate;
	double m_vibratoFreq;
};


