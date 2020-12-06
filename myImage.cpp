#include "stdafx.h"
#include "myImage.h"
#include "Math.h"
#include "HistogramWindow.h"
#include <vector>
#include <map>
using namespace std;

myImage::myImage()
{
}

myImage::~myImage()
{
}

bool myImage::LoadDIB(CString sciezka_do_pliku) { // wczytaj Image
	CFile f;
	header;
	f.Open(sciezka_do_pliku, CFile::modeReadWrite);
	f.Read(&header, sizeof(BITMAPFILEHEADER));

	sizeOfFile = header.bfSize - sizeof(BITMAPFILEHEADER);

	ImageInfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeOfFile);
	f.Read(ImageInfo, sizeOfFile);
	ImageBajty = (BYTE*)ImageInfo + header.bfOffBits - sizeof(BITMAPFILEHEADER);

	BITMAPINFOHEADER* info = (BITMAPINFOHEADER*)ImageInfo;

	fileWidth = info->biWidth;
	fileHeight = info->biHeight;
	typeOfImage = info->biBitCount;

	f.Close();
	return true;
}

bool myImage::PaintDIB(HDC kontekst, CRect r)
{
	SetStretchBltMode(kontekst, COLORONCOLOR);
	int x = 0, y = 0; // d = destination
	int szerokosc;
	int wysokosc;

	if (fileWidth > fileHeight) { // jesli szerokosc wieksza niz wysokosc to skalujemy wysokosc
		szerokosc = r.Width();
		wysokosc = (float)fileHeight / (float)fileWidth * (float)(r.Height());
		y = (r.Height() - wysokosc) / 2;
	}
	else { // w przeciwnym wypadku szerokosc
		wysokosc = r.Height();
		szerokosc = (float)fileWidth / (float)fileHeight * (float)(r.Width());
		x = (r.Width() - szerokosc) / 2;
	}

	StretchDIBits(kontekst, x, y, szerokosc, wysokosc,
		0, 0, fileWidth, fileHeight,
		ImageBajty,
		(BITMAPINFO*)ImageInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	return true;
}

//do wyœwietlania wczytanych Imageów,
bool myImage::GetPixel1(int x, int y)
{
	long int index = y * ((fileWidth + 31) / 32) * 4 + x / 8;
	BYTE byte = ((BYTE*)ImageBajty)[index];
	bool bit = byte & (0x80 >> (x % 8));

	return bit;
}
//do odczytywania wartoœci pikseli w bitmapach 1 - bitowych,
BYTE myImage::GetPixel8(int x, int y)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	return ((BYTE*)ImageBajty)[index];
}
//do odczytywania wartoœci pikseli w bitmapach 8 - bitowych,
RGBTRIPLE myImage::GetPixel24(int x, int y)
{
	int index = y * ((24 * fileWidth + 31) / 32) * 4 + 3 * x;
	BYTE* color = (BYTE*)ImageBajty + index;
	return ((RGBTRIPLE*)color)[0];
}
//do odczytywania wartoœci pikseli w bitmapach 24 - bitowych,

bool myImage::SetPixel8(int x, int y, BYTE val)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	((BYTE*)ImageBajty)[index] = val;
	return true;
}

bool myImage::CreateGreyscaleDIB(CRect rozmiar_Imageu, int xPPM, int yPPM)
{
	int rozmiar_palety = 256 * sizeof(RGBQUAD);

	int szerokosc = fileWidth;
	int wysokosc = fileHeight;
	int liczba_pixeli = szerokosc * wysokosc;

	int rozmiar_tablicy_pixeli = (((szerokosc * 8 + 31) / 32) * 4) * wysokosc;
	DWORD rozmiar_pamieci = sizeof(BITMAPINFOHEADER) + rozmiar_palety + rozmiar_tablicy_pixeli;
	LPVOID Image = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, rozmiar_pamieci);

	BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)Image;
	info_header->biBitCount = 8;
	info_header->biHeight = wysokosc;
	info_header->biWidth = szerokosc;
	info_header->biSize = sizeof(BITMAPINFOHEADER);
	info_header->biPlanes = 1;
	info_header->biCompression = BI_RGB;
	info_header->biSizeImage = rozmiar_tablicy_pixeli;
	info_header->biXPelsPerMeter = ((BITMAPINFOHEADER*)ImageInfo)->biXPelsPerMeter;
	info_header->biYPelsPerMeter = ((BITMAPINFOHEADER*)ImageInfo)->biYPelsPerMeter;
	info_header->biClrUsed = 0;
	info_header->biClrImportant = 0;

	RGBQUAD* paleta_kolorow = (RGBQUAD*)((BYTE*)Image + info_header->biSize);

	RGBQUAD kolor;
	for (int i = 0; i < 256; i++)
	{
		kolor.rgbBlue = i;
		kolor.rgbGreen = i;
		kolor.rgbRed = i;
		kolor.rgbReserved = 0;
		paleta_kolorow[i] = kolor;
	}

	BITMAPINFO* bitmap_info = (BITMAPINFO*)Image;

	BYTE* tablica_pixeli = (BYTE*)paleta_kolorow + rozmiar_palety;

	int bitMapType = ((BITMAPINFOHEADER*)ImageInfo)->biBitCount;

	for (int y = 0; y < wysokosc; y++)
	{
		for (int x = 0; x < szerokosc; x++)
		{
			int index = y * ((szerokosc * 8 + 31) / 32) * 4 + x;
			if (bitMapType == 1)
			{
				bool values = GetPixel1(x, y);
				tablica_pixeli[index] = values ? 255 : 0;
			}
			else if (bitMapType == 8)
			{
				BYTE byteValue = GetPixel8(x, y);
				tablica_pixeli[index] = byteValue;
			}
			else if (bitMapType == 24)
			{
				RGBTRIPLE kolor = GetPixel24(x, y);
				BYTE byteValue = 0.299 * (float)kolor.rgbtRed + 0.587 * (float)kolor.rgbtGreen + 0.114 * (float)kolor.rgbtBlue;
				tablica_pixeli[index] = byteValue;
			}
		}
	}

	BITMAPFILEHEADER file_header;

	file_header = header;
	file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + rozmiar_palety;
	file_header.bfSize = sizeof(BITMAPFILEHEADER) + info_header->biSize + rozmiar_palety + rozmiar_tablicy_pixeli;

	header = file_header;
	ImageInfo = bitmap_info;
	ImageKolory = paleta_kolorow;
	ImageBajty = tablica_pixeli;

	sizeOfFile = header.bfSize - sizeof(BITMAPFILEHEADER);


	return true;
}

