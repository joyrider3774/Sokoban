#include "CSelector.h"
#include "CWorldPart.h"

CSelector::CSelector()
{
	Selection = IDEmpty;
	Part = new CEmpty(9,7);
	Part->Selected = true;
}

void CSelector::MoveUp()
{
	Part->SetPosition(Part->GetPlayFieldX(),Part->GetPlayFieldY()-1);
}

void CSelector::MoveDown()
{
	Part->SetPosition(Part->GetPlayFieldX(),Part->GetPlayFieldY()+1);
}

void CSelector::MoveRight()
{
	Part->SetPosition(Part->GetPlayFieldX() +1,Part->GetPlayFieldY());
}

void CSelector::MoveLeft()
{
	Part->SetPosition(Part->GetPlayFieldX() -1,Part->GetPlayFieldY());
}

void CSelector::IncSelection()
{
	int X,Y;
	Selection++;
	if (Selection > 5)
		Selection = 1;
	X = Part->GetPlayFieldX();
	Y = Part->GetPlayFieldY();
	delete Part;
	switch (Selection)
	{
		case IDEmpty:
			Part = new CEmpty(X,Y);
			break;
		case IDBox:
			Part = new CBox(X,Y);
			break;
		case IDPlayer:
			Part = new CPlayer(X,Y);
			break;
		case IDSpot:
			Part = new CSpot(X,Y);
			break;
		case IDWall:
			Part = new CWall(X,Y);
			break;
		case IDFloor:
			Part = new CFloor(X,Y);
			break;
	}
	Part->Selected = true;
}

void CSelector::DecSelection()
{
	int X,Y;
	Selection--;
	if (Selection < 1)
		Selection = 5;
	X = Part->GetPlayFieldX();
	Y = Part->GetPlayFieldY();
	delete Part;
	switch (Selection)
	{
		case IDEmpty:
			Part = new CEmpty(X,Y);
			break;
		case IDBox:
			Part = new CBox(X,Y);
			break;
		case IDPlayer:
			Part = new CPlayer(X,Y);
			break;
		case IDSpot:
			Part = new CSpot(X,Y);
			break;
		case IDWall:
			Part = new CWall(X,Y);
			break;
		case IDFloor:
			Part = new CFloor(X,Y);
			break;
	}
	Part->Selected = true;
}

void CSelector::Draw(SDL_Surface *Surface)
{
	Part->Draw(Surface);
}

int CSelector::GetSelection()
{
	return Selection;
}

int CSelector::GetPlayFieldX()
{
	return Part->GetPlayFieldX();
}

int CSelector::GetPlayFieldY()
{
	return Part->GetPlayFieldY();
}

CSelector::~CSelector()
{
	delete Part;
}
