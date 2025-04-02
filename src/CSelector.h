#ifndef CSELECTOR_H
#define CSELECTOR_H

#include <SDL3/SDL.h>
#include "CWorldPart.h"

class CSelector
{
	private:
		CWorldPart *Part;
		int Selection;
	public:
		CSelector();
		void MoveUp();
		void MoveDown();
		void MoveLeft();
		void MoveRight();
		void IncSelection();
		void DecSelection();
	void Draw();
		int GetSelection();
		int GetPlayFieldX();
		int GetPlayFieldY();
		~CSelector();
};


#endif