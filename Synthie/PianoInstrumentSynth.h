#pragma once
#include "PianoInstrument.h"
#include "Notes.h"
#include "Notes.h"
#include <vector>
class CPianoInstrumentSynth
{

public:
	CPianoInstrumentSynth(void);
	~CPianoInstrumentSynth(void);

	CPianoInstrument* CreateInstrument();
	void SetNote(CNote* note);
	bool LoadSoftPiano(const char*);
	bool LoadLoudPiano(const char*);
	void Interpolate(double dyna);
	bool PedalPress(const char*);
	bool PedalRelease(const char*);

	bool PianoLoad(const char* pianofile, std::vector<short>& table);

private:
	double m_dynam;
	bool m_sustain;
	CPianoInstrument* m_instrument;
	std::vector<short> m_wave; //change
	std::vector<short> m_softwave;
	std::vector<short> m_loudwave;
	std::vector<short> m_pedalPress;
	std::vector<short> m_pedalReleased;
	

};

