#ifndef CHISTORY_H
#define CHISTORY_H

#include "Defines.h"

class CWorldPart;

struct SPrevPoint
{
	int X,Y;
};



class CHistory
{
	private:
		SPrevPoint Items[MaxHistory];
		CWorldPart *Part;
		int ItemCount;
	
	public:
		CHistory(CWorldPart *Partin);
		void Add (int X, int Y);
		void GoBack();
};

#endif