void myImage::CreateGreyscaleDIBWhite(CRect rect, int xPPM, int yPPM) { // lab7
	int rozmiar_palety = 256 * sizeof(RGBQUAD);

	int szerokosc = rect.right;
	int wysokosc = rect.bottom;
	int liczba_pixeli = szerokosc * wysokosc;

	int rozmiar_tablicy_pixeli = (((szerokosc * 8 + 31) / 32) * 4) * wysokosc;
	DWORD rozmiar_pamieci = sizeof(BITMAPINFOHEADER) + rozmiar_palety + rozmiar_tablicy_pixeli;
	LPVOID Image = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, rozmiar_pamieci);

	BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)Image;
	info_header->biBitCount = 8;
	info_header->biHeight = wysokosc;
	info_header->biWidth = szerokosc;
	info_header->biSize = sizeof(BITMAPINFOHEADER);
	info_header->biPlanes = 1;
	info_header->biCompression = BI_RGB;
	info_header->biSizeImage = rozmiar_tablicy_pixeli;
	info_header->biXPelsPerMeter = xPPM;
	info_header->biYPelsPerMeter = yPPM;
	info_header->biClrUsed = 0;
	info_header->biClrImportant = 0;

	RGBQUAD* paleta_kolorow = (RGBQUAD*)((BYTE*)Image + info_header->biSize);

	RGBQUAD kolor;
	for (int i = 0; i < 256; i++)
	{
		kolor.rgbBlue = i;
		kolor.rgbGreen = i;
		kolor.rgbRed = i;
		kolor.rgbReserved = 0;
		paleta_kolorow[i] = kolor;
	}

	BITMAPINFO* bitmap_info = (BITMAPINFO*)Image;

	BYTE* tablica_pixeli = (BYTE*)paleta_kolorow + rozmiar_palety;

	int bitMapType = ((BITMAPINFOHEADER*)ImageInfo)->biBitCount;

	for (int y = 0; y < wysokosc; y++) // make every pixel white
	{
		for (int x = 0; x < szerokosc; x++)
		{
			int index = y * ((szerokosc * 8 + 31) / 32) * 4 + x;
			tablica_pixeli[index] = 255;
		}
	}

	BITMAPFILEHEADER file_header;

	file_header = header;
	file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + rozmiar_palety;
	file_header.bfSize = sizeof(BITMAPFILEHEADER) + info_header->biSize + rozmiar_palety + rozmiar_tablicy_pixeli;

	header = file_header;
	ImageInfo = bitmap_info;
	ImageKolory = paleta_kolorow;
	ImageBajty = tablica_pixeli;
	fileWidth = szerokosc;
	fileHeight = wysokosc;

	sizeOfFile = header.bfSize - sizeof(BITMAPFILEHEADER);
}

bool myImage::SaveDIB(CString sciezka_do_pliku)
{
	CFile f;
	f.Open(sciezka_do_pliku, CFile::modeCreate);
	f.Close();
	f.Open(sciezka_do_pliku, CFile::modeWrite);

	f.Write(&header, sizeof(BITMAPFILEHEADER));
	f.Write(ImageInfo, sizeOfFile);

	f.Close();

	return true;
}

BYTE Floor(float value) {
	if (value > 255) {
		value = 255;
	}
	else if (value < 0) {
		value = 0;
	}
	return (BYTE)value;
}

void myImage::ChangeBrightness(float value) {
	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {
			SetPixel8(x, y, Floor(GetPixel8(x, y) + value));
		}
	}


}
void myImage::ChangeContrast(float value)
{
	value /= 10.0;

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			SetPixel8(x, y, Floor((GetPixel8(x, y) - 127) * value + 127));
		}
	}
}

void myImage::ChangeExponent(float value) {
	float newValue = value / 10.0;

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			float zmianaKoloru = (float)pow((float)(GetPixel8(x, y)) / 255, newValue);
			SetPixel8(x, y, zmianaKoloru*255);
		}
	}
}

void myImage::Negative() {
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			SetPixel8(x, y, Floor(255-GetPixel8(x,y)));
		}
	}
}

void myImage::CalculateHistogram(int sX, int sY, int eX, int eY) {
	histogram = new float[256];
	for (int x = 0; x < 256; x++)
		histogram[x] = 0;
	int width = eX - sX;
	int height = eY - sY;
	int histogramCount = width * height;

	for (int x = sX; x < eX; x++)
	{
		for (int y = sY; y < eY; y++)
		{
			int byteValue = GetPixel8(x, y);
			histogram[byteValue]++;
		}
	}
	for (int x = 0; x < 256; x++)
		histogram[x] = histogram[x] / histogramCount;
}

void myImage::ShowHistogram(int threshold) { // wyswietl histogram
	HistogramWindow dialog;
	dialog.values = histogram;
	if (threshold >= 0 && threshold < 256)
		dialog.threshold = threshold;
	dialog.DoModal();
}

void myImage::EqualizeHistogram() {
	float dystrybuanta[256] = { 0 };
	int histogramCount = fileWidth * fileHeight;

	float sum = 0;
	for (int i = 0; i < 256; i++)
	{
		sum += (histogram[i] * histogramCount);
		dystrybuanta[i] = sum;
	}

	float min = 0;
	for (int i = 0; i < 256; i++) {
		if (dystrybuanta[i] > 0) {
			min = dystrybuanta[i];
			break;
		}
	}

	for (int x = 0; x < fileWidth; x++)
	{
		for (int y = 0; y < fileHeight; y++) 
		{
			int byteValue = GetPixel8(x, y);
			int newValue = ((dystrybuanta[byteValue] - min) / (float)(histogramCount - min)) * 255;
			SetPixel8(x, y, newValue);
		}
	}
	CalculateHistogram(0, 0, fileWidth, fileHeight);
}

//LAB3

void myImage::ManualThreshold(int t)
{
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			if (GetPixel8(x, y) > t)
			{
				SetPixel8(x, y, 255);
			}
			else
			{
				SetPixel8(x, y, 0);
			}
		}
	}
}

void myImage::IterativeSegmentation() {
	CalculateHistogram(0, 0, fileWidth, fileHeight);

	float t0;
	float t1;

	float max = 0;
	float min = 255;

	for (int y = 0; y < fileHeight; y++) { // znajdz min i max
		for (int x = 0; x < fileWidth; x++) {
			int pixel = GetPixel8(x, y);
			if (pixel > max)
				max = pixel;
			else if (pixel < min)
				min = pixel;
		}
	}

	t0 = (min + max) / 2;
	t1 = t0;
	do {
		t0 = t1;
		float P0 = 0, P1 = 0;
		// P0 ponizej, P1 powyzej

		for (int i = 0; i < 256; i++) {
			if (i < t0)
				P0 += histogram[i];
			else
				P1 += histogram[i];
		}

		float u0 = 0, u1 = 0;

		for (int i = 0; i < t0; i++) {
			u0 += (i * histogram[i]) / P0;
		}
		for (int i = t0; i < 256; i++) {
			u1 += (i * histogram[i]) / P1;
		}

		t1 = (u1 + u0) / 2;

	} while (abs(t0 - t1) < 2.0f);

	ManualThreshold(t1);
	ShowHistogram(t1);
}


void myImage::GradientSegmentation() {
	CalculateHistogram(0, 0, fileWidth, fileHeight);

	int a = 0;
	int b = 0;
	int t = 0;
	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {
			if (x > 0 && x < 255 && y>0 && y < 255) {
				int GX = GetPixel8(x + 1, y) - GetPixel8(x - 1, y);
				int GY = GetPixel8(x, y + 1) - GetPixel8(x, y - 1);
				int G = max(abs(GX), abs(GY));
				a += GetPixel8(x, y) * G;
				b += G;
			}
		}
	}
	t = a / b;

	ManualThreshold(t);
	ShowHistogram(t);
}

