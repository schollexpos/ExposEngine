// ExposEngine.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h" 
#include "helper.h"
#include "game.h"
#include "packfile.h"
 

int main() 
{
	int a = 15;

	char b = 'A';
	float c = 17.6;

	void *test[5];
	test[0] = (void*)&a;
	test[1] = (void*)&b;
	test[2] = (void*)&c;

	void *pointer = (void*)test;



	void **test2 = (void**) pointer;

	int *a2 = ((int*) test2[0]);
	char *b2 = ((char*)test2[1]);

	std::cout << *a2 << std::endl << *b2 << std::endl;


	for (int i = 0; i < 0; i++) {
		expos::Packfile packfile;

		packfile.addFile("sound.wav", "boomalter3.wav");
		packfile.addFile("Don_Paolo.mid", "DONPAOLO.MID");
		packfile.addFile("michal.png", "michol.png");
		packfile.addFile("sound.wav", "boomalter.wav");
		packfile.addFile("sound.wav", "boomalter2.wav");

		packfile.write("packfile.pack");
		packfile.extract("packfile/");
	}
	

	expos::Game *g = new expos::Game();
	
	g->run();

	delete g;

    return 0;
}
