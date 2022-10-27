#include "stdafx.h"
#include "Instrument.h"


CInstrument::CInstrument()
{
}

// Constructor to set the beats per minute
CInstrument::CInstrument(double bpm)
{
	m_bpm = bpm;
}

CInstrument::~CInstrument()
{
}