int myImage::OtsuSegmentation() {
	CalculateHistogram(0, 0, fileWidth, fileHeight);

	float sigma[256] = { 0 };

	for (int t = 0; t < 256; t++) { // dla kazdego z progow 
		float P0 = 0;
		float P1 = 0;
		float u0 = 0;
		float u1 = 0;
		for (int i = 0; i < 256; i++) {
			if (i < t)
				P0 += histogram[i];
			else {
				P1 += histogram[i];
			}
		}

		for (int i = 0; i < t; i++) {
			u0 += (i * histogram[i]) / P0;
		}

		for (int i = t; i < 256; i++) {
			u1 += (i * histogram[i]) / P1;
		}

		float s0 = 0;
		float s1 = 0;
		for (int i = 0; i < t; i++) {
			s0 += ((i - u0) * (i - u0) * histogram[i]) / P0;
		}
		for (int i = t; i < 256; i++) {
			s1 += ((i - u1) * (i - u1) * histogram[i]) / P1;
		}
		sigma[t] = P0 * s0 + P1 * s1;
	}

	int t_star = 0;
	for (int t = 0; t < 256; t++) {
		if ((1 / sigma[t]) > (1 / sigma[t_star])) // jesli wartosc funkcji dla progu t jest wieksza od wartoscu dotychczasowego maxa
			t_star = t;
	}

	ShowHistogram(t_star);
	ManualThreshold(t_star);

	for (int i = 0; i < 256; i++) { // zamiast histogramu teraz bedzie rozklad funkcji wariancji
		histogram[i] = 1 / sigma[i];
	}

	
	return t_star; 
	
}

void myImage::LinearFilter(int mask[3][3], bool maskaWyostrzajaca) {

	bool minusInMask = false;

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			if (mask[j][i] < 0)
			{
				minusInMask = true;
			}
		}
	}

	int size = fileWidth * fileHeight;
	BYTE* newPixels = new BYTE[size]; // nowe kolory wszystkich pikseli

	for (int y = 1; y < fileHeight - 1; y++) {//
		for (int x = 1; x < fileWidth - 1; x++) { // pomin piksele na skraju
			float sumBrightness = 0;
			float sumK = 0;

			//przejdz po kazdym elemencie maski
			for (int i = -1; i < 2; i++) { // -1 0 1
				for (int j = -1; j < 2; j++) { // -1 0 1
					int pixel = GetPixel8(x - i, y - j);
					sumBrightness += pixel * mask[i + 1][j + 1];
					sumK += mask[i + 1][j + 1];
				}
			}

			if (minusInMask) {
				float newValue = sumBrightness;
				if (!maskaWyostrzajaca) {
					newValue += 127;
				}
				newPixels[y * fileWidth + x] = Floor(newValue);
			}
			else {
				newPixels[y * fileWidth + x] = Floor(sumBrightness / sumK);
			}

		}
	}

	for (int y = 1; y < fileHeight - 1; y++) {
		for (int x = 1; x < fileWidth - 1; x++  ) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}
	delete []newPixels;
}

void myImage::MedianFilter(int value, int option) { //option 0 3x3 option , option 1 3x3 krzyz
	int size = fileWidth * fileHeight;
	BYTE* newPixels = new BYTE[size]; // nowe kolory wszystkich pikseli
    int masksize = value * value;
	int* mask = new int[masksize];

	int maskOffset = value / 2;

	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {

			//przejdz po kazdym elemencie maski
			if (option == 0) { // jesli niekrzyziwa
				int counter = 0;
				for (int i = -maskOffset; i < maskOffset+1; i++) { 
					for (int j = -maskOffset; j <  maskOffset+1; j++) {
						int pixel;
						if (x - i < 0 || x - i >= fileWidth || y - j < 0 || y - j >= fileHeight) {
							pixel = GetPixel8(x, y);
						}
						else {
							pixel = GetPixel8(x - i, y - j);
						}
						mask[counter++] = pixel;
					}
				}
			}
			else if (option == 1) { // jesli krzyzowa
				int counter = 0;
				for (int i = -maskOffset; i < maskOffset+1; i++) { //pion od piksela
					int pixel;
					if(y - i < 0 || y - i >= fileHeight){
						pixel = GetPixel8(x, y);
					}
					else {
						pixel = GetPixel8(x, y-i);
					}
					mask[counter++] = pixel;
				}
				for (int i = -maskOffset; i < maskOffset+1; i++) {
					int pixel;
					if (x - i < 0 || x - i >= fileWidth) {
						pixel = GetPixel8(x, y);
					}
					else {
						pixel = GetPixel8(x-i, y); // poziom od piksela
					}
					mask[counter++] = pixel;
				}
			}

			for (int k = 0; k < masksize - 1; k++) { //bubble sort
				for (int l = 0; l < masksize - k; l++) {
					if (mask[l] > mask[l + 1]) {
						int tmp = mask[l + 1];
						mask[l + 1] = mask[l];
						mask[l] = tmp;
					}
				}
			}

			int middle = (int)masksize / 2;
			if (option == 1)
				middle = 2; // jak krzyzowa to jest mniej elementow w tej tablicy, srodkiem jest indeks2
			newPixels[y * fileWidth + x] = mask[middle];

		}
	}

	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}

	delete []newPixels;
}

void myImage::LogFilter(int value) {
	float sigma = value / 10.0f;


	int maskSize = (1 + 2 * ceil(2.5f * sigma));
	float** mask = new float* [maskSize];
	int maskOffset = maskSize / 2;

	for (int i = 0; i < maskSize; i++) {
		mask[i] = new float[maskSize];
	}
	float sum = 0;
	for (int i = 0; i < maskSize; i++) {
		for (int j = 0; j < maskSize; j++) {
			float a = (-1) *(i * i + j * j - (sigma * sigma)) / pow(sigma, 4);
			float expPow = (-1)*((i * i + j * j) / (2 * sigma * sigma));
			float expo = exp(expPow);
			mask[i][j] = a * expo;
			sum += mask[i][j];
		}
	}
	float b = sum;
	sum = 0;
	for (int i = 0; i < maskSize; i++) {
		for (int j = 0; j < maskSize; j++) {
			mask[i][j] /= mask[0][maskOffset] * (-1);
			sum += mask[i][j];
		}
	}
	b = sum;
	int size = fileWidth * fileHeight;
	int* intPixels = new int[size];


	for (int x = 0; x < fileWidth; x++){
		for (int y = 0; y < fileHeight; y++){

			float licznik = 0;
			for (int i = -maskOffset; i < maskOffset + 1; i++){
				for (int j = -maskOffset; j < maskOffset + 1; j++){

					int pixel;
					if (x - i < 0 || x - i >= fileWidth || y - j < 0 || y - j >= fileHeight)
						pixel = GetPixel8(x,y);
					else
						pixel = GetPixel8(x - i, y - j);
					licznik += (mask[maskOffset -i][maskOffset -j] * pixel);
				}
			}
			float newValue = licznik + 127;
			if (newValue < 0) newValue = 0;
			else newValue = 255;

			intPixels[y * fileWidth + x] = newValue;
		}
	}

	for (int x = 0; x < fileWidth; x++){
		for (int y = 0; y < fileHeight; y++){
			SetPixel8(x, y, intPixels[y * fileWidth + x]);
		}
	}
	delete []intPixels;
	for (int i = 0; i < maskSize; ++i)
		delete mask[i];
	delete[] mask;
}


