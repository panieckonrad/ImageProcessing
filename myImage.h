#pragma once
class myImage
{			

public:
	BITMAPFILEHEADER header;
	LPVOID ImageInfo;
	LPVOID ImageBajty;
	LPVOID ImageKolory;
	DWORD sizeOfFile;
	int typeOfImage;
	int fileWidth = 0, fileHeight = 0;
	float* histogram;

	myImage();
	~myImage();

	//LAB1
	bool LoadDIB(CString sciezka_do_pliku);
	bool PaintDIB(HDC kontekst, CRect r);
	bool CreateGreyscaleDIB(CRect rozmiar_obrazu, int xPPM, int yPPM);
	void CreateGreyscaleDIBWhite(CRect rozmiar_obrazu, int xPPM, int yPPM);
	bool GetPixel1(int x, int y);
	BYTE GetPixel8(int x, int y);
	RGBTRIPLE GetPixel24(int x, int y);
	bool SetPixel8(int x, int y, BYTE val);
	bool SaveDIB(CString sciezka_do_pliku);

	//LAB2
	void ChangeBrightness(float value);
	void ChangeContrast(float value);
	void ChangeExponent(float value);
	void Negative();
	void CalculateHistogram(int sX, int sY, int eX, int eY); // baza do histogramow
	void ShowHistogram(int threshold);
	void EqualizeHistogram();

	//LAB3
	void ManualThreshold(int threshold);
	void IterativeSegmentation();
	void GradientSegmentation();
	int OtsuSegmentation();

	//LAB4
	void LinearFilter(int mask[3][3], bool maskaWyostrzajaca);
	void MedianFilter(int value, int option);
	void LogFilter(int value);

	//LAB5
	void Pavlidis(HDC context, CRect r);
	void MapaOdleglosci(int t);

	//LAB6
	void Dylatacja();
	void Erozja();
	void Otwarcie();
	void Zamkniecie();
	void KonturWewnetrzny();
	void KonturZewnetrzny();
	void Dylatacja8();
	void Erozja8();
	void Otwarcie8();
	void Zamkniecie8();
	void WhiteTopHat();
	void BlackTopHat();

	//LAB7
	void HoughWykres(int grow);
	void Hough(int grow, int parameter);

	//LAB8
	void Hamming();
	void FourierAmplituda();
	void FourierFaza();
};

