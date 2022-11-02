#pragma once
#include "Instrument.h"
#include "audionode.h"
class CAdditive :
    public CAudioNode
{

public:
	double m_duration;
	double frequnecy;
	double amplitude2;
	double amplitude;
	int index;
	short* audio;
	double CycleVibrato;
	double FreqVib;

	CAdditive(void);
	~CAdditive(void);

	virtual void Start();

	virtual bool Generate();
	
	void VibratoCycle(double d) { CycleVibrato = d; }

	void VibratoFrequency(double d) { FreqVib = d; }

	void SetFreq(double f) { frequnecy = f; }

	void SetAmplitude(double a) { amplitude2 = a; }

	void SetDuration(double d) { m_duration = d; }



	void GenerateWaveTable();


};