//void Image::LogFilter(int value) {
//	float sigma = value / 10.0f;
//
//
//	int maskSize = (1 + 2 * ceil(2.5f * sigma));
//	float** mask = new float* [maskSize];
//	int maskOffset = maskSize / 2;
//
//	for (int i = 0; i < maskSize; i++) {
//		mask[i] = new float[maskSize];
//	}
//	float sum = 0;
//	for (int i = -maskOffset; i < maskOffset+1; i++) {
//		for (int j = -maskOffset; j < maskOffset+1; j++) {
//			float a = (-1) * (i * i + j * j - (sigma * sigma)) / pow(sigma, 4);
//			float expPow = (-1) * ((i * i + j * j) / (2 * sigma * sigma));
//			float expo = exp(expPow);
//			mask[maskOffset+i][maskOffset+j] = a * expo;
//			sum += mask[i+maskOffset][j+maskOffset];
//		}
//	}
//	//float S = sum; // sum of all
//	//float PS = 0; // sum of positives
//
//
//	//for (int i = 0; i < maskSize; i++) {
//	//	for (int j = 0; j < maskSize; j++) {
//	//		if (mask[i][j] > 0)
//	//			mask[i][j]  = mask[i][j]+ (mask[i][j] * S) / PS;
//	//	}
//	//}
//
//		//for (int i = 0; i < maskSize; i++) {
//	//	for (int j = 0; j < maskSize; j++) {
//	//		if (mask[i][j] > 0)
//	//			mask[i][j]  = mask[i][j]+ (mask[i][j] * S) / PS;
//	//	}
//	//}
//
//	int size = fileWidth * fileHeight;
//	int* intPixels = new int[size];
//
//
//	for (int x = 0; x < fileWidth; x++) {
//		for (int y = 0; y < fileHeight; y++) {
//
//			float licznik = 0;
//			for (int i = -maskOffset; i < maskOffset+1; i++) {
//				for (int j = -maskOffset; j < maskOffset+1; j++) {
//
//					int pixel;
//					if (x - i < 0 || x - i >= fileWidth || y - j < 0 || y - j >= fileHeight)
//						continue;
//					else
//						pixel = GetPixel8(x - i, y - j);
//					licznik += (mask[i+maskOffset][j+maskOffset] * pixel);
//				}
//			}
//			float newValue = licznik + 127*5;
//			if (newValue < 0) newValue = 0;
//			else newValue = 255;
//
//			intPixels[y * fileWidth + x] = newValue;
//		}
//	}
//
//	for (int x = 0; x < fileWidth; x++) {
//		for (int y = 0; y < fileHeight; y++) {
//			SetPixel8(x, y, intPixels[y * fileWidth + x]);
//		}
//	}
//	delete[]intPixels;
//	for (int i = 0; i < maskSize; i++)
//		delete mask[i];
//	delete[] mask;
//}

void myImage::Pavlidis(HDC context, CRect r) {
	int size = fileWidth * fileHeight;
	int* values = new int[size];
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			if (GetPixel8(x, y) == 0) values[y * fileWidth + x] = 1;
			else if (GetPixel8(x, y) == 255) values[y * fileWidth + x] = 0;
		}
	}

	bool loop = true;
	while (loop)
	{
		loop = false;
		for (int i = 0; i < 4; i++)
		{
			for (int x = 1; x < fileWidth - 1; x++) {
				for (int y = 1; y < fileHeight - 1; y++) {

					bool contin = false;
					if (values[y * fileWidth + x] == 1) {

						if (i == 0) {
							if (values[y * fileWidth + (x + 1)] == 0)
								contin = true;
						}
						else if (i == 1) {
							if (values[(y + 1) * fileWidth + x] == 0)
								contin = true;
						}
						else if (i == 2) {
							if (values[y * fileWidth + (x - 1)] == 0)
								contin = true;
						}
						else if (i == 3) {
							if (values[(y - 1) * fileWidth + x] == 0)
								contin = true;
						}
					}
					if (contin) {
						bool skeleton = false;

						//zero horizontal
						if (values[y * fileWidth + (x + 1)] == 0 && values[y * fileWidth + (x - 1)] == 0) {
							if ((values[(y + 1) * fileWidth + (x + 1)] != 0 || values[(y + 1) * fileWidth + (x - 1)] != 0 || values[(y + 1) * fileWidth + x] != 0) &&
								(values[(y - 1) * fileWidth + (x + 1)] != 0 || values[(y - 1) * fileWidth + (x - 1)] != 0 || values[(y - 1) * fileWidth + x] != 0))
								skeleton = true;
						}
						//zero vertical
						if (values[(y - 1) * fileWidth + x] == 0 && values[(y + 1) * fileWidth + x] == 0) {
							if ((values[(y + 1) * fileWidth + (x + 1)] != 0 || values[(y - 1) * fileWidth + (x + 1)] != 0 || values[(y)*fileWidth + (x + 1)] != 0) &&
								(values[(y + 1) * fileWidth + (x - 1)] != 0 || values[(y - 1) * fileWidth + (x - 1)] != 0 || values[(y)*fileWidth + (x - 1)] != 0))
								skeleton = true;
						}
						//0 degrees
						if (values[(y - 1) * fileWidth + (x - 1)] == 1 || values[(y - 1) * fileWidth + (x - 1)] == 2) { //1
							if (values[(y)*fileWidth + (x - 1)] == 0 && values[(y - 1) * fileWidth + (x)] == 0) {
								if (values[(y + 1) * fileWidth + (x - 1)] != 0 ||
									values[(y + 1) * fileWidth + (x)] != 0 ||
									values[(y + 1) * fileWidth + (x + 1)] != 0 ||
									values[(y)*fileWidth + (x + 1)] != 0 ||
									values[(y - 1) * fileWidth + (x + 1)] != 0) {
									skeleton = true;
								}
							}
						}
						//90 degrees
						if (values[(y - 1) * fileWidth + (x + 1)] == 1 || values[(y - 1) * fileWidth + (x + 1)] == 2) { //1
							if (values[(y)*fileWidth + (x + 1)] == 0 && values[(y - 1) * fileWidth + (x)] == 0) { //0
								if (values[(y + 1) * fileWidth + (x + 1)] != 0 ||
									values[(y + 1) * fileWidth + (x)] != 0 ||
									values[(y + 1) * fileWidth + (x - 1)] != 0 ||
									values[(y)*fileWidth + (x - 1)] != 0 ||
									values[(y - 1) * fileWidth + (x - 1)] != 0) {
									skeleton = true;
								}
							}
						}
						//180 degrees
						if (values[(y + 1) * fileWidth + (x + 1)] == 1 || values[(y + 1) * fileWidth + (x + 1)] == 2) { //1
							if (values[(y)*fileWidth + (x + 1)] == 0 && values[(y + 1) * fileWidth + (x)] == 0) { //0
								if (values[(y - 1) * fileWidth + (x + 1)] != 0 ||
									values[(y - 1) * fileWidth + (x)] != 0 || 
									values[(y - 1) * fileWidth + (x - 1)] != 0 ||
									values[(y)*fileWidth + (x - 1)] != 0 ||
									values[(y + 1) * fileWidth + (x - 1)] != 0) {
									skeleton = true;
								}
							}
						}
						//270 degrees
						if (values[(y + 1) * fileWidth + (x - 1)] == 1 || values[(y + 1) * fileWidth + (x - 1)] == 2) { //1
							if (values[(y)*fileWidth + (x - 1)] == 0 && values[(y + 1) * fileWidth + (x)] == 0) { //0
								if (values[(y - 1) * fileWidth + (x - 1)] != 0 ||
									values[(y - 1) * fileWidth + (x)] != 0 ||
									values[(y - 1) * fileWidth + (x + 1)] != 0 ||
									values[(y)*fileWidth + (x + 1)] != 0 ||
									values[(y + 1) * fileWidth + (x + 1)] != 0) {
									skeleton = true;
								}
							}
						}

						if (skeleton)
							values[y * fileWidth + x] = 2;
						else {
							values[y * fileWidth + x] = 3;
							loop = true;
						}
					}

				}
			}


			for (int x = 0; x < fileWidth; x++) {
				for (int y = 0; y < fileHeight; y++) {
					if (values[y * fileWidth + x] == 3) {
						SetPixel8(x, y, 255);
						values[y * fileWidth + x] = 0;
					}
				}
			}
			PaintDIB(context, r);
			Sleep(20);
		}
	}
	delete values;

}

