#pragma once
#include "Instrument.h"
#include "Additive.h"


class CSynthesizerAdd :
	public CInstrument
{
public:
	CSynthesizerAdd(void);
	virtual ~CSynthesizerAdd(void);
public:
	CAdditive   sinewave;
	double duration;
	double time;
	double release;
	double attack;
	double CrossIn;
	double CrossOut;
	double sustain;
	double decay;

	virtual void Start(); //given in steps 
	virtual bool Generate(); //given  in steps 
	virtual void SetNote(CNote* note); //given in steps 
	
	
	void SetCrossFadeIn(double ci)

	{
		attack = GetSecondsPerBeat() * ci; //attakc is the corss in times SPB
	}

	void SetCrossFadeOut(double co)
	{
		release = GetSecondsPerBeat() * co; //release is the cross out times SPB
	}
	 
	void SetDuration(double d) 
	{ 
		duration = d * GetSecondsPerBeat(); // we want full duration for wavetable
	
	}
	void SetFreq(double f) { sinewave.SetFreq(f); } //given in steps 
	void SetAmplitude(double& a) { sinewave.SetAmplitude(a); }	 //given in steps 
	



};

