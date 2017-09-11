// Vs13cpp-Schiffe-Versenken-Projekt.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
// ----------------------------------------------------------------------------------------------
// Content:		Schiffe-Versenken-Projekt
// Autor:		Recep Karakoc und David Leinert
// Date:		2017.07.14 - 2017.07.30
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// includes.
// ----------------------------------------------------------------------------------------------

#include "stdafx.h" // vorkompilierter header, ist eine visualstudio seitige datei.
#include <string> // n�tig f�r die schl�sselw�rter string.
#include <time.h> // n�tig f�r die inizialisierung des zufallsgenerators.
#include <windows.h> // n�tig f�r die einstellung der fenstergr��e sowie der cursor positionierung.

// ----------------------------------------------------------------------------------------------
// global variables.
// ----------------------------------------------------------------------------------------------

// globaler namensraum, um den �bergeordneter pfadname einer objektinstanziierung aus der string bibliothek zu referenzieren.
// ben�tigt f�r die freischaltung der sonderzeichen f�r die deutsche sprache, sowie bei der verwendung der schl�sselw�rter string.
using namespace std;

// globale quadratzahl, welche die schlachtfeldgr��e beschreibt.
const unsigned short int GRID_SQUARE_SIZE = 10;

unsigned char combatUnitTypes[4] = { '4', '3', '2', '1' }; // globale schiffstypen.
unsigned short int combatUnitTypeOccurrences[4] = { 1, 2, 3, 4 }; // globale vorkommen der schiffstypen.

unsigned char abscissa[GRID_SQUARE_SIZE] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' }; // die globalen x_achsen bezeichner des schlachtfeldes.
unsigned char ordinate[GRID_SQUARE_SIZE] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '10' }; // die globalen y_achsen bezeichner des schlachtfeldes.

unsigned char gridHitMask[GRID_SQUARE_SIZE][GRID_SQUARE_SIZE]; // globale treffermaske.
unsigned char grid[GRID_SQUARE_SIZE][GRID_SQUARE_SIZE]; // globales schlachtfeld.

unsigned short int successHitCounters[4]; // globaler z�hler der erfolgreichen treffer.
unsigned short int assaultCounter; // globaler z�hler aller durchgef�hrten agriffe.

unsigned short int cellpointer[2]; // globaler zellenzeiger, also ein koordinatenhalter f�r eine schlachtfeldkoordinate.

// ----------------------------------------------------------------------------------------------
// functions.
// ----------------------------------------------------------------------------------------------

// funktion: schlachtfeld bzw. alle zellen und z�hlervariablen leeren. 
void reset(){

	// schleifendurchlauf, um zeilenweise alle zeilen der treffermaske und dem schlachtfeld f�r das leeren zu durchlaufen.
	for (unsigned short int i = 0; i < GRID_SQUARE_SIZE; i++){

		// schleifendurchlauf, um alle spalten der zeile f�r das leeren zu durchlaufen.
		for (unsigned short int j = 0; j < GRID_SQUARE_SIZE; j++){

			gridHitMask[i][j] = ' '; // leere eine zelle der globale treffermaske, bzw. f�lle diese mit einem leerzeichen.
			grid[i][j] = 'e'; // leere eine zelle des globale schlachtfeld, bzw. f�lle diese mit einem e-zeichen, welches auf emptyness also inhaltslosigkeit hinweisen soll.
		}
	}

	// leere den globalen z�hler f�r die erfolgreichen treffer.
	successHitCounters[0] = 0;
	successHitCounters[1] = 0;
	successHitCounters[2] = 0;
	successHitCounters[3] = 0;

	// leere den globalen z�hler aller durchgef�hrten agriffe.
	assaultCounter = 0;

	// leere den globalen zellenzeiger.
	cellpointer[0] = 0;
	cellpointer[1] = 0;
}

// funktion: halte die fenstergr��e auf grundeinstellung, 400 x 522.
void adjustConsoleWindow(){

	RECT rect;
	GetWindowRect(GetConsoleWindow(), &rect);
	MoveWindow(GetConsoleWindow(), rect.left, rect.top, 400, 522, TRUE);
}