void myImage::MapaOdleglosci(int t) {
	int size = fileWidth * fileHeight;
	int* values = new int[size];
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			if (GetPixel8(x, y) == 0)
				values[y * fileWidth + x] = 1;
			if (GetPixel8(x, y) == 255)
				values[y * fileWidth + x] = 0;
		}
	}
	int max = 0;
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			if (GetPixel8(x, y) == 0) {
				int maskSize = 1;
				bool found = false;
				while (!found) {
					for (int i = -maskSize; i < maskSize + 1; i++) {
						for (int j = -maskSize; j < maskSize + 1; j++) {
							if (y + j < 0 || x + i < 0 || y + j >= fileHeight || x + i >= fileWidth) continue;
							if (values[(y + j) * fileWidth + (x + i)] == 0) {
								values[y * fileWidth + x] = maskSize;
								found = true;
							}
						}
					}
					maskSize++;
				}
				if (maskSize > max)
					max = maskSize;
			}
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			int value = values[y * fileWidth + x];
			value = ((float)value / (float)max) * 255;
			SetPixel8(x, y, t>=(255 - value)?0:255);
			//SetPixel8(x, y, 255-value);
		}
	}

	delete values;
}

void myImage::Dylatacja() {
	int size = fileWidth * fileHeight;
	int* newValues = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			newValues[y * fileWidth + x] = 255; // ustaw na poczatku wszystko na biale
		}
	}
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			if (GetPixel8(x, y) != 0) {// jesli nie jest czescia obiektu to SKIP
				continue;
			}
			 
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {

					if (x + i <= 0 || x + i >= fileWidth || y + j <= 0 || y + j >= fileHeight)
						continue;
					newValues[(y + j) * fileWidth + (x + i)] = 0;

				}
			}
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newValues[y * fileWidth + x]);
		}
	}

	delete []newValues;
}

void myImage::Erozja() {
	int size = fileWidth * fileHeight;
	int* newValues = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			if (GetPixel8(x, y) != 0) {
				newValues[y * fileWidth + x] = 255;
				continue;
			}
			 // ta czesc wykona sie tylko dla piksela nalezacego do obiektu
			int toDelete = 0;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {

					if (x + i <= 0 || x + i >= fileWidth || y + j <= 0 || y + j >= fileHeight)
						continue;
					if (GetPixel8(x + i, y + j) != 0) // jesli piskel maski nie jest czescia obiektu to rozjasnij dany piksel
						toDelete++;
				}
			}
			if (toDelete > 0)
				newValues[y * fileWidth + x] = 255;
			else
				newValues[y * fileWidth + x] = 0;
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newValues[y * fileWidth + x]);
		}
	}

	delete []newValues;
}

void myImage::Otwarcie() {
	Erozja();
	Dylatacja();
}
void myImage::Zamkniecie() {
	Dylatacja();
	Erozja();
}

void myImage::KonturWewnetrzny() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];
	int* oldPixels = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			oldPixels[y * fileWidth + x] = GetPixel8(x, y);
		}
	}

	Erozja();


	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			if (oldPixels[y * fileWidth + x] < GetPixel8(x, y)) // jesli w starym obrazie dany piksel byl czarny a po erozji bialy, to ustaw ten piksel na czarny bo to znaczy ze byl wewnetrzna krawedzia.
				newPixels[y * fileWidth + x] = 0;
			else
				newPixels[y * fileWidth + x] = 255;
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}

	delete []newPixels;
	delete []oldPixels;
}

void myImage::KonturZewnetrzny() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];
	int* oldPixels = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			oldPixels[y * fileWidth + x] = GetPixel8(x, y);
		}
	}

	Dylatacja();

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			if (oldPixels[y * fileWidth + x] > GetPixel8(x, y))// jesli w starym obrazie dany piksel byl bialy a po dylatacji czarny, to ustaw ten piksel na czarny bo byl zewnetrzna krawedzia
				newPixels[y * fileWidth + x] = 0;
			else
				newPixels[y * fileWidth + x] = 255;
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}

	delete []newPixels;
	delete []oldPixels;
}

void myImage::Erozja8() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			newPixels[y * fileWidth + x] = 255;
		}
	}
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			int max = 0;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {

					if (x + i <= 0 || x + i >= fileWidth || y + j <= 0 || y + j >= fileHeight)
						continue;
					if (GetPixel8(x + i, y + j) > max)
						max = GetPixel8(x + i, y + j);
				}
			}
			newPixels[y * fileWidth + x] = max;
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}

	delete []newPixels;
}

void myImage::Dylatacja8() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			newPixels[y * fileWidth + x] = 255;
		}
	}
	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			int min = 255;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {

					if (x + i <= 0 || x + i >= fileWidth || y + j <= 0 || y + j >= fileHeight)
						continue;
					if (GetPixel8(x + i, y + j) < min)
						min = GetPixel8(x + i, y + j);
				}
			}
			newPixels[y * fileWidth + x] = min;
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}

	delete[]newPixels;
}

void myImage::Zamkniecie8() {
	Dylatacja8();
	Erozja8();
}
void myImage::Otwarcie8() {
	Erozja8();
	Dylatacja8();
}

void myImage::WhiteTopHat() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];
	int* oldPixels = new int[size];
	int a = 5;

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			oldPixels[y * fileWidth + x] = GetPixel8(x, y);
		}
	}

	Zamkniecie8();


	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

			//if (oldPixels[y * fileWidth + x] > GetPixel8(x, y))
				newPixels[y * fileWidth + x] = oldPixels[y * fileWidth + x] - GetPixel8(x, y);
		//	else
			//	newPixels[y * fileWidth + x] = GetPixel8(x, y) - oldPixels[y * fileWidth + x];
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, Floor(newPixels[y * fileWidth + x]*a));
		}
	}

	delete[] newPixels;
	delete[] oldPixels;
}

