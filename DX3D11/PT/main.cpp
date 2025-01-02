#include "pch.h"

#include "CubeApp.h"
#include "SquareApp.h"

// LinkError Solution
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")		

using namespace std;

int main ()
{
	//pt::CubeApp cubeApp;
	pt::Square squareApp;

	/*if ( !cubeApp.Initialize () ) {
		cout << "cubeApp::Initilaize() failed" << endl;
		return -1;
	}*/

	if ( !squareApp.Initialize () ) {
		cout << "SquareApp::Initialize() failed" << endl;
		return -1;
	}

	return squareApp.Run ();
}