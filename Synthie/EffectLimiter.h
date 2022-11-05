#pragma once
#include "Note.h"

class EffectLimiter
{
public:
	EffectLimiter();

	void Process(double* frameIn, double* frameOut);
	void SetNote(CNote* note);
private:
	double m_limit = 0.00001;
	double m_limitMultiplier = 1;
	int m_wrloc = 0;
	int m_rdloc = 1; // set to 1 in chapter 4 notes
	int m_sampleRate = 44100;
	int m_amplitude = 1;

	double m_dry = 1;
	double m_wet = 0;
	double m_delay = 0;
	CNote* m_note;

	bool m_goUp = true;
};

