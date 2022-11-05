#include "stdafx.h"
#include "SynthesizerAdd.h"
#include "Notes.h"
#include <string>
#include <iostream>
#include <sstream>

CSynthesizerAdd::CSynthesizerAdd(void)
{
	// set values 
	attack = 0;

	decay = 0;

	sustain = 1.0;

	release = 0;

	CrossIn = 0;

	CrossOut = 0;

}


CSynthesizerAdd::~CSynthesizerAdd(void)
{
	
}

void CSynthesizerAdd::Start()
{
	sinewave.SetDuration(duration); // set duration

	sinewave.SetSampleRate(GetSampleRate()); //set sample rate

	sinewave.Start(); // start

	time = 0; // init time
}



bool CSynthesizerAdd::Generate()
{
	sinewave.Generate(); // generate

	m_frame[0] = sinewave.Frame(0); // set frame 0

	m_frame[1] = sinewave.Frame(1); // set frame 1

	double mutliplier = 1; // start mult

	double constant = 0; // start neg constant 
	
	/*
	if (sustain > 1.5)  // if susatin is less than 1.5 make the constant 1
	{
		constant = 1;
	}
		

	if (time < attack) // if time less than attack
	{
		mutliplier = time / attack; // multiplyer is  time / attack
	}
	
	else if (time < decay) // if time is less than decay
	{
		mutliplier = constant * ((1 - sustain) / (decay - attack)) * (time) +  (1 - constant * ((1 - sustain) / (decay - attack)) * (attack)); // use decay equation we are given
	}

	else if (time > (duration - release) && release != 0) // make sure release is not 0, then subtract duration by time. If this is the case 
	{
		if (sustain <= 0) // mkae sure statin is not negative or zero (that does not mkae much sense)
		{
			sustain = 1; // set sustain
		}
		
		mutliplier = time * - sustain / release + (sustain/release)*(duration); // set the new multilyer 
	}
	
	else 
	{
		mutliplier = sustain; // if none of that is ture set the new multiplyer by sustain
	}
	*/
	m_frame[0] = mutliplier * m_frame[0]; // set frame 0 by multiplyer by multiplying (get it) to the current frame 0
	
	m_frame[1] =  mutliplier * m_frame[1]; // set frame 1 by multiplyer by multiplying (get it) to the current frame 1

	time = time +  GetSamplePeriod(); // set new nime by the sample period

	return time < duration; // we only need time less than duration
}


void CSynthesizerAdd::SetNote(CNote* note)
{
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	note->Node()->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	// Loop over the list of attributes
	for (int i = 0; i < len; i++)
	{
		// Get attribute i
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		// Get the name of the attribute
		CComBSTR name;
		attrib->get_nodeName(&name);


		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == "duration")
		{
			
			value.ChangeType(VT_R8);
			
			SetDuration(value.dblVal);
		
		}
		else if (name == "note")
		{
			SetFreq(NoteToFrequency(value.bstrVal));
		}
		
		 //ALL BASED OFF OF STEP 5
		else if (name == "amplitudes") // meed nultiple amplitudes for vibrato 
		{
			double sound[8] = { 0 }; //agrragte the sound to 0
			
			
			// set stings
			// -------------
			std::wstring wide(value.bstrVal);
			
			std::string str(wide.begin(), wide.end());
			
			std::stringstream ss(str);
			
			std::string item;
			// ---------------

			int place = 0;
			
			while (std::getline(ss, item, char(32))) 
			{
				sound[place++] = atof(item.c_str());
			}

			SetAmplitude(sound[0]);
		}
		
		
		
		else if (name == "vibrato")
		{

			// defined in Additive.cpp

			// set stings
			// -------------
			std::wstring wide(value.bstrVal);

			std::string str(wide.begin(), wide.end());

			std::stringstream ss(str);

			std::string item;
			// -------------

			
			
			std::getline(ss, item, char(32)); // get lines

			sinewave.CycleVibrato(atof(item.c_str())); // set rate

			std::getline(ss, item, char(32)); // get lines

			sinewave.FrequencyVibrato(atof(item.c_str())); // set freqwency
		}
		

		else if (name == "ADSR") {
			// set stings
			// -------------
			std::wstring wide(value.bstrVal);
			
			std::string str(wide.begin(), wide.end());

			std::stringstream ss(str);
			
			std::string item;
			// ---------------


			// regular
			std::getline(ss, item, char(32));
			
			attack = atof(item.c_str()) * duration;

			
			// decay
			std::getline(ss, item, char(32));
			
			decay = attack + atof(item.c_str()) * duration;

			
			// sustain
			std::getline(ss, item, char(32));
			
			sustain = atof(item.c_str());


			// release
			std::getline(ss, item, char(32));
			
			release = atof(item.c_str()) * duration;
		}
		
		else if (name == "Fadein") // cross fade in 
		{
			value.ChangeType(VT_R8); // change type 

			SetCrossFadeIn(value.dblVal * duration); // vade in on vaule by duration
		}
		
		else if (name == "Fadeout") // cross out
		{
			value.ChangeType(VT_R8); // change type 

			SetCrossFadeOut(value.dblVal * duration); // fade out on value by duration
		}
		

	}

}