void myImage::BlackTopHat() {
	int size = fileWidth * fileHeight;
	int* newPixels = new int[size];
	int* oldPixels = new int[size];
	int a = 5;

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			oldPixels[y * fileWidth + x] = GetPixel8(x, y);
		}
	}

	Otwarcie8();


	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {

		//	if (oldPixels[y * fileWidth + x] < GetPixel8(x, y))
				newPixels[y * fileWidth + x] = GetPixel8(x, y) - oldPixels[y * fileWidth + x];
		//	else
		//		newPixels[y * fileWidth + x] = oldPixels[y * fileWidth + x] - GetPixel8(x, y);
		}
	}

	for (int x = 0; x < fileWidth; x++) {
		for (int y = 0; y < fileHeight; y++) {
			SetPixel8(x, y, Floor(newPixels[y * fileWidth + x] * a));
		}
	}

	delete []newPixels;
	delete []oldPixels;
}

# define M_PI           3.14159265358979323846  /* pi */

void myImage::HoughWykres(int step) {

	float maxRoHeight = sqrt(pow(fileWidth / 2, 2) + pow(fileHeight / 2, 2));

	float ro = 0.0;

	int widthOfAkumulator = 360;
	int heightOfAkumulator = int(maxRoHeight) + 1;

	int** akumulator = new int* [widthOfAkumulator];
	for (int i = 0; i < widthOfAkumulator; i++) {
		akumulator[i] = new int[heightOfAkumulator];
	}

	for (int i = 0; i < widthOfAkumulator; i++) {
		for (int j = 0; j < heightOfAkumulator; j++) {
			akumulator[i][j] = 0;
		}
	}

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			if (GetPixel8(x, y) == 0)
			{
				int yi = y - fileHeight / 2; 
				int xi = x - fileWidth / 2;

				for (int deg = 0; deg < 360; deg += step)
				{
					ro = xi * cos((M_PI / 180) * deg) + yi * sin((M_PI / 180) * deg);

					if (ro < 0)
						continue;

					akumulator[deg][int(ro)]++; 
				}
			}
		}
	}

	int maxPrzeciecia = 0;
	int maxDeg = 0;
	int maxRo = 0;

	for (int i = 0; i < widthOfAkumulator; i++) // wyznacz maksymalna wartosc w akumulatorze
	{
		for (int j = 0; j < heightOfAkumulator; j++)
		{
			int value = akumulator[i][j];

			if (value > maxPrzeciecia)
			{
				maxPrzeciecia = value;
				maxDeg = i;
				maxRo = j;
			}
		}
	}
	int color = 255;
	if (maxPrzeciecia != 0) {
		color = 255 / maxPrzeciecia;
	}
	else {
		color = 255;
	}

	CreateGreyscaleDIBWhite(new CRect(0, 0, widthOfAkumulator, heightOfAkumulator), 0, 0); // stworz pusty bialy obraz o rozmiarach akumulatora

	for (int i = 0; i < widthOfAkumulator; i += step) // rysuj wykres z przecieciami
	{
		for (int j = 0; j < heightOfAkumulator; j++)
		{
			int przeciecia = akumulator[i][j];

			if (przeciecia > 0)
			{
				for (int k = 0; k < step; k++)
				{
					if (i+k <= 0 || i+k >= fileWidth || j <= 0 || j >= fileHeight)
						continue;
					SetPixel8(i+k , j, Floor(255 - color*przeciecia-5)); // im wieksza wartosc w akumulatorze tym ciemniejsze
				}
			}
			else
			{
				for (int k = 0; k < step; k++)
				{
					if (i + k <= 0 || i + k >= fileWidth || j <= 0 || j >= fileHeight)
						continue;
					SetPixel8(i + k, j, 255);
				}
			}
		}
	}

	for (int i = 0; i < widthOfAkumulator; ++i)
		delete akumulator[i];
	delete[] akumulator;
}

void myImage::Hough(int step, int parameter) { // parameter -> ile linii narysowac / ile krawedzi wykrywac
	float maxRoHeight = sqrt(pow(fileWidth / 2, 2) + pow(fileHeight / 2, 2));

	float ro = 0.0;

	int widthOfAkumulator = 360;
	int heightOfAkumulator = int(maxRoHeight) + 1;

	int** akumulator = new int* [widthOfAkumulator];
	for (int i = 0; i < widthOfAkumulator; i++) {
		akumulator[i] = new int[heightOfAkumulator];
	}

	for (int i = 0; i < widthOfAkumulator; i++) {
		for (int j = 0; j < heightOfAkumulator; j++) {
			akumulator[i][j] = 0;
		}
	}
	
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			if (GetPixel8(x, y) == 0)
			{
				int yi = y - fileHeight / 2;
				int xi = x - fileWidth / 2;

				for (int deg = 0; deg < 360; deg += step)
				{
					ro = xi * cos((M_PI / 180) * deg) + yi * sin((M_PI / 180) * deg);

					if (ro < 0)
						continue;

					akumulator[deg][int(ro)]++;
				}
			}
		}
	}

	vector<int> tabMaxPrzeciecia;
	vector<int> tabMaxDeg;
	vector<int> tabMaxRo;

	for (int p = 0; p < parameter; p++) {
		int maxPrzeciecia = 0;
		int maxDeg = 0;
		int maxRo = 0;
		int counter = 0;

		for (int i = 0; i < widthOfAkumulator; i++) // wyznacz maksymalna wartosc w akumulatorze 
		{
			for (int j = 0; j < heightOfAkumulator; j++)
			{
				int przeciecia = akumulator[i][j];

				if (przeciecia > maxPrzeciecia)
				{
					maxPrzeciecia = przeciecia; // najwieksza ilosc przeciec
					maxDeg = i; // wartosc kata theta dla max wartosci
					maxRo = j; // wartosc ro dla tego kata
					counter++;
				}
			}
		}
		tabMaxPrzeciecia.push_back(maxPrzeciecia);
		tabMaxDeg.push_back(maxDeg);
		tabMaxRo.push_back(maxRo);
		//akumulator[maxDeg][maxRo] = 0;
		// zeruj otoczenie wyznaczonego maxa tak dlugo dopoki nie natrafimy na piksel ktorego wartosc jest mniejsza niz 1/10 maxa.
		int maskSize = 1;
		bool found = false;
		while (!found) {
			for (int i = -maskSize; i < maskSize + 1; i++) {
				for (int j = -maskSize; j < maskSize + 1; j++) {
					if (maxDeg + i < 0 || maxDeg + i >= widthOfAkumulator || maxRo + j < 0 || maxRo + j >= heightOfAkumulator) {
						continue;
					}
					akumulator[maxDeg + i][maxRo + j] = 0;
					if (akumulator[maxDeg + i][maxRo + j] < maxPrzeciecia / 10 || maskSize >= 100) {
						found = true;
					}
				}
			}
			maskSize++;
		}
	}
	float offset = 0.5; // jak grube ma byc zaznaczenie linii


	for (int p = 0; p < parameter; p++) {

		for (int y = 0; y < fileHeight; y++)
		{
			for (int x = 0; x < fileWidth; x++)
			{
				int yi = y - fileHeight / 2;
				int xi = x - fileWidth / 2;

				float r = (xi * cos((M_PI / 180) * tabMaxDeg[p]) + yi * sin((M_PI / 180) * tabMaxDeg[p]));

				if (r > 0)
				{
					if (r >= tabMaxRo[p] - offset && r <= tabMaxRo[p] + offset)
					{
						SetPixel8(x, y, 150); // zaznacz na szaro zeby bylo widoczne
					}
				}
			}
		}
	}

	for (int i = 0; i < widthOfAkumulator; ++i)
		delete akumulator[i];
	delete[] akumulator;
}

