/*
 *  Name :         wave.cxx
 *  Description :  Simple WAVE file read/write class library.
 *  Version :      1.00  9-06-99 CBO WAVE version from AIFF version.
 *  Author :       Charles B. Owen
 */

#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <sstream>

#include "wave.h"

using namespace std;

// Simple inline function to put a four character code into 
// a field.
inline void IDPlace(char *p_to, const char *p_fm)
{
	for(int i=0;  i<4;  i++)
		p_to[i] = p_fm[i];
}

// **********************************************************************
// 
// CWaveIn Input object
// 
// **********************************************************************



/*
 *  Name :         CWaveIn::CWaveIn()
 *  Description :  Constructors.  We can construct with a filename or
 *                 without.
 */

CWaveIn::CWaveIn() : CWave(), std::ifstream()
{
}


CWaveIn::CWaveIn(const LPCTSTR fname) : CWave(), ifstream(fname, ios::binary)
{
   if(bad() || !good())
   {
      _Error(TEXT("Unable to open file "), fname, TEXT(" for reading."));
      return;
   }

   // The stream has been opened okay, open the Wave file
   if(!_open())
      setf(ios::badbit | ios::failbit);
}


CWaveIn::~CWaveIn()
{
}


/*
 *  Name :         CWaveIn::open()
 *  Description :  Open a file for reading.
 */

bool
CWaveIn::open(const LPCTSTR fname)
{
   ifstream::open(fname, ios::binary);

   if(bad() || !good())
   {
      _Error(TEXT("Unable to open file "), fname, TEXT(" for reading."));
      return false;
   }

   // The stream has been opened okay, open the Wave file
   if(!_open())
      setf(ios::badbit | ios::failbit);

   return true;
}


/*
 *  Name :         CWaveIn::_open()
 *  Description :  This is the private part of the open process after
 *                 the file is opened.  This reads the Wave headers and
 *                 prepares for sample reading.
 */
int CWaveIn::_open()
{
	soundStart = 0;

	// Read the RIFF/WAVE chunk.
	Chunk RiffChunk;
	if(!ReadChunk(RiffChunk))
	{
		Error(TEXT("File is not a valid Wave file"));
		return 0;
	}

	// Ensure this is actually a RIFF chunk.
	if (strncmp(RiffChunk.ckID, "RIFF", 4) != 0)
	{
		Error(TEXT("File is not a valid Wave file"));
		return 0;
	}

	if(strncmp(RiffChunk.formType, "WAVE", 4) != 0)
	{
		Error(TEXT("File is not a valid Wave file"));
		return 0;
	}

	// Read the chunks.  Chunks need not be in any particular order.

	ChunkHeader Header;
	while (ReadChunkHeader(Header))
	{
		// Offset to point after the current chunk
		int seek_offset = (int)tellg() + Header.ckSize;

		if (strncmp(Header.ckID, "fmt ", 4) == 0) 
		{
			/*
			* read fmt chunk
			*/

			int type;
			ReadSHORT(type);   
			if(type != 1)
			{
				Error(TEXT("Only PCM WAVE files are supported"));
				return 0;
			}

			ReadSHORT(numChannels);
			unsigned long lSampleRate;
			ReadULONG(lSampleRate);
			sampleRate = lSampleRate;

			unsigned long dummy;
			ReadULONG(dummy);	// Actually number of bytes per second

			int dummy2;
			ReadSHORT(dummy2);	// Bytes per frame (as if we care)

			ReadSHORT(sampleSize);	// This we might need
		} 
		else if (strncmp(Header.ckID, "data", 4) == 0) 
		{
			/*
			* record position of sound data
			*/

			soundStart =  (unsigned int)tellg();
			numSampleFrames = Header.ckSize / 
				(numChannels * ((sampleSize + 7) / 8));
		} 

		// Seek to after the current chunk
		seekg(seek_offset);
		if (fail())
		{
			Error(TEXT("Error reading Wave file."));
			return 0;
		}
	}

	if(soundStart == 0)
	{
		Error(TEXT("Unable to find sound data in Wave file."));
		return 0;
	}

	// Prepare for reading the sound data.
	clear();
	seekg(soundStart);
	curFrame = 0;
	if (fail())
	{
		Error(TEXT("Error after seek to sound data."));
		return 0;
	}

	return 1;
}


/*
 *  Name :         CWaveIn::ReadFrame()
 *  Description :  Read a frame of audio.
 */

int
CWaveIn::ReadFrame(short *frame)
{
   if(sampleSize == 16)
   {
      signed char b[2];

      for(int c=0;  c<numChannels;  c++)
      {
			read((char *)b, 2);
			frame[c] = (int(b[1]) << 8) | (unsigned char)(b[0]);
      }
   }
   else
   {
      for(int c=0;  c<numChannels;  c++)
      {
			signed char ch;
			read((char *)&ch, 1);
			frame[c] = ch;
      }

   }

   curFrame++;
   return !fail();
}


/*
 *  Name :         CWaveIn::SeekFrame()
 *  Description :  Set the file position at a particular location in the file.
 */

