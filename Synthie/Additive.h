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
	// init valaibles
	// ----------------
	double duration;
	double amp[8];
	double amplitude;
	double freq;
	double CycleVib;
	double FreqVibrato;
	int loc;
	short* audio;
	// -----------------



public:
	virtual void Start();

	virtual bool Generate();

	void SetFreq(double f) { freq = f; }

	


	void SetDuration(double d) { duration = d; }

	// declare new funcs
	// -----------------------
	void GenerateWaveTable();

	void CycleVibrato(double cv);

	void FrequencyVibrato(double fv);


	void SetAmplitude(double& a)
	{
		amplitude = a;
	}

};