//void Image::Hough(int step, int parameter) { // parameter -> ile linii narysowac / ile krawedzi wykrywac // hough z automatycznym wykrywaniem krawedzi
//	float maxRoHeight = sqrt(pow(fileWidth / 2, 2) + pow(fileHeight / 2, 2));
//
//	float ro = 0.0;
//
//	int widthOfAkumulator = 360;
//	int heightOfAkumulator = int(maxRoHeight) + 1;
//
//	int** akumulator = new int* [widthOfAkumulator];
//	for (int i = 0; i < widthOfAkumulator; i++) {
//		akumulator[i] = new int[heightOfAkumulator];
//	}
//
//	for (int i = 0; i < widthOfAkumulator; i++) {
//		for (int j = 0; j < heightOfAkumulator; j++) {
//			akumulator[i][j] = 0;
//		}
//	}
//
//	for (int y = 0; y < fileHeight; y++)
//	{
//		for (int x = 0; x < fileWidth; x++)
//		{
//			if (GetPixel8(x, y) == 0)
//			{
//				int yi = y - fileHeight / 2;
//				int xi = x - fileWidth / 2;
//
//				for (int deg = 0; deg < 360; deg += step)
//				{
//					ro = xi * cos((M_PI / 180) * deg) + yi * sin((M_PI / 180) * deg);
//
//					if (ro < 0)
//						continue;
//
//					akumulator[deg][int(ro)]++;
//				}
//			}
//		}
//	}
//
//	vector<int> tabMaxPrzeciecia;
//	vector<int> tabMaxDeg;
//	vector<int> tabMaxRo;
//	int a = 0;
//	int counterPetla = 0;
//	int maxGlobal = 0;
//	int counter = 0;
//	do{
//	//for (int p = 0; p < parameter; p++) {
//		int maxPrzeciecia = 0;
//		int maxDeg = 0;
//		int maxRo = 0;
//
//		for (int i = 0; i < widthOfAkumulator; i++) // wyznacz maksymalna wartosc w akumulatorze 
//		{
//			for (int j = 0; j < heightOfAkumulator; j++)
//			{
//				int przeciecia = akumulator[i][j];
//
//				if (przeciecia > maxPrzeciecia)
//				{
//					
//					a = przeciecia;
//					maxPrzeciecia = przeciecia; // najwieksza ilosc przeciec
//					maxDeg = i; // wartosc kata theta dla max wartosci
//					maxRo = j; // wartosc ro dla tego kata
//				}
//			}
//		}
//		if (counter == 0) { maxGlobal = maxPrzeciecia; }
//		counter++;
//		tabMaxPrzeciecia.push_back(maxPrzeciecia);
//		tabMaxDeg.push_back(maxDeg);
//		tabMaxRo.push_back(maxRo);
//		//akumulator[maxDeg][maxRo] = 0;
//		// zeruj otoczenie wyznaczonego maxa tak dlugo dopoki nie natrafimy na piksel ktorego wartosc jest mniejsza niz 1/10 maxa.
//		int maskSize = 1;
//		bool found = false;
//		while (!found) {
//			for (int i = -maskSize; i < maskSize + 1; i++) {
//				for (int j = -maskSize; j < maskSize + 1; j++) {
//					if (maxDeg + i < 0 || maxDeg + i >= widthOfAkumulator || maxRo + j < 0 || maxRo + j >= heightOfAkumulator) {
//						continue;
//					}
//					akumulator[maxDeg + i][maxRo + j] = 0;
//					if (akumulator[maxDeg + i][maxRo + j] < maxPrzeciecia / 10) {
//						found = true;
//					}
//				}
//			}
//			maskSize++;
//		}
//		counterPetla++;
//	} while (a >= maxGlobal / 2 );
//	float offset = 0.5; // jak grube ma byc zaznaczenie linii
//
//
//	for (int p = 0; p < counterPetla; p++) {
//
//		for (int y = 0; y < fileHeight; y++)
//		{
//			for (int x = 0; x < fileWidth; x++)
//			{
//				int yi = y - fileHeight / 2;
//				int xi = x - fileWidth / 2;
//
//				float r = (xi * cos((M_PI / 180) * tabMaxDeg[p]) + yi * sin((M_PI / 180) * tabMaxDeg[p]));
//
//				if (r > 0)
//				{
//					if (r >= tabMaxRo[p] - offset && r <= tabMaxRo[p] + offset)
//					{
//						SetPixel8(x, y, 150); // zaznacz na szaro zeby bylo widoczne
//					}
//				}
//			}
//		}
//	}
//
//	for (int i = 0; i < widthOfAkumulator; ++i)
//		delete akumulator[i];
//	delete[] akumulator;
//}

void myImage::Hamming()
{
	float alpha = 0.53836f;
	float beta = 1 - alpha;

	float N = sqrt(fileWidth*fileWidth + fileHeight*fileHeight);

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			int xi = x - fileWidth / 2;
			int yi = y - fileHeight / 2;


			float r = sqrt(xi*xi + yi*yi);
			float n = N / 2 - r;

			float w = alpha - beta * cos((2 * M_PI * n) / (N - 1));

			byte pixel = GetPixel8(x, y);

			SetPixel8(x, y, Floor(w * pixel));
		}
	}
}

