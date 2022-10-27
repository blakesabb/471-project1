#include "stdafx.h"
#include "Notes.h"
#include <map>
#include <string>

using namespace std;

static int g_initialized = false;
static map<wstring, double> g_name2freq;

double NoteToFrequency(const WCHAR *name)
{
    if(!g_initialized)
    {
        for(int i=0;  i<sizeof(notes) / sizeof(Notes);  i++)
        {
            g_name2freq[notes[i].name] = notes[i].freq;
        }

        g_initialized = true;
    }

    map<wstring, double>::iterator f = g_name2freq.find(name);
    if(f == g_name2freq.end())
        return 0;

    return f->second;
}