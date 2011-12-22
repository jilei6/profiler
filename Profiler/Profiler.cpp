#include "stdafx.h"
#include "Profiler.h"
#include <fstream>
using namespace std;

static map<int, StopWatch*> StopWatches;


int Profiler::AcquireWatch( string name, string file /*= L""*/, int line)
{
	int count = (int)StopWatches.size();

	StopWatches[count] = new StopWatch(name, file, line, false);

	return count;
}

StopWatch* Profiler::QueryWatch( int count )
{
	map<int, StopWatch*>::iterator it = StopWatches.find(count);
	if(it != StopWatches.end())
		return it->second;

	return NULL;
}

void Profiler::ReportResult(string title, string filePath)
{
	fstream outfile;
	outfile.open(filePath.c_str(), ios_base::app);
	
	outfile << "<PerformanceCounters title =" << title << ">" << endl;

	::OutputDebugStringA("\n------------------");
	::OutputDebugStringA(title.c_str());

	for(map<int, StopWatch*>::iterator it = StopWatches.begin(); it != StopWatches.end(); ++it)
	{
		it->second->Report(outfile);
	}
	::OutputDebugStringA("------------------\n");

	outfile << "</PerformanceCounters>" << endl;

	outfile.close();
}

void StopWatchHelper::Stop()
{
	StopWatch* sw = Profiler::QueryWatch(mID);
	if(sw != NULL)
		sw->Stop();
}

StopWatchHelper::StopWatchHelper( int id )
{
	StopWatch* sw = Profiler::QueryWatch(id);
	if(sw != NULL)
		sw->Start();

	mID = id;

	mStopped = false;
}

StopWatchHelper::~StopWatchHelper()
{
	if(mStopped)
		return ;

	Stop();
}

StopWatch::StopWatch( string reportName, string file, int line, bool start /*= true*/ )
	: mReportName(reportName),
	  mFile(file),
	  mLine(line)
{
	mTicks = 0;
	mHitCount = 0;

	if(start)
		Start();
}

StopWatch::~StopWatch()
{
	Stop();
}

void StopWatch::Stop()
{
	if(mIsStopped)
		return;

	DWORD endTick = ::GetTickCount();

	mTicks += (endTick - mStartTick);

	mIsStopped = true;
}

void StopWatch::Start()
{
	mHitCount ++;
	mStartTick = ::GetTickCount();
	mIsStopped = false;
}

void StopWatch::Report( fstream& out, bool bReset /*= false*/ )
{
	char sValue[512];
	sprintf_s(sValue, 512, "%s: %d Hit Count: %d", mReportName.c_str(),  mTicks, mHitCount);

	::OutputDebugStringA(sValue);

	out << "	<Name>" << mReportName << "</Name>" << endl;
	out << "	<Ticks>" << mTicks << "</Ticks>" << endl;
	out << "	<HitCount>" << mHitCount << "</HitCount>" << endl;
	if(!mFile.empty())
	{
		out << "	<File>" << mFile << "</File>" << endl;
		out << "	<Line>" << mLine << "</Line>" << endl;
	}

	if(bReset)
	{
		mTicks = 0;
		mHitCount = 0;
	}
}