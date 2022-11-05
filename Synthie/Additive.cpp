#include "stdafx.h"
#include "Additive.h"




CAdditive::CAdditive(void)
{
	// set vals
	// --------------------
	amplitude2[0] = 1; // first value as non zero

	for (int i = 1; i < 8; i++) // initalize size of the second amplitide at 0 for the size 
	{
		
		amplitude2[i] = 0;

	}	

	amplitude = 500;

	FreqVibrato = 0;

	CycleVib = 0;

	freq = 440;
	// ------------------------



}

CAdditive::~CAdditive(void)
{
	
	// delete audio at all indexes since we use new 
	delete[] audio;

	audio = NULL;
}

void CAdditive::Start()
{
	audio = new short[int(GetSampleRate() * duration)]; // get the new audio for the full length of the sample 

	loc = 0; // initialize this lcoation

	CAdditive::GenerateWaveTable();
}

bool CAdditive::Generate()
{
	//double loc_new = loc + 1; // get new loc at the nect loication 

	m_frame[0] = audio[loc++]; // set audio serio at the location over from the current (new) location

	m_frame[1] = m_frame[0]; // set frame 1 to frame 0 whch as at the new location

	return true;
}

void CAdditive::CycleVibrato(double cv)
{
	CycleVib = cv;
}

void CAdditive::FrequencyVibrato(double fv)
{
	FreqVibrato = fv;
}

short Overflow(double d)
{

	// get the overflow inclase. Using 3000 as the overflow value. 


	if (d > 3000)
		
		return 3000;

	else if (d < -3000)
			
		return -3000;

	return short(d);
}

void CAdditive::GenerateWaveTable()
{


	double vibangle2 = 0; //angle in radians for the vibrato


	double sinangle = 0; // angle for the sin angle


	double time = 0; // set time 

	auto full_dur = duration * GetSampleRate(); // full duration: what will will iterate to 

	for (int i = 0; i < int(full_dur); i++, time = time + 1./ GetSampleRate()) // if the iter is less than the full duration
	{

		double val = 0; // set sample

		for (auto j = 1; j * freq <= 22000 && j < 9; j++) // less the frequency
		{
			int sec_amp_loc = int(j - 1);
			
			val = val + (amplitude * amplitude2[sec_amp_loc] * (sin(j * sinangle))); // as long as the frequency is get add up the sample vlaue

		}

		sinangle += (2 * PI * (sin(vibangle2)  * FreqVibrato  + freq )) / GetSampleRate(); // get the sin angle (given in step 4/5)
		
		vibangle2 += (2 * PI * CycleVib) / GetSampleRate(); // get the vib angle angle (given in step 4/5)



		audio[i] = Overflow(val); // grab the overflow
	}
}