int
CWaveIn::SeekFrame(int frame)
{
   clear();
   curFrame = frame;
   seekg(soundStart + frame * numChannels * (sampleSize + 7) / 8);
   return !fail();
}


void CWaveIn::Rewind()
{
   SeekFrame(0);
}



/*
 *  Name :         CWaveIn::ReadChunk()
 *  Description :  This reads an object of type Chunk from disk,
 *                 for any type of machine.
 */

int
CWaveIn::ReadChunk(Chunk &chunk)
{
   ReadID(chunk.ckID);
   ReadLONG(chunk.ckSize);
   ReadID(chunk.formType);

   return !fail();
}


/*
 *  Name :         CWaveIn::ReadChunkHeader()
 *  Description :  This reads an object of type ChunkHeader from disk,
 *                 for any type of machine.
 */

int
CWaveIn::ReadChunkHeader(ChunkHeader &chunk)
{
   ReadID(chunk.ckID);
   ReadLONG(chunk.ckSize);

   return !fail();
}

/*
 *  Name :         CWaveIn::ReadID()
 *  Description :  Reads an Wave file object of type ID.
 */

int
CWaveIn::ReadID(ID id)
{
   read(id, 4);
   return 1;
}


/*
 *  Name :         CWaveIn::ReadLONG()
 *  Description :  Reads an Wave file object of type LONG.
 */

int
CWaveIn::ReadLONG(long &item)
{
   int i;
   unsigned char b[4];

   read((char *)b, 4);

   item = 0;

   for(i=0;  i<4;  i++)
   {
      item <<= 8;
      item |= b[3 - i];
   }

   return 1;
}


/*
 *  Name :         CWaveIn::ReadULONG()
 *  Description :  Reads an Wave file object of type LONG.
 */

int
CWaveIn::ReadULONG(unsigned long &item)
{
   int i;
   unsigned char b[4];

   read((char *)b, 4);
   item = 0;

   for(i=0;  i<4;  i++)
   {
      item <<= 8;
      item |= b[3 - i];
   }

   return 1;
}


/*
 *  Name :         CWaveIn::ReadSHORT()
 *  Description :  Reads an Wave/AIFC file object of type SHORT
 */

int
CWaveIn::ReadSHORT(int &item)
{
   unsigned char b[2];

   read((char *)b, 2);
   item = (b[1] << 8) | b[0];
   return 1;
}


// **********************************************************************
// 
// WaveOut output object
// 
// **********************************************************************


/*
 *  Name :         CWaveOut::CWaveOut()
 *  Description :  Constructors.  We can construct with a filename or
 *                 without.
 */

CWaveOut::CWaveOut() : CWave(), ofstream()
{
   _default();
}


CWaveOut::CWaveOut(const LPCTSTR fname) : CWave()
{
   _default();

   ofstream::open(fname, ios::binary | ios::out);

   if(bad() || !good())
   {
      _Error(TEXT("Unable to open file "), fname, TEXT(" for writing."));
      return;
   }

   // The stream has been opened okay, open the Wave file
   if(!_open())
      setf(ios::badbit | ios::failbit);
}


CWaveOut::~CWaveOut()
{
   close();
}


/*
 *  Name :         CWaveOut::open()
 *  Description :  Open the file for writing
 */

void
CWaveOut::open(const LPCTSTR fname)
{
   ofstream::clear();
   ofstream::open(fname, ios::binary | ios::out);

   if(bad() || !good())
   {
      _Error(TEXT("Unable to open file "), fname, TEXT(" for writing."));
      return;
   }

   // The stream has been opened okay, open the Wave file
   if(!_open())
      setf(ios::badbit | ios::failbit);
}


/*
 *  Name :         CWaveOut::_default()
 *  Description :  Set all file parameters to default values.
 */

void
CWaveOut::_default()
{
   isopen = 0;
   isstarted = 0;
   numChannels = 1;
   sampleSize = 16;
   sampleRate = 44100.;
}


/*
 *  Name :         CWaveOut::_open()
 *  Description :  This is the private part of the open process after
 *                 the file is opened.  This reads the Wave headers and
 *                 prepares for sample reading.
 */

int
CWaveOut::_open()
{

   isopen = 1;
   isstarted = 0;
   return 1;
}


/*
 *  Name :         CWaveOut::_headers()
 *  Description :  This routine writes the file headers.  It is called
 *                 the first time we write stuff into the file.  Only 
 *                 call this once.
 */

