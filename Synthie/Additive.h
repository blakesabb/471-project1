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
	double amplitude;
	double freq;
	double CycleVib;
	double FreqVibrato;
	int loc;
	short* audio;
	double amplitude2[8]; // second amplitide of length 8
	// -----------------



public:
	virtual void Start();

	virtual bool Generate();

	void SetFreq(double f) { freq = f; }

	


	void SetDuration(double d) { duration = d; }

	// declare new funcs
	// -----------------------
	void GenerateWaveTable();

	void CycleVibrato(double cv); //ctcle vib

	void FrequencyVibrato(double fv); // vib freq


	void SetAmplitude(double& a)
	{
		memcpy(&amplitude2, &a, sizeof(double) * 8); // set amp in memeory
	}

};


