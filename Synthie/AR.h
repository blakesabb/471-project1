#pragma once
#include "AudioNode.h"
class CAR :
	public CAudioNode
{
public:
	CAR();
	~CAR();
private:
	double m_duration;
	double m_time;
	double m_attack;
	double m_release;
	CAudioNode* m_source;
public:
	void SetSource(CAudioNode* const &source) { m_source = source; }
	void SetDuration(double d) { m_duration = d; }
	virtual void Start();
	virtual bool Generate();
};