int
CWaveOut::_headers()
{
   if(isstarted)
      return 0;

   isstarted = 1;

   // Write the file header
   Chunk form;
   IDPlace(form.ckID, "RIFF");
   IDPlace(form.formType, "WAVE");
   form.ckSize = 0;      // Have to rewrite later
   WriteChunk(form);

   // Write the fmt  header
   ChunkHeader fmt;
   IDPlace(fmt.ckID, "fmt ");
   fmt.ckSize = 16;
   WriteChunkHeader(fmt);
   if(fail() || bad())
   {
      Error(TEXT("Failure writing Wave file"));
      return 0;
   }

   int bytesper = (sampleSize + 7) / 8;

   WriteSHORT(1);		// Only PCM is supported
   WriteSHORT(numChannels);
   WriteULONG((unsigned long)(sampleRate));
   WriteULONG((unsigned long)(sampleRate) * numChannels * bytesper);
   WriteSHORT(numChannels * bytesper);
   WriteSHORT(sampleSize);
   
   m_lenLoc = (int)tellp();		// Save off location for data length

   ChunkHeader data;
   IDPlace(data.ckID, "data");
   data.ckSize = 0;	   	// Have to fill in later
   WriteChunkHeader(data);
   
   // We are ready to write audio at this point (we write the other stuff last)
   numSampleFrames = 0;

   if (fail())
   {
      Error(TEXT("Error writing sound file header."));
      return 0;
   }

   return 1;
}



/*
 *  Name :         CWaveOut::close()
 *  Description :  Close the Wave output file.
 */

void CWaveOut::close()
{
   if(!isopen)
      return;

   // This allows for a zero length file...
   if(!isstarted)
      _headers();

   isopen = 0;

   // How long is the file?
   unsigned long flen = (int)tellp();

   // Write in the sound length
   seekp(m_lenLoc + 4);
   WriteULONG(flen - m_lenLoc - 8);

   // Write in the entire file length
   seekp(4l);
   WriteULONG(flen - 8);

   if(fail() || bad())
   {
      Error(TEXT("Failure writing Wave file"));
      return;
   }

   ofstream::close();
}



/*
 *  Name :         CWaveOut::WriteFrame()
 *  Description :  Write a frame of audio.
 */

int
CWaveOut::WriteFrame(short *frame)
{
   // Since we may set file parameters after the file is opened,
   // we need to defer writing the file headers until we are actually
   // writing data.

   if(!isstarted)
      _headers();

   signed char b[2];

   if(sampleSize == 16)
   {
      for(int c=0;  c<numChannels;  c++)
      {
			 b[1] = (signed char)(frame[c] >> 8);
			 b[0] = (signed char)(frame[c]);
			 write((char *)b, 2);
      }
   }
   else
   {
      for(int c=0;  c<numChannels;  c++)
      {
			 b[0] = (signed char)(frame[c]);
			 write((char *)b, 1);
      }

   }

   numSampleFrames++;
   return !fail();
}



/*
 *  Name :         CWaveOut::WriteChunk()
 *  Description :  This writes an object of type Chunk to disk,
 *                 for any type of machine.
 */

int
CWaveOut::WriteChunk(const Chunk &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);
   WriteID(chunk.formType);

   return !fail();
}


/*
 *  Name :         CWaveOut::WriteChunkHeader()
 *  Description :  This writes an object of type ChunkHeader to disk,
 *                 for any type of machine.
 */

int
CWaveOut::WriteChunkHeader(const ChunkHeader &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);
   return !fail();
}

/*
 *  Name :         CWaveOut::WriteID()
 *  Description :  Writes an Wave file object of type ID.
 */

int
CWaveOut::WriteID(const ID id)
{
   write(id, 4);
   return 1;
}


/*
 *  Name :         CWaveOut::WriteLONG()
 *  Description :  Writes an Wave file object of type LONG.
 */

int
CWaveOut::WriteLONG(long item)
{
   int i;
   unsigned char b[4];

   for(i=0;  i<4;  i++)
   {
      b[i] = (unsigned char)(item & 0xff);
      item >>= 8;
   }

   write((char *)b, 4);

   return 1;
}


/*
 *  Name :         CWaveOut::WriteULONG()
 *  Description :  Writes an Wave file object of type LONG.
 */

int
CWaveOut::WriteULONG(unsigned long item)
{
   int i;
   unsigned char b[4];

   for(i=0;  i<4;  i++)
   {
      b[i] = (unsigned char)(item & 0xff);
      item >>= 8;
   }

   write((char *)b, 4);

   return 1;
}


/*
 *  Name :         CWaveOut::WriteSHORT()
 *  Description :  Writes an Wave file object of type SHORT
 */

int
CWaveOut::WriteSHORT(int item)
{
   unsigned char b[2];

   b[1] = item >> 8;
   b[0] = item & 0xff;
   write((char *)b, 2);
   return 1;
}



// **********************************************************************
// 
// class Wave - Generic Wave functions
// 
// **********************************************************************


/*! Generic error reporting routine
 *
 * Function is virtual and can be overridden for
 * more user control of error reporting.
 * \param str The string error message to print
 */
void CWave::Error(LPCTSTR str)
{
   AfxMessageBox(str, MB_OK | MB_ICONEXCLAMATION);
}


/*
 *  Name :         CWave::_Error()
 *  Description :  Utility version of Error that accepts additional strings.  
 */

void CWave::_Error(const LPCTSTR str1, const LPCTSTR str2, const LPCTSTR str3)
{
	std::basic_ostringstream<TCHAR> ostr;
	ostr << str1;
	if(str2)
	  ostr << str2;

	if(str3)
	  ostr << str3;

	ostr << ends;

	Error(ostr.str().c_str());
}



