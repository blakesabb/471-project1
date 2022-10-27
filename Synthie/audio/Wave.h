/*
 *  Name :         wave.h
 *  Description :  Simple Wave file read/write class library.
 *  Version :      See wave.cxx
 */

#ifndef _WAVE_H
#define _WAVE_H

#include <fstream>

/*! Abstract base class for wave file handling
 *
 * This class provides a place for common functionality
 * between the read and write operations 
 */
class CWave
{
public:
	//! Destructor
	virtual ~CWave() {}

protected:
	virtual void Error(const LPCTSTR);

	//! Protected constructor - abstract base class
	CWave() {}

	typedef char  ID[4];

	struct  Chunk {
		ID      ckID;
		long    ckSize;
		ID      formType;
	};

	struct  ChunkHeader {
		ID      ckID;
		long    ckSize;
	};

	void _Error(const LPCTSTR s1, const LPCTSTR s2=NULL, const LPCTSTR s3=NULL);

private:
	// Disable the copy constructor and assignment operators
	CWave(const CWave &);
	CWave &operator=(const CWave &);
};

/*! WAVE audio file input class
 *
 * Supports input of audio from .wav format files.
 */
class CWaveIn : public CWave, private std::ifstream
{
public:
	CWaveIn(const LPCTSTR);
	CWaveIn();
	virtual ~CWaveIn();

	bool open(const LPCTSTR);

	void Rewind();
	int ReadFrame(short *);
	int SeekFrame(int frame);

	int CurFrame() const {return curFrame;}
	int NumChannels() const {return numChannels;}
	int NumSampleFrames() const {return numSampleFrames;}
	int SampleSize() const {return sampleSize;}
	double SampleRate() const {return sampleRate;}

private:
	int _open();

	int ReadChunk(Chunk &chunk);
	int ReadID(ID id);
	int ReadLONG(long &item);
	int ReadULONG(unsigned long &item);
	int ReadSHORT(int &item);
	int ReadChunkHeader(ChunkHeader &chunk);

	unsigned int soundStart;		// Offset to beginning of sound data
	int curFrame;		// Current frame we are reading
	int numChannels;		// Number of audio channels
	unsigned long numSampleFrames;	// Total sample frames
	int sampleSize;		// Sample size in bits
	double sampleRate;		// Samples per second
};

/*! Wave audio output class
 *
 * Allows for writing .wav files
 */
class CWaveOut : public CWave, private std::ofstream
{
public:
   CWaveOut(const LPCTSTR);
   CWaveOut();
   virtual ~CWaveOut();

   void open(const LPCTSTR);
   void close();
   bool fail() {return std::ofstream::fail();}

   int WriteFrame(short *);

   void NumChannels(int n) {numChannels = n;}
   void SampleSize(int s) {sampleSize = s;}
   void SampleRate(double d) {sampleRate = d;}

private:
   void _default();
   int _open();
   int _headers();

   int WriteChunk(const Chunk &chunk);
   int WriteChunkHeader(const ChunkHeader &chunk);
   int WriteID(const ID id);
   int WriteLONG(long item);
   int WriteULONG(unsigned long item);
   int WriteSHORT(int item);
   
   unsigned long m_lenLoc;	// Location in file to write length
   int isopen;
   unsigned long numSampleFrames;
   int numChannels;		// Number of audio channels
   int sampleSize;		// Sample size in bits
   double sampleRate;		// Samples per second

   int isstarted;	       	// For delayed writing of fmt chunk
};

#endif