void myImage::FourierAmplituda()
{
	Hamming();
	double pixel;
    typedef pair<int, int> uv;
	typedef pair<double, double> ComplexNumber;
	double value = 0;
	double valueReal = 0.0;
	double valueImagined = 0.0;
	double result;
	map<uv, ComplexNumber> fourier;
	map<uv, ComplexNumber> fourierReordered;

	for (int v = 0; v < fileHeight; v++)
	{
		for (int u = 0; u < fileWidth; u++)
		{
			valueReal = 0;
			valueImagined = 0;

			for (int y = 0; y < fileHeight; y++)
			{
				for (int x = 0; x < fileWidth; x++)
				{
					pixel = GetPixel8(x, y);
					result = 2 * M_PI * (float)((float)(u * x) / (float)fileWidth + (float)(v * y) / (float)fileHeight);
					valueReal += (pixel * cos(result));
					valueImagined -= (pixel * sin(result));
				}
			}

			uv uv(u, v);
			ComplexNumber number(valueReal, valueImagined);
			fourier[uv] = number;
		}
	}

	double w = fileWidth / 2;
	double h = fileHeight / 2;

	for (int y = 0; y < fileHeight; y++) // PRZENOSZENIE CWIARTEK
	{
		for (int x = 0; x < fileWidth; x++)
		{
			uv uv1(x, y);
			uv uv2;

			if (x < w && y < h) //A
			{
				uv2= uv(x + w, y + h); 
			}
			else if (x >= w && y < h) //  B
			{
				uv2 = uv(x - w, y + h);
			}
			else if (x < w && y >= h) //C
			{
				uv2 = uv(x + w, y - h);
			}
			else if (x >= w && y >= h) // D
			{
				uv2 = uv(x - w, y - h);
			}
		

			fourierReordered[uv2] = fourier[uv1];
		}
	}

	vector <double> v;
	int max = -99999;
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			uv uv(x, y);
			double real = fourierReordered[uv].first;
			double imag = fourierReordered[uv].second;
			value = abs(sqrt(real * real + imag * imag)); // modul z 
			value = log(1 + value);
			if (value > max) {
				max = value;
			}
			v.push_back(value);
		}
	}
	int counter = 0;
	for(int y = 0; y < fileHeight; y++){
		for(int x = 0; x<fileWidth; x++){

			value = v[counter++]*255/max; // skalowanie

			SetPixel8(x, y, Floor((float)value));
		}
	}
}

//void myImage::FourierAmplituda()
//{
//	Hamming();
//	double pixel;
//	typedef pair<int, int> uv;
//	typedef pair<double, double> ComplexNumber;
//	double value = 0;
//	double valueReal = 0.0;
//	double valueImagined = 0.0;
//	double result;
//	map<uv, ComplexNumber> fourier;
//	map<uv, ComplexNumber> fourierReordered;
//
//
//	int w = fileWidth / 2;
//	int h = fileHeight / 2;
//	
//	int tab[200][200];
//
//	for (int y = 0; y < fileHeight; y++) // PRZENOSZENIE CWIARTEK
//	{
//		for (int x = 0; x < fileWidth; x++)
//		{
//			tab[x][y] = GetPixel8(x, y);
//		}
//	}
//
//
//
//	for (int y = 0; y < fileHeight; y++) // PRZENOSZENIE CWIARTEK
//	{
//		for (int x = 0; x < fileWidth; x++)
//		{
//
//			if (x < w && y < h) //A
//			{
//
//				SetPixel8(x + w, y + h, tab[x][y]);
//			}
//			if (x >= w && y < h) //  B
//			{
//				SetPixel8(x - w, y + h, tab[x][y]);
//			}
//			if (x < w && y >= h) //C
//			{
//				SetPixel8(x + w, y - h, tab[x][y]);
//			}
//			if (x >= w && y >= h) // D
//			{
//				SetPixel8(x - w, y - h, tab[x][y]);
//			}
//		}
//	}
//
//	for (int v = 0; v < fileHeight; v++)
//	{
//		for (int u = 0; u < fileWidth; u++)
//		{
//			valueReal = 0;
//			valueImagined = 0;
//
//			for (int y = 0; y < fileHeight; y++)
//			{
//				for (int x = 0; x < fileWidth; x++)
//				{
//					pixel = GetPixel8(x, y);
//					result = 2 * M_PI * (float)((float)(u * x) / (float)fileWidth + (float)(v * y) / (float)fileHeight);
//					valueReal += (pixel * cos(result));
//					valueImagined -= (pixel * sin(result));
//				}
//			}
//
//			uv uv(u, v);
//			ComplexNumber number(valueReal, valueImagined);
//			fourier[uv] = number;
//		}
//	}
//
//
//	
//
//	vector <double> v;
//	int max = -99999;
//	for (int y = 0; y < fileHeight; y++)
//	{
//		for (int x = 0; x < fileWidth; x++)
//		{
//			uv uv(x, y);
//			double real = fourier[uv].first;
//			double imag = fourier[uv].second;
//			value = abs(sqrt(real * real + imag * imag)); // modul z 
//			value = log(1 + value);
//			if (value > max) {
//				max = value;
//			}
//			v.push_back(value);
//		}
//	}
//	int counter = 0;
//	for (int y = 0; y < fileHeight; y++) {
//		for (int x = 0; x < fileWidth; x++) {
//
//			value = v[counter++] * 255 / max; // skalowanie
//
//			SetPixel8(x, y, Floor((float)value));
//		}
//	}
//}
#include<complex>
void myImage::FourierFaza()
{
	Hamming();
	double pixel;
	typedef pair<int, int> uv;
	typedef pair<double, double> ComplexNumber;
	double value = 0;
	double valueReal = 0.0;
	double valueImagined = 0.0;
	double result;
	map<uv, ComplexNumber> fourier;
	map<uv, ComplexNumber> fourierReordered;

	for (int v = 0; v < fileHeight; v++)
	{
		for (int u = 0; u < fileWidth; u++)
		{
			valueReal = 0;
			valueImagined = 0;

			for (int y = 0; y < fileHeight; y++)
			{
				for (int x = 0; x < fileWidth; x++)
				{
					result = 2 * M_PI * (float)((float)(u * x) / (float)fileWidth + (float)(v * y) / (float)fileHeight);
					pixel = GetPixel8(x, y);
					valueReal += (pixel * (cos(result)));
					valueImagined -= (pixel * sin(result));
				}
			}
			uv uv(u, v);
			ComplexNumber number(valueReal, valueImagined);
			fourier[uv] = number;
		}
	}

	double w = fileWidth / 2;
	double h = fileHeight / 2;

	for (int y = 0; y < fileHeight; y++) // PRZENOSZENIE CWIARTEK
	{
		for (int x = 0; x < fileWidth; x++)
		{
			uv uv1(x, y);
			uv uv2;

			if (x < w && y < h)
			{
				uv2 = uv(x + w, y + h);
			}
			if (x >= w && y < h)
			{
				uv2 = uv(x - w, y + h);
			}
			if (x >= w && y >= h)
			{
				uv2 = uv(x - w, y - h);
			}
			if (x < w && y >= h)
			{
				uv2 = uv(x + w, y - h);
			}

			fourierReordered[uv2] = fourier[uv1];
		}
	}

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			uv uv(x, y);
			double real = fourierReordered[uv].first;
			double imagined = fourierReordered[uv].second;

			//std::complex<double> complexNumber(real, imagined);
			//value = arg(complexNumber);

			//if (real < 0.0001 && real >0)
			//	real = M_PI / 2;
			//if (real > -0.0001 && real < 0)
			//	real = -M_PI / 2;

			double tangensValue = imagined / real;
			//if (tangensValue > 0) {
			value = atan(tangensValue);
			//}
			//else {
			//	value = atan(tangensValue) + M_PI;
			//}

			//if (real < 0.001 && imagined >0)
			//	value = M_PI / 2;
			//else if (real < 0.001 && imagined <0)
			//	value = -M_PI / 2;
			if (real < 0.0001 && real >0)
				value = M_PI / 2;
			if (real > -0.0001 && real < 0)
				value = -M_PI / 2;

			//value = ((value+M_PI/2) * 255 / 3); 
			value = ((value + (M_PI / 2)) / M_PI)*255;

			SetPixel8(x, y, Floor((float)value));
		}
	}
}