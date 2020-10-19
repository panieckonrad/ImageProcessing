#pragma once
class MyClass
{

public:
	BITMAPFILEHEADER header;
	LPVOID obrazInfo;
	LPVOID obrazBajty;
	LPVOID obrazKolory;
	DWORD sizeOfFile;
	int typeOfImage;
	int fileWidth = 0, fileHeight = 0;
	float* histogram;

	MyClass();
	~MyClass();

	//LAB1
	bool LoadDIB(CString sciezka_do_pliku);
	bool PaintDIB(HDC kontekst, CRect r);
	bool CreateGreyscaleDIB(CRect rozmiar_obrazu, int xPPM, int yPPM);
	//do tworzenia pustych 8 - bitowych bitmap w odcieniach szaro�ci o zadanym rozmiarze (PPM oznacza rozdzielczo�� pixels per meter),
	bool GetPixel1(int x, int y);
	//do odczytywania warto�ci pikseli w bitmapach 1 - bitowych,
	BYTE GetPixel8(int x, int y);
	//do odczytywania warto�ci pikseli w bitmapach 8 - bitowych,
	RGBTRIPLE GetPixel24(int x, int y);
	//do odczytywania warto�ci pikseli w bitmapach 24 - bitowych,
	bool SetPixel8(int x, int y, BYTE val);
	//do ustawiania warto�ci pikseli w bitmapach 8 - bitowych,
	bool SaveDIB(CString sciezka_do_pliku);
	//do zapisywania plik�w BMP.

	//LAB2
	void ChangeBrightness(float value);
	void ChangeKontrast(float value);
	void ChangePotega(float value);
	void Negatyw();
	void CalculateHistogram(int startX, int startY, int endX, int endY);
	void ShowHistogram();
	void WyrownajHistogram();
	void WygladzHistogram();
};

