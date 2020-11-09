#include "stdafx.h"
#include "Image.h"
#include "Math.h"
#include "HistogramWindow.h"
using namespace std;

Image::Image()
{
}

Image::~Image()
{
}

bool Image::LoadDIB(CString sciezka_do_pliku) { // wczytaj Image
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

bool Image::PaintDIB(HDC kontekst, CRect r)
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
bool Image::GetPixel1(int x, int y)
{
	long int index = y * ((fileWidth + 31) / 32) * 4 + x / 8;
	BYTE byte = ((BYTE*)ImageBajty)[index];
	bool bit = byte & (0x80 >> (x % 8));

	return bit;
}
//do odczytywania wartoœci pikseli w bitmapach 1 - bitowych,
BYTE Image::GetPixel8(int x, int y)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	return ((BYTE*)ImageBajty)[index];
}
//do odczytywania wartoœci pikseli w bitmapach 8 - bitowych,
RGBTRIPLE Image::GetPixel24(int x, int y)
{
	int index = y * ((24 * fileWidth + 31) / 32) * 4 + 3 * x;
	BYTE* color = (BYTE*)ImageBajty + index;
	return ((RGBTRIPLE*)color)[0];
}
//do odczytywania wartoœci pikseli w bitmapach 24 - bitowych,

bool Image::SetPixel8(int x, int y, BYTE val)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	((BYTE*)ImageBajty)[index] = val;
	return true;
}

bool Image::CreateGreyscaleDIB(CRect rozmiar_Imageu, int xPPM, int yPPM)
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

bool Image::SaveDIB(CString sciezka_do_pliku)
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

void Image::ChangeBrightness(float value) {
	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {
			SetPixel8(x, y, Floor(GetPixel8(x, y) + value));
		}
	}


}
void Image::ChangeContrast(float value)
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

void Image::ChangeExponent(float value) {
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

void Image::Negative() {
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			SetPixel8(x, y, Floor(255-GetPixel8(x,y)));
		}
	}
}

void Image::CalculateHistogram(int sX, int sY, int eX, int eY) {
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

void Image::ShowHistogram(int threshold) { // wyswietl histogram
	HistogramWindow dialog;
	dialog.values = histogram;
	if (threshold >= 0 && threshold < 256)
		dialog.threshold = threshold;
	dialog.DoModal();
}

void Image::EqualizeHistogram() {
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

void Image::ManualThreshold(int t)
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

void Image::IterativeSegmentation() {
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


void Image::GradientSegmentation() {
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

int Image::OtsuSegmentation() {
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

void Image::LinearFilter(int mask[3][3], bool maskaWyostrzajaca) {

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

void Image::MedianFilter(int value, int option) { //option 0 3x3 option , option 1 3x3 krzyz
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
						BYTE pixel;
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
					BYTE pixel;
					if(y - i < 0 || y - i >= fileHeight){
						pixel = GetPixel8(x, y);
					}
					else {
						pixel = GetPixel8(x, y-i);
					}
					mask[counter++] = pixel;
				}
				for (int i = -maskOffset; i < maskOffset+1; i++) {
					BYTE pixel;
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

void Image::LogFilter(int value) {
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
	float* newPixels = new float[size];


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

			newPixels[y * fileWidth + x] = newValue;
		}
	}

	for (int x = 0; x < fileWidth; x++){
		for (int y = 0; y < fileHeight; y++){
			SetPixel8(x, y, newPixels[y * fileWidth + x]);
		}
	}
	delete []newPixels;
	for (int i = 0; i < maskSize; ++i)
		delete mask[i];
		delete[] mask;
}

