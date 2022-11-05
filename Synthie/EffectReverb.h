#pragma once
#include <vector>
#include "Note.h"

class EffectReverb
{
public:
	EffectReverb();

	void Process(double* frameIn, double* frameOut);
	void SetNote(CNote* note);
private:
	int m_wrloc = 0;
	int m_rdloc = 1; // set to 1 in chapter 4 notes
	int m_sampleRate = 44100;
	//short* m_queue = new short[m_sampleRate + 1];
	std::vector<double> m_queue;
	double m_dry = 1;
	double m_wet = 0;
	double m_delay = 0;
	CNote* m_note;
};