// funktion: bewege den kursor an eine gew�nschte position in der konsole.
void moveCursorToPosition(unsigned short int x, unsigned short int y){

	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// funktion: kehre mit dem kursor an den bereich der eingabezeile zur�ck.
// sodass der cursor keinen zeilenumbruch macht, bzw. in der konsole kein selbstst�ndiges scrolling verursacht.
void revertLineBreak(){

	moveCursorToPosition(10, 28); // funktionsaufruf: bewege den kursor an eine gew�nschte position in der konsole.
}

// funktion: zeichne eine horizontale linie.
void drawUnderline(){

	printf(" ============================================\n");
}

// funktion: zeichne die programm�berschrift mit �bergabe der gw�nschten �berschriftenerweiterung.
void drawHeader(string headerExtension){

	printf(" SCHIFFE-VERSENKEN: %s\n", headerExtension.c_str());
}

// funktion: zeichne, je nach �bergabe, die komponenten des schlachtfeldes als zeile.
// es stehen drei �bergabetypen zur auswahl:
//		- abscissa_line			= ist die zeile welche die x_achsen bezeichnung des schlachtfeldes zeichnet.
//		- separator_line		= ist die zeile welche den horizontalen gitterbegrenzer zeichnet.
//		- grid_hitmask_line		= ist die zeile welche alle spalten einer zeile, also schiffe, treffer, nichttreffer und den vertikalen gitterbegrenzer, zeichnet.
//								  bei diesem dritten �bergabetype kann zus�tzlich noch eine �bergaben gemacht werden, 
//							      n�mlich die gew�nschte zeile, ansonsten wird immer die erste zeile genommen.
void drawBattleScenarioLine(string type, unsigned short int lineCounter = 0){

	// wenn abscissa_line als �bergabetype �bergeben wurde.
	if (type == "abscissa_line"){

		// zeichne x_achsen bezeichnung des schlachtfeldes.
		printf("      A   B   C   D   E   F   G   H   I   J  \n");
	}

	// wenn separator_line als �bergabetype �bergeben wurde.
	if (type == "separator_line"){

		// zeichne horizontalen gitterbegrenzer.
		printf("    +---+---+---+---+---+---+---+---+---+---+\n");
	}

	// wenn grid_hitmask_line als �bergabetype �bergeben wurde.
	if (type == "grid_hitmask_line"){

		// zeichne die y_achsen bezeichnung, mit f�hrender null, solange der zeilenwert unter 10 ist.
		printf(" %02u", lineCounter + 1);

		// schleifendurchlauf, um alle spalten einer zeile, also schiffe, treffer, nichttreffer und den vertikalen gitterbegrenzer, zu zeichnen.
		for (unsigned short int iX = 0; iX < GRID_SQUARE_SIZE; iX++){

			// zeichne spalte der zeile von der globalen treffermaske.
			printf(" | %c", gridHitMask[lineCounter][iX]);
		}

		// mache einen zeilenumbruch.
		printf(" |\n");
	}
}

// funktion: zeichne das schlachtfeld mit den schiffe, zeilenweise.
void drawBattleScenario(){

	// funktionsaufruf: zeichne, je nach �bergabe, die komponenten des schlachtfeldes als zeile.
	// in dem falle die komponenten zeile: welche die x_achsen bezeichnung des schlachtfeldes zeichnet.
	drawBattleScenarioLine("abscissa_line");

	// funktionsaufruf: zeichne, je nach �bergabe, die komponenten des schlachtfeldes als zeile.
	// in dem falle die komponenten zeile: welche den horizontalen gitterbegrenzer zeichnet.
	drawBattleScenarioLine("separator_line");

	// schleifendurchlauf, um alle zeilen des schlachtfeldes zu zeichnen, also 10.
	for (unsigned short int iY = 0; iY < GRID_SQUARE_SIZE; iY++){

		// funktionsaufruf: zeichne, je nach �bergabe, die komponenten des schlachtfeldes als zeile.
		// in dem falle die komponenten zeile: welche alle spalten einer zeile, also schiffe, treffer, nichttreffer und den vertikalen gitterbegrenzer, zeichnet.
		// hier wird zus�tzlich die �bergabe eines z�hlers gemacht, welcher durch die dar�berliegende schleife hochgez�hlt wird,
		// um somit alle zeilen des schlachtfeldes zu zeichnen.
		drawBattleScenarioLine("grid_hitmask_line", iY);

		// funktionsaufruf: zeichne, je nach �bergabe, die komponenten des schlachtfeldes als zeile.
		// in dem falle die komponenten zeile: welche den horizontalen gitterbegrenzer zeichnet.
		drawBattleScenarioLine("separator_line");
	}
}

// funktion: zeichne die punktetafel.
void drawHud(){

	printf("    Z�ge(%03u) S1(%u/4) S2(%u/3) S3(%u/2) S4(%u/1)\n", assaultCounter, successHitCounters[3], successHitCounters[2], successHitCounters[1], successHitCounters[0]);
}

// funktion: zeichne den fu�bereich, also die letzte m�gliche zeile.
void drawFooter(){

	printf("\n\n ============================================");
}

// funktion: zeichne den aufforderungstext des spielmen�s.
void drawGamePrompt(){

	printf(" Koordinieren Sie einen Angriff oder Abbruch!\n Eingabe: ");
}

// funktion: zeichne den aufforderungstext des hauptmen�s.
void drawMainPrompt(){

	printf(" W�hlen Sie Spiel- o. Testmodus oder Beenden!\n Eingabe: ");
}

// funktion: zeichne die fehlermeldung.
void drawError(){

	printf("Falsche Eingabe!");
}

// funktion: fehlermeldung.
void runError(){

	revertLineBreak(); // funktionsaufruf: kehre mit dem kursor an den bereich der eingabezeile zur�ck.
	drawError(); // funktionsaufruf: zeichne die fehlermeldung.
	revertLineBreak(); // funktionsaufruf: kehre mit dem kursor an den bereich der eingabezeile zur�ck.
	Sleep(800); // funktionsaufruf: wartezeit von 0.8 sekunden.
}

// funktion: beenden.
void runExit(){

	revertLineBreak(); // funktionsaufruf: kehre mit dem kursor an den bereich der eingabezeile zur�ck.
	Sleep(100); // funktionsaufruf: wartezeit von 0.1 sekunden, die funktion kann auch weggelassen werde.
	exit(0); // funktionsaufruf: beenden.
}

// funktion: aktualisiere gesamtabbildung, mit �bergabe der gw�nschten �berschriftenerweiterung.
void refreshFrame(string headerExtension){

	revertLineBreak(); // funktionsaufruf: kehre mit dem kursor an den bereich der eingabezeile zur�ck.
	fflush(stdin); // funktionsaufruf: leere alle zeichen aus dem tastaturpuffer, um diesen wieder f�r neue zeichen freizugeben.
	adjustConsoleWindow(); // funktionsaufruf: halte die fenstergr��e auf grundeinstellung, 400 x 522.
	system("cls"); // funktionsaufruf: leeren der console.
	drawUnderline(); // funktionsaufruf: zeichne eine horizontale linie.
	drawHeader(headerExtension); // funktionsaufruf: zeichne die programm�berschrift mit �bergabe der gw�nschten �berschriftenerweiterung.
	drawUnderline(); // funktionsaufruf: zeichne eine horizontale linie.
	drawBattleScenario(); // funktionsaufruf: zeichne das schlachtfeld mit den schiffe, zeilenweise.
	drawHud(); // funktionsaufruf: zeichne die punktetafel.
	drawUnderline(); // funktionsaufruf: zeichne eine horizontale linie.
	drawFooter(); // funktionsaufruf: zeichne den fu�bereich, also die letzte m�gliche zeile.
	moveCursorToPosition(0, 27); // funktionsaufruf: bewege den kursor an eine gew�nschte position in der konsole.
}

// funktion: enth�lle den schiffstyp.
void passCombatUnitUnveiler(unsigned short int pY, unsigned short int pX, unsigned short int rotation){

	// der schiffstype der trefferzelle.
	unsigned char combatUnit = grid[pY][pX];

	// reservierter wahrheitswert ob noch gez�hlt wird.
	bool stillCountingEnabled = false;

	// schleifendurchlauf, definiere zwei z�hler variablen welche f�r den negativ und positiv bereich zust�ndig sind. 
	// es handelt sich um eine endlosschleife, welche nicht beendet wird, sondern die gesamte funktion wird verlassen,
	// sobald bei der wahrheitswert stillCountingEnabled auf falsch gesetzt bleibt.
	for (unsigned short int iNe = 1, iPo = 1;;){

		// setzte den, wird noch gez�hlt wahrheitswert, auf falsch.
		stillCountingEnabled = false;

		// wenn die rotation horizontal ist.
		if (rotation == 0){

			// wenn die trefferzelle nicht am linken rand des schlachtfeldes liegt und
			// der negative z�hler nicht �ber den linken rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der negative z�hler befindet, auch ein treffer ist.
			if ((pX != 0) && (pX - iNe >= 0) && (gridHitMask[pY][pX - iNe] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				gridHitMask[pY][pX - iNe] = combatUnit; // trefferzelle mit dem negativen z�hler versatz wird in der treffermaske eingetragen, und somit enth�llt.
				iNe++; // erh�he den negativen z�hler.
			}

			// wenn die trefferzelle nicht am rechten rand des schlachtfeldes liegt und
			// der positive z�hler nicht �ber den rechten rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der positive z�hler befindet, auch ein treffer ist.
			if ((pX != GRID_SQUARE_SIZE - 1) && (pX + iPo <= GRID_SQUARE_SIZE - 1) && (gridHitMask[pY][pX + iPo] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				gridHitMask[pY][pX + iPo] = combatUnit; // trefferzelle mit dem positiven z�hler versatz wird in der treffermaske eingetragen, und somit enth�llt.
				iPo++; // erh�he den positiven z�hler.
			}
		}

		// wenn die rotation vertical ist.
		else{

			// wenn die trefferzelle nicht am oberen rand des schlachtfeldes liegt und
			// der negative z�hler nicht �ber den oberen rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der negative z�hler befindet, auch ein treffer ist.
			if ((pY != 0) && (pY - iNe >= 0) && (gridHitMask[pY - iNe][pX] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				gridHitMask[pY - iNe][pX] = combatUnit; // trefferzelle mit dem negativen z�hler versatz wird in der treffermaske eingetragen, und somit enth�llt.
				iNe++; // erh�he den negativen z�hler.
			}

			// wenn die trefferzelle nicht am unteren rand des schlachtfeldes liegt und
			// der positive z�hler nicht �ber den unteren rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der positive z�hler befindet, auch ein treffer ist.
			if ((pY != GRID_SQUARE_SIZE - 1) && (pY + iPo <= GRID_SQUARE_SIZE - 1) && (gridHitMask[pY + iPo][pX] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				gridHitMask[pY + iPo][pX] = combatUnit; // trefferzelle mit dem positiven z�hler versatz wird in der treffermaske eingetragen, und somit enth�llt.
				iPo++; // erh�he den positiven z�hler.
			}
		}

		// wenn keine schleifendurchl�ufe mehr von n�ten sind.
		if (!stillCountingEnabled){

			// auch die trefferzelle wird enth�llt.
			gridHitMask[pY][pX] = combatUnit;
			return;
		}
	}
}

// funktion: wurde das schiff zerst�rt.
bool hasCombatUnitBeenDestroyed(unsigned short int pY, unsigned short int pX, unsigned short int rotation){

	// k�rperl�nge, 48 weil ab dieser stelle, die ziffern in der ascii tabelle stehen und ein character als int, die stelle in der ascii tabelle definiert.
	unsigned short int bodyLenght = ((int)grid[pY][pX] - 48);

	// reservierter wahrheitswert ob noch gez�hlt wird.
	bool stillCountingEnabled = false;

	// taste nun von der zelle aus, je nach rotation, ab, welche nachstehenden zellen des entsprechenden schiffk�rpers schon getroffen sind.
	// sind alle nachstehenden zellen schon getroffen ist das gesamte schiff zerst�rt und es wird ein wahrer wahrheitswert zur�ckgegeben.
	//
	// beispiel: bei einem horizontalen 4er schiff sind schon 3 treffer erfolgreich gemacht und es fehlt nur noch der letze treffer, 
	// welcher an der dritten stelle des schiffsk�rpers vorzunehmen ist, also an der dritten stelle steht noch kein X, so wie hier XX_X dargestellt.
	// geschieht nun die eingabe an dieser dritten stelle, m�ssen folgende pr�fungen geschehen, um zu wissen ob es einen punkt auf komplette zerst�rung gibt.
	//
	// zum einen m�ssen die zwei zellen links von der trefferzelle aus auf schon gemachte treffer hin �berpr�ft werden, sowie die zelle rechts von der trefferzelle.
	// dazu wird in der folgenden z�hlschleife zwei variable definiert, welche f�r den negativ und positven bereich zust�ndig sind.
	// die negative variable z�hlt also die zwei linken zellen und die positive die eine rechte zelle.
	//
	// am ende werden die gez�hlten dergebnisse addiert und mit der schiffsk�rperl�nge verglichen, um nun festzustellen ob das schiff komplett zerst�rt wurde.
	// gleicher vorgang gilt f�r ein verticales schiff, der negative z�hler vergleicht die zellen oberhalb der trefferzelle und der positive die zellen unterhalb.

	// schleifendurchlauf, definiere zwei z�hler variablen welche f�r den negativ und positiv bereich zust�ndig sind. 
	// es handelt sich um eine endlosschleife, welche nicht beendet wird, sondern die gesamte funktion wird verlassen,
	// sobald bei der letzte pr�fung die schiffsk�rperl�nge mit den gez�hlten variablen verglichen wurde.
	for (unsigned short int iNe = 1, iPo = 1;;){

		// setzte den, wird noch gez�hlt wahrheitswert, auf falsch.
		stillCountingEnabled = false;

		// wenn die rotation horizontal ist.
		if (rotation == 0){

			// wenn die trefferzelle nicht am linken rand des schlachtfeldes liegt und
			// der negative z�hler nicht �ber den linken rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der negative z�hler befindet, auch ein treffer ist.
			if ((pX != 0) && (pX - iNe >= 0) && (gridHitMask[pY][pX - iNe] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				iNe++; // erh�he den negativen z�hler.
			}

			// wenn die trefferzelle nicht am rechten rand des schlachtfeldes liegt und
			// der positive z�hler nicht �ber den rechten rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der positive z�hler befindet, auch ein treffer ist.
			if ((pX != GRID_SQUARE_SIZE - 1) && (pX + iPo <= GRID_SQUARE_SIZE - 1) && (gridHitMask[pY][pX + iPo] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				iPo++; // erh�he den positiven z�hler.
			}
		}

		// wenn die rotation vertical ist.
		else{

			// wenn die trefferzelle nicht am oberen rand des schlachtfeldes liegt und
			// der negative z�hler nicht �ber den oberen rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der negative z�hler befindet, auch ein treffer ist.
			if ((pY != 0) && (pY - iNe >= 0) && (gridHitMask[pY - iNe][pX] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				iNe++; // erh�he den negativen z�hler.
			}

			// wenn die trefferzelle nicht am unteren rand des schlachtfeldes liegt und
			// der positive z�hler nicht �ber den unteren rande des schlachtfelde hinnaus ragt und
			// die zelle, an dem sich der positive z�hler befindet, auch ein treffer ist.
			if ((pY != GRID_SQUARE_SIZE - 1) && (pY + iPo <= GRID_SQUARE_SIZE - 1) && (gridHitMask[pY + iPo][pX] == 'X')){

				stillCountingEnabled = true; // es sind noch weitere schleifendurchl�ufe von n�ten.
				iPo++; // erh�he den positiven z�hler.
			}
		}

		// wenn keine schleifendurchl�ufe mehr von n�ten sind.
		if (!stillCountingEnabled){

			// z�hle die ergebnisse der zwei variablen zusammen und vergleiche diese mit der schiffsk�rperl�nge.
			if ((iNe + 1 + iPo) - 2 == bodyLenght){

				return true; // schiff ist komplett zerst�rt.
			}

			return false; // schiff ist noch nicht komplett zerst�rt.
		}
	}
}

// funktion: gib rotation von einer gew�nschten zelle zur�ck.
unsigned short int getRotationByCellContent(unsigned short int pY, unsigned short int pX){

	// zeigerichtung im uhrzeigersinn vom der zelle aus.
	// dient f�r das abtasten der herrum liegenden zellen.
	short int clockwiseVectorSequence[4][2] = { { -1, 0 }, { 0, +1 }, { +1, 0 }, { 0, -1 } };
	
	// reservierte variable f�r die zu ermittelnde rotation, standartm��ig ist der rotationswert auf horizontal gesetzt, also 0.
	unsigned short int rotation = 0;

	// schleifendurchlauf: f�r jede zeigerichtung, also 4.
	for (unsigned short int i = 0; i < sizeof(clockwiseVectorSequence[0]); i++){

		// wenn jeder abtastvorgang nicht auserhalb des schlachtfeldes liegt.
		if ((i == 0 && pY != 0) || (i == 1 && pX != GRID_SQUARE_SIZE - 1) || (i == 2 && pY != GRID_SQUARE_SIZE - 1) || (i == 3 && pX != 0)){

			// wenn die anliegende zelle, welches durch die zeigerichtung von der zelle aus angew�hlt ist, den gleichen wert der zelle hat.
			if (grid[pY + clockwiseVectorSequence[i][0]][pX + clockwiseVectorSequence[i][1]] == grid[pY][pX]){

				// wenn sich die zeigerichtung in verticaler zeige richtung befindet.
				if (i == 0 || i == 2){

					rotation = 1; // die rotation ist vertical.
				}

				break;
			}
		}
	}

	return rotation;
}

// funktion: durchlaufe den punkte h�ndler, um zu schauen ob ein schiff zerst�rt wurde.
void passPointHandler(unsigned short int pY, unsigned short int pX, unsigned short int cui){

	// wertzuweisung durch funktionsaufruf: gib rotation von einer gew�nschten zelle zur�ck.
	unsigned short int rotation = getRotationByCellContent(pY, pX);

	// wenn es sich nur um ein schiff handelt welches l�nger als 1 ist, also 4er, 3er und 2er schiffe.
	if (cui < sizeof(combatUnitTypes) - 1){

		// wertzuweisung durch funktionsaufruf: wurde das schiff zerst�rt.
		bool combatUnitBeenDestroyed = hasCombatUnitBeenDestroyed(pY, pX, rotation);

		// wenn das schiff zerst�rt wurde.
		if (combatUnitBeenDestroyed){

			passCombatUnitUnveiler(pY, pX, rotation); // funktionsaufruf: enth�lle den schiffstyp.
			successHitCounters[cui]++; // z�hle den entsprechenden punktestand des schiffstypes hoch.
		}
	}

	// wenn es sich nur um ein 1er schiff handelt, 
	// denn dieses brauch nicht darauf gepr�ft werden, ob andere teile von dem schiff getroffen wurden.
	else{

		gridHitMask[pY][pX] = combatUnitTypes[cui]; // enth�lle den schiffstyp, durch eintrag des typen in die entsprechende zelle der treffermaske.
		successHitCounters[cui]++; // z�hle den entsprechenden punktestand des schiffstypes hoch, in dem falle den vom typ 1er schiffe.
	}
}

// funktion: ist ein angriff m�glich, bzw. wurde diese schlachtfeld-zelle noch nicht angegriffen.
// also es wird mit der angriffskoordinate eine koordinate aus ganzzahlen generiert,
// sowie die entsprechenden zeichen f�r getroffen oder nicht getroffen in der globalen treffermaske eingetragen.
bool hasAssaultExecuted(char assaultCoord[5]){

	// wenn die gesamte angriffskoordinate g�ltig ist.
	if (assaultCoord[0] != '!'){

		unsigned short int pY = GRID_SQUARE_SIZE; // reservierte ganzzahl, als verweis auf die y_achse der anzugreifenden zelle. erstmal als ung�ltige zahl gesetzt, also 10.
		unsigned short int pX = GRID_SQUARE_SIZE; // reservierte ganzzahl, als verweis auf die x_achse der anzugreifenden zelle. erstmal als ung�ltige zahl gesetzt, also 10.

		bool alreadyHitEnabled = false; // reservierter wahrheitswert, ob die zelle schonmal angegriffen wurden.
		bool newHitEnabled = false; // reservierter wahrheitswert, ob die zelle ein treffer ist.

		// generiere aus den angriffskoorndiate eine koordinate aus ganzzahlen...

		// schleifendurchlauf pro zeichen der globalen bezeichner.
		for (unsigned short int i = 0; i < GRID_SQUARE_SIZE; i++){

			// wenn das erste zeichen der angriffskoordinate einem zeichen der globalen x_achsen bezeichner entspricht.
			if (assaultCoord[0] == abscissa[i]){

				pX = i; // x_achsen verweis bekommt den wert des index.
			}

			// wenn das dritte zeichen der angriffskoordinate ein ; ist.
			if (assaultCoord[2] == ';'){

				// wenn das zweite zeichen der angriffskoordinate einem zeichen der globalen y_achsen bezeichner entspricht.
				if (assaultCoord[1] == ordinate[i]){

					pY = i; // y_achsen verweis bekommt den wert des index.
				}
			}

			// wenn das dritte zeichen der angriffskoordinate kein ; ist.
			else{

				pY = 9; // das bedeutet es kann sich nur um eine 9 handeln, da es nur eine zweistellige zahl geben kann, n�mlich eine 10.
			}

			// wenn beide ganzzahlen x_ und y_achse der anzugreifend zelle eine g�ltige zahl haben, also weniger als 10.
			if (pY < GRID_SQUARE_SIZE && pX < GRID_SQUARE_SIZE){

				break; // breche die schleife vorzeitig ab, um somit die pr�fung nicht weiterzuf�hren.
			}
		}

		// die entsprechenden zeichen f�r getroffen oder nicht getroffen in der globalen treffermaske eingetragen...

		// schleifendurchlauf f�r jeden globalen schiffstype, also 4 mal.
		for (unsigned short int i = 0; i < sizeof(combatUnitTypes); i++){

			// wenn die verwiesene zelle der globale treffermaske ein leerzeichen oder eine raute beinhaltet.
			if (gridHitMask[pY][pX] == ' ' || gridHitMask[pY][pX] == '#'){

				// wenn die verwiesene zelle des schlachtfeldes einem zeichen der globalen schiffstypen entspricht.
				if (grid[pY][pX] == combatUnitTypes[i]){

					gridHitMask[pY][pX] = 'X'; // treffer zeichen an die verwiesene zelle in der globale treffermaske eintragen.
					passPointHandler(pY, pX, i); // funktionsaufruf: durchlaufe den punkte h�ndler, um zu schauen ob ein schiff zerst�rt wurde.
					newHitEnabled = true; // zelle ist ein treffer.
					break;
				}
			}

			// wenn die verwiesene zelle der globale treffermaske kein leerzeichen oder keine raute beinhaltet, also schonmal angegriffen wurde.
			else{

				alreadyHitEnabled = true; // die zelle wurde schonmal angegriffen.
				return false;
			}
		}

		// wenn zelle kein treffer ist.
		if (!newHitEnabled){

			gridHitMask[pY][pX] = 'O'; // nicht treffer zeichen an die verwiesene zelle in der globale treffermaske eintragen.
		}

		assaultCounter++; // zug hochz�hlen, also den globalen z�hler aller durchgef�hrten agriffe erh�hen.

		return true;
	}

	return false;
}

// funktion: durchlaufe die g�ltigkeitspr�fung der eingabe bzw. anriffskoordinate und signiere diese.
// bedeutet einer g�ltigen anriffskoordinate wird ein ; an das ende angef�gt, welches das befehlsende signalisiert.
// sobald eine anriffskoordinate falsch eingegeben wurde, wird diese durch die zeichenfolge !!; ersetzt.
void passAssaultCoordValidator(char coord[5]){
	
	bool absValidated = false; // reservierter wahrheitswert, ob die x_achse der angriffskoordinate g�ltig ist.
	bool ordValidated = false; // reservierter wahrheitswert, ob die y_achse der angriffskoordinate g�ltig ist.

	// wenn die koordinate kleiner als 4 zeichen lang ist, z.b. a1; ist und das zweite zeichen keine 0 ist,
	// oder wenn die koordinate gr��er als 4 zeichen lang ist und das zweite zeichen eine 1 ist und das dritte zeichen eine 0 ist, z.b. a10; ist.
	// hier wir also gepr�ft dass das zweite zeichen keine 0 ist, weil es ja im koordinatensystem des schlachfeldes keine 0 gibt,
	// und es wird gepr�ft das als gr��ter wert nur eine 10 auf dem zweiten und dem dritten zeichen stehen darf, also a11 oder a20 w�rde nicht funktionieren.
	// jedoch z.b. a100... funktioniert noch, kann/muss also noch abgefangen werden. 
	if (((unsigned)strlen(coord) < 4 && coord[1] != '0') || ((unsigned)strlen(coord) < 5 && coord[1] == '1' && coord[2] == '0')){

		// schleifendurchlauf pro zeichen des globalen x_achsen bezeichner.
		for (unsigned short int i = 0; i < sizeof(abscissa); i++){

			// wenn das erste zeichen der angriffskoordinate einem zeichen der globalen x_achsen bezeichner entspricht, 
			// oder einem globalen x_achsen bezechner in kleingeschriebener form entspricht.
			if (coord[0] == abscissa[i] || coord[0] == tolower(abscissa[i])){

				coord[0] = toupper(coord[0]); // erste zeichen der angriffskoordinate gro�schreiben.
				absValidated = true; // x_achse wird auf g�ltig gesetzt.
			}

			// wenn das zweite zeichen der angriffskoordinate einem zeichen der globalen y_achsen bezeichner entspricht. 
			if (coord[1] == ordinate[i]){

				ordValidated = true; // y_achse wird auf g�ltig gesetzt.
			}

			// wenn x_achse und y_achse der angriffskoordinate g�ltig sind.
			if (absValidated && ordValidated){

				// ; an das ende anf�gen, welches das befehlsende signalisiert, je nachdem ob die kooridnaten z.b. a1 oder a10 sind,
				// demzufolge endweder an dritter stelle oder an vierter stelle das ; bekommen.

				if ((unsigned)strlen(coord) < 4){

					coord[2] = ';'; // ; and das ende anf�gen, also an die dritte stelle.
				}
				else{

					coord[3] = ';'; // ; and das ende anf�gen, also an die vierte stelle.
				}

				return;
			}
		}
	}

	// sobald dieser bereicht erreicht wird, gilt es die angriffskoordinate als ung�ltig zu signieren.

	coord[0] = '!'; // erstes zeichen als ung�ltig signieren.
	coord[1] = '!'; // zweites zeichen als ung�ltig signieren.
	coord[2] = ';'; // ; and das ende anf�gen, welches das befehlsende signalisiert.
}

// funktion: men� schleife der zwei modi.
void runGameMenu(string type){
	
	// spielmen�schleife endlos.
	while (true){

		// wenn standard_mode_menu als type �bergeben wurde.
		if (type == "standard_mode_menu"){

			// funktionsaufruf: aktualisiere gesamtabbildung, mit �bergabe der gw�nschten �berschriftenerweiterung.
			// in dem falle die �berschriftenerweiterung f�r den spielmodus.
			refreshFrame("SPIELMODUS");
		}

		// wenn test_mode_menu als type �bergeben wurde.
		else if (type == "test_mode_menu"){

			// funktionsaufruf: aktualisiere gesamtabbildung, mit �bergabe der gw�nschten �berschriftenerweiterung.
			// in dem falle die �berschriftenerweiterung f�r den testmodus.
			refreshFrame("TESTMODUS");
		}
		
		// funktionsaufruf: zeichne den aufforderungstext des spielmen�s.
		drawGamePrompt();

		char assaultCoord[5]; // reservierte variable f�r die agriffskoordinate, 5 weil A10; = 4 + \n = 5.
		fgets(assaultCoord, 5, stdin); // wetzuweisung der eingabe an die reservierte angriffskoordinate variable.

		// wenn die eingabe mehr als 1 zeichen betr�gt und weniger als 4 zeichen lang ist.
		if ((unsigned)strlen(assaultCoord) > 2 && (unsigned)strlen(assaultCoord) < 5){

			// funktionsaufruf: durchlaufe die g�ltigkeitspr�fung der eingabe bzw. anriffskoordinate und signiere diese.
			// bedeutet einer g�ltigen anriffskoordinate wird ein ; an das ende angef�gt, welches das befehlsende signalisiert.
			// sobald eine anriffskoordinate falsch eingegeben wurde, wird diese durch die zeichenfolge !!; ersetzt.
			// dieser aufbereitungen bzw. vorbereitung der anriffskoordinate wird f�r die nachfolgenden funktionen ben�tigt.
			passAssaultCoordValidator(assaultCoord);

			// wertzuweisung durch funktionsaufruf: ist ein angriff m�glich, bzw. wurde diese schlachtfeld-zelle noch nicht angegriffen.
			// also es wird mit der zuvor auf g�ltigkeit gepr�ften angriffskoordinate eine koordinate aus ganzzahlen generiert,
			// sowie die entsprechenden zeichen f�r getroffen oder nicht getroffen in der globalen treffermaske eingetragen.
			bool assaultExecuted = hasAssaultExecuted(assaultCoord);

			// wenn ein angriff nicht m�glich war, bzw. diese schlachtfeld-zelle schonmal angegriffen wurde.
			if (!assaultExecuted){

				runError(); // funktionsaufruf: fehlermeldung.
			}
		}

		// wenn die eingabe nur 1 zeichen betr�gt und entweder eine 3, ein q oder Q ist.
		else if ((unsigned)strlen(assaultCoord) == 2 && (assaultCoord[0] == '3' || assaultCoord[0] == 'q' || assaultCoord[0] == 'Q')){

			reset(); // funktionsaufruf: schlachtfeld bzw. alle zellen und z�hlervariablen leeren.
			break;
		}
		
		// wenn alles jenseits der g�ltigkeit eingegeben worden ist.
		else{

			runError(); // funktionsaufruf: fehlermeldung.
		}
	}
}

// funktion: erzeuge den schiffshof um dem schiffsk�rper herrum.
void generateCombatUnitBodyMargin(unsigned short int cellY, unsigned short int cellX, unsigned short int bodyRotation, unsigned char combatUnitType){

	// k�rperl�nge, 48 weil ab dieser stelle, die ziffern in der ascii tabelle stehen und ein character als int, die stelle in der ascii tabelle definiert.
	unsigned short int bodyLenght = ((int)combatUnitType - 48);

	// wenn die rotation horizontal ist.
	if (bodyRotation == 0){

		// wenn schiffshof oberhalb des k�rpers m�glich ist.
		if (cellY != 0){

			// wenn anfangsst�ck des schiffshofes m�glich ist.
			if (cellX != 0){

				grid[cellY - 1][cellX - 1] = 'm';
			}

			// schleifendurchlauf, f�lle schiffshof oberhalb des k�rpers entlang.
			for (unsigned short int cL = 0; cL < bodyLenght; cL++){

				grid[cellY - 1][cellX + cL] = 'm';
			}

			// wenn endst�ck des schiffhofes m�glich ist.
			if (cellX + bodyLenght != GRID_SQUARE_SIZE){

				grid[cellY - 1][cellX + bodyLenght] = 'm';
			}

		}

		// wenn schiffshof unterhalb des k�rpers m�glich ist.
		if (cellY != GRID_SQUARE_SIZE - 1){

			// wenn anfangsst�ck des schiffhofes m�glich ist.
			if (cellX != 0){

				grid[cellY + 1][cellX - 1] = 'm';
			}

			// schleifendurchlauf, f�lle schiffshof unterhalb des k�rpers entlang.
			for (unsigned short int cL = 0; cL < bodyLenght; cL++){

				grid[cellY + 1][cellX + cL] = 'm';
			}

			// wenn endst�ck des schiffshofes m�glich ist.
			if (cellX + bodyLenght != GRID_SQUARE_SIZE){

				grid[cellY + 1][cellX + bodyLenght] = 'm';
			}
		}

		// wenn schiffshof links des k�rpers m�glich ist.
		if (cellX != 0){

			grid[cellY][cellX - 1] = 'm';
		}

		// wenn schiffshof rechts des k�rpers m�glich ist.
		if (cellX + bodyLenght != GRID_SQUARE_SIZE){

			grid[cellY][cellX + bodyLenght] = 'm';
		}
	}

	// wenn die rotation vertical ist.
	else {

		// wenn schiffshof links des k�rpers m�glich ist.
		if (cellX != 0){

			// wenn anfangsst�ck des schiffshofes m�glich ist.
			if (cellY != 0){

				grid[cellY - 1][cellX - 1] = 'm';
			}

			// schleifendurchlauf, f�lle schiffshof links des k�rpers entlang.
			for (unsigned short int cL = 0; cL < bodyLenght; cL++){

				grid[cellY + cL][cellX - 1] = 'm';
			}

			// wenn endst�ck des schiffshofes m�glich ist.
			if (cellY + bodyLenght != GRID_SQUARE_SIZE){

				grid[cellY + bodyLenght][cellX - 1] = 'm';
			}

		}

		// wenn schiffshof rechts des k�rpers m�glich ist.
		if (cellX != GRID_SQUARE_SIZE - 1){

			// wenn anfangsst�ck des schiffhofes m�glich ist.
			if (cellY != 0){

				grid[cellY - 1][cellX + 1] = 'm';
			}

			// schleifendurchlauf, f�lle schiffshof rechts des k�rpers entlang.
			for (unsigned short int cL = 0; cL < bodyLenght; cL++){

				grid[cellY + cL][cellX + 1] = 'm';
			}

			// wenn endst�ck des schiffhofes m�glich ist.
			if (cellY + bodyLenght != GRID_SQUARE_SIZE){

				grid[cellY + bodyLenght][cellX + 1] = 'm';
			}
		}

		// wenn schiffshof oberhalb des k�rpers m�glich ist.
		if (cellY != 0){

			grid[cellY - 1][cellX] = 'm';
		}

		// wenn schiffshof unterhalb des k�rpers m�glich ist.
		if (cellY + bodyLenght != GRID_SQUARE_SIZE){

			grid[cellY + bodyLenght][cellX] = 'm';
		}
	}
}

// funktion: erzeuge den schiffsk�rper, orientiert nach den �bergewerten.
void generateCombatUnitBody(unsigned short int cellY, unsigned short int cellX, unsigned short int bodyRotation, unsigned char combatUnitType){

	// k�rperl�nge, 48 weil ab dieser stelle, die ziffern in der ascii tabelle stehen und ein character als int, die stelle in der ascii tabelle definiert.
	unsigned short int bodyLenght = ((int)combatUnitType - 48);

	// schleifendurchlauf der k�rperl�nge in jeder seiner wachstumsphasen.
	for (unsigned short int cL = 0; cL < bodyLenght; cL++){

		// wenn die rotation horizontal ist.
		if (bodyRotation == 0){

			// f�lle in dieser horizontalen wachstumsphase die zelle mit dem schiffstyp.
			grid[cellY][cellX + cL] = combatUnitType;
		}

		// wenn die rotation vertical ist.
		else{

			// f�lle in dieser verticalen wachstumsphase die zelle mit dem schiffstyp.
			grid[cellY + cL][cellX] = combatUnitType;
		}
	}
}

// funktion: hat das schiff mit einem anderem schiff oder dessen schiffshof kollidiert.
bool hasCombatUnitBodyCollided(unsigned short int cellY, unsigned short int cellX, unsigned short int bodyRotation, unsigned char combatUnitType){

	// k�rperl�nge, 48 weil ab dieser stelle, die ziffern in der ascii tabelle stehen und ein character als int, die stelle in der ascii tabelle definiert.
	unsigned short int bodyLenght = ((int)combatUnitType - 48);

	// wenn die k�rperl�nger mit verticaler rotation �ber das schlachtfeld hinnausragt. 
	if (bodyRotation == 1 && cellY + bodyLenght > GRID_SQUARE_SIZE){

		return true; // kollision erfolgt.
	}

	// wenn die k�rperl�nger mit horizontaler rotation �ber das schlachtfeld hinnausragt.
	if (bodyRotation == 0 && cellX + bodyLenght > GRID_SQUARE_SIZE){

		return true; // kollision erfolgt.
	}

	// schleifendurchlauf, um die k�rperl�nge in jeder seiner wachstumsphasen auf kollision hin zu pr�fen.
	for (unsigned short int cL = 0; cL < bodyLenght; cL++){

		// wenn die rotation horizontal ist.
		if (bodyRotation == 0){

			// wenn die horziontale wachstumsphase nicht in einer zelle mit einem e-zeichen liegt, sondern in einer mit m-zeichen.
			if (grid[cellY][cellX + cL] != 'e'){

				return true; // kollision erfolgt.
			}
		}

		// wenn die rotation vertical ist.
		else{

			// wenn die verticale wachstumsphase nicht in einer zelle mit einem e-zeichen liegt, sondern in einer mit m-zeichen.
			if (grid[cellY + cL][cellX] != 'e'){

				return true; // kollision erfolgt.
			}
		}
	}

	return false; // keine kollision.
}

// funktionsaufruf: erzeuge ein schiff auf einer zuf�lligen position und mit einer zuf�lligen rotation.
void dropCombatUnitOnGrid(unsigned char combatUnitType, unsigned short int cellY, unsigned short int cellX, unsigned short int bodyRotation){

	// funktionsaufruf: hat das schiff mit einem anderem schiff oder dessen schiffshof kollidiert.
	if (!hasCombatUnitBodyCollided(cellY, cellX, bodyRotation, combatUnitType)){

		generateCombatUnitBody(cellY, cellX, bodyRotation, combatUnitType); // funktionsaufruf: erzeuge den schiffsk�rper, orientiert nach den �bergewerten.
		generateCombatUnitBodyMargin(cellY, cellX, bodyRotation, combatUnitType); // funktionsaufruf: erzeuge den schiffshof um dem schiffsk�rper herrum.
	}

	// hat keine kollision stattgefunden.
	else{

		// puzzel so lange das schiff bis es passt, also verschiebe es solange, bis es wieder an dem anfang ist, 
		// danach rotiere es und verschiebe wieder, usw., es handelt sich um eine rekursion.

		// wenn der x_wert der zelle nicht den linken rand erreicht hat, also noch �ber 0 ist.
		if (cellX > 0){

			// verschiebe den x_wert der zelle weiter nach links, also minus 1.
			cellX--;
		}

		// wenn der x_wert der zelle den linken rand erreicht hat, also 0 ist.
		else{

			// verschiebe den x_wert der zelle wieder ganz nach rechts, also den wert der schlachtfeldbreite minus 1.
			cellX = GRID_SQUARE_SIZE - 1;

			// wenn der y_wert der zelle nicht den oberen rand erreicht hat, also noch �ber 0 ist.
			if (cellY > 0){

				// verschiebe den y_wert der zelle weiter nach oben, also minus 1.
				cellY--;
			}

			// wenn der y_wert der zelle den oberen rand erreicht hat, also 0 ist.
			else{

				// verschiebe den y_wert der zelle wieder ganz nach unten, also den wert der schlachtfeldh�he minus 1.
				cellY = GRID_SQUARE_SIZE - 1;
			}
		}

		// wenn die verschiebung nicht so weit gelaufen ist, dass der y_wert und der x_wert der zelle wieder erreicht wurden.
		if (!(cellY == cellpointer[0] && cellX == cellpointer[1])){

			// rekursion.
			dropCombatUnitOnGrid(combatUnitType, cellY, cellX, bodyRotation);
		}

		// wenn die verschiebung so weit gelaufen ist, dass der y_wert und der x_wert der zelle wieder erreicht wurden.
		// diese verzweigung kann auch weggelassen werden, da kein schiff so lang ist, dass es mit einer bestimmten rotation nicht reinpassen k�nnte.
		else{

			// neuer rotationswert.
			unsigned short int newbodyRotation = 0;

			// wenn der bisherige rotationswert 0 ist.
			if (bodyRotation == 0){

				// neuer rotationswert bekommt die 1.
				newbodyRotation = 1;
			}

			// rekursion, mit neuem rotationswert.
			dropCombatUnitOnGrid(combatUnitType, cellY, cellX, newbodyRotation);
		}
	}
}

// funktion: gib eine zuf�llige rotation zur�ck, f�r den schiffsk�rper von einem schiff.
unsigned short int getRandomRotation(){

	return (rand() % 2) + 0; // generierter zufall zwischen 0 und 1 zur�ckgeben.
}

// funktion: gib eine zuf�llige zellen position zur�ck, f�r ein schiff.
unsigned short int *getRandomCellpointer(){

	unsigned short int point[2];

	point[0] = (rand() % GRID_SQUARE_SIZE) + 0; // generierter zufall zwischen 0 und der globalen quadratzahl, welche die schlachtfeldgr��e beschreibt, f�r die y_achse.
	point[1] = (rand() % GRID_SQUARE_SIZE) + 0; // generierter zufall zwischen 0 und der globalen quadratzahl, welche die schlachtfeldgr��e beschreibt, f�r die x_achse.

	return point;
}

// funktion: markiere die schiffe mit einer raute, um diese f�r den testmodus sichtbar zu machen.
void markCombatUnitsAsVisible(){

	// schleifendurchlauf, um zeilenweise alle zeilen der treffermaske f�r das sichtbarmachen zu durchlaufen.
	for (unsigned short int i = 0; i < GRID_SQUARE_SIZE; i++){

		// schleifendurchlauf, um alle spalten der zeile f�r das sichtbarmachen zu durchlaufen.
		for (unsigned short int j = 0; j < GRID_SQUARE_SIZE; j++){

			// wenn die zelle des schlachtfeldes kein e- und kein m-zeichen beinhaltet.
			if (grid[i][j] != 'e' && grid[i][j] != 'm'){

				// f�lle die zelle der treffermaske mit einer raute.
				gridHitMask[i][j] = '#';
			}
		}
	}
}

// funktion: erstelle neue schlacht, bzw. positioniere alle schiffe.
void createNewBattleScenario(){

	// schleifendurchlauf f�r jeden globalen schiffstype, also 4 mal.
	for (unsigned short int i = 0; i < sizeof(combatUnitTypes); i++){

		// schleifendurchlauf, so oft ein schiff f�r diesen schiffstype vorkommen soll.
		for (unsigned short int j = 0; j < combatUnitTypeOccurrences[i]; j++){

			unsigned short int randomCellY = getRandomCellpointer()[0]; // wertezuweisung durch funktionsaufruf: eine zuf�llige zellen position y, also spalte.
			unsigned short int randomCellX = getRandomCellpointer()[1]; // wertezuweisung durch funktionsaufruf: eine zuf�llige zellen position x, also zeile.

			cellpointer[0] = randomCellY; // zuweisung der spalten position, an den globalen zellen zeiger.
			cellpointer[1] = randomCellX; // zuweisung der zeilen position, an den globalen zellen zeiger.

			// funktionsaufruf: erzeuge ein schiff auf einer zuf�lligen position und mit einer zuf�lligen rotation.
			dropCombatUnitOnGrid(combatUnitTypes[i], randomCellY, randomCellX, getRandomRotation());
		}
	}
}

// funktion: spiel modus.
void runSpielmodus(){

	createNewBattleScenario(); // funktionsaufruf: erstelle neue schlacht, bzw. positioniere alle schiffe.
	runGameMenu("standard_mode_menu"); // funktionsaufruf: men� schleife des standard modus.
}

// funktion: test modus.
void runTestmodus(){

	createNewBattleScenario(); // funktionsaufruf: erstelle neue schlacht, bzw. positioniere alle schiffe.
	markCombatUnitsAsVisible(); // funktionsaufruf: markiere die schiffe mit einer raute, um diese f�r den testmodus sichtbar zu machen.
	runGameMenu("test_mode_menu"); // funktionsaufruf: men� schleife des test modus.
}

// funktion: hauptmen� schleife.
void runMainMenu(){

	// schleifenkopf endlos.
	while (true){

		// funktionsaufruf: aktualisiere gesamtabbildung, mit �bergabe der gw�nschten �berschriftenerweiterung.
		// in dem falle die �berschriftenerweiterung f�r das hauptmen�.
		refreshFrame("HAUPTMEN�");

		// funktionsaufruf: zeichne den aufforderungstext des hauptmen�s.
		drawMainPrompt();

		// men� navigation, wartet auf eine eingabe des benutzers, ob dieser eine 1, 2, 3, q, Q oder etwas anderes eingibt.
		switch (getchar()){
		case '1':
			runSpielmodus(); // funktionsaufruf: spiel modus.
			break;
		case '2':
			runTestmodus(); // funktionsaufruf: test modus.
			break;
		case '3':
			runExit(); // funktionsaufruf: beenden.
		case 'q':
			runExit(); // funktionsaufruf: beenden.
		case 'Q':
			runExit(); // funktionsaufruf: beenden.
		default:
			runError(); // funktionsaufruf: fehlermeldung.
		}
	}
}

// ----------------------------------------------------------------------------------------------
// main.
// ----------------------------------------------------------------------------------------------

// funktion: hauptschleife.
void main( int argc, _TCHAR* argv[] ){

	// funktionsaufruf: farbe der schrift und des hintergrundes einstellen.
	// also stellt die schriftfarbe auf absolutes weiss.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	// sonderzeichen f�r deutsche sprache freischalten.
	locale::global(locale("German_germany"));

	// funktionsaufruf: inizialisierung des zufallsgenerators.
	srand(time(NULL));

	// funktionsaufruf: schlachtfeld, bzw. alle zellen und z�hlervariablen leeren.
	reset();

	// funktionsaufruf: einstieg in die hauptmen� schleife.
	runMainMenu();
}