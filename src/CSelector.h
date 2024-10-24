#ifndef CSELECTOR_H
#define CSELECTOR_H

#include <SDL.h>
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
		void Draw(SDL_Surface *Surface);
		int GetSelection();
		int GetPlayFieldX();
		int GetPlayFieldY();
		~CSelector();
};


#endif