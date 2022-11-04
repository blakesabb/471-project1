#pragma once
class CAudioNode
{
public:
	CAudioNode();
	CAudioNode(double bpm);

	virtual ~CAudioNode();
public:


public:
	double m_sampleRate;
	double m_samplePeriod;
	double m_bpm;
	double m_frame[2];
	double spb; // added


	virtual void Start() = 0;

	virtual bool Generate() = 0;

	double GetSampleRate() { return m_sampleRate; }

	double GetSamplePeriod() { return m_samplePeriod; }

	double GetSecondsPerBeat() { return spb; } //added for 

	void SetSampleRate(double s) { m_sampleRate = s;  m_samplePeriod = 1 / s; }

	const double *Frame() { return m_frame; }

	double Frame(int c) { return m_frame[c]; }

	void SetSecondsPerBeat(double inp) { spb = inp; } //added for sp



};

