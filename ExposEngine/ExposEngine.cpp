// ExposEngine.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h" 
#include "helper.h"
#include "game.h"
#include "packfile.h"
 

int main() {
	for (int i = 0; i < 0; i++) {
		expos::Packfile packfile;

		packfile.addFile("sound.wav", "data/sounds/boomalter3.wav");
		packfile.addFile("sound.wav", "data/sounds/boomalter.wav");
		packfile.addFile("sound.wav", "boomalter2.wav");
		packfile.addFolder("../data/fonts/", "data/fonts/");

		packfile.write("packfile.pack");
		packfile.extract("packfile/");
	}

	/*ContentFile test("#thisname { peter: 35; name: \"Nils-Torben Pott\"; kinder: [{ name: \"anja\"; alter: 25;}, {name: \"peter\"; alter: 37;}]; anschrift: {stadt: \"Bad Laasphe\"; hausnummer: 8;}; }");
	test.print(std::cout);
	*/
	expos::Game *g = new expos::Game();
	
	g->run();

	delete g;

    return 0;
}
