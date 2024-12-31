#include "pch.h"

#include "CubeApp.h"

// LinkError Solution
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")		

using namespace std;

int main ()
{
	pt::CubeApp cubeApp;

	if ( !cubeApp.Initialize () ) {
		cout << "cubeApp::Initilaize() failed" << endl;
		return -1;
	}

	return cubeApp.Run ();
}