#ifndef DCCSETTINGS_HPP
#define DCCSETTINGS_HPP


class DCCSettings
{

public:
	DCCSettings()
	{
		tripCurrent = 0;
		toff = 0;
		slewRate = 0;
		track.fault = false;
		track.overTemp = Good;
		track.overCurrent = Good;
		track.openLoad = false;
		track.current = 0;
		prog.fault = false;
		prog.overTemp = Good;
		prog.overCurrent = Good;
		prog.openLoad = false;
		prog.current = 0;
	}

	enum Status
	{
		Good = 0,
		Warning = 1,
		Fault = 2
	};
	
	struct HBStatus 
	{
		bool fault;
		Status overTemp;
		Status overCurrent;
		bool openLoad;
		int current;
	};
		
protected:
	int tripCurrent;
	int toff;
	int slewRate;
	HBStatus track;
	HBStatus prog;
};

#endif 
