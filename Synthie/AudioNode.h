#pragma once
class CAudioNode
{
public:
	CAudioNode();
	CAudioNode(double bpm);

	virtual ~CAudioNode();
public:
	//! Start the node generation
	virtual void Start() = 0;

	//! Cause one sample to be generated
	virtual bool Generate() = 0;

	//! Get the sample rate in samples per second
	double GetSampleRate() { return m_sampleRate; }

	//! Get the sample period in seconds
	double GetSamplePeriod() { return m_samplePeriod; }

	//! Set the sample rate
	void SetSampleRate(double s) { m_sampleRate = s;  m_samplePeriod = 1 / s; }

	//! Access a generated audio frame
	const double *Frame() { return m_frame; }

	//! Access one channel of a generated audio frame
	double Frame(int c) { return m_frame[c]; }

protected:
	double m_sampleRate;
	double m_samplePeriod;
	double m_bpm;
	double m_frame[2];
};

