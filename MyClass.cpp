#include "stdafx.h"
#include "MyClass.h"
#include "Math.h"
#include "HistogramDialog.h"
using namespace std;

MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}

bool MyClass::LoadDIB(CString sciezka_do_pliku) { // wczytaj obraz
	CFile f;
	header;
	f.Open(sciezka_do_pliku, CFile::modeReadWrite);
	f.Read(&header, sizeof(BITMAPFILEHEADER));

	sizeOfFile = header.bfSize - sizeof(BITMAPFILEHEADER);

	obrazInfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeOfFile);
	f.Read(obrazInfo, sizeOfFile);
	obrazBajty = (BYTE*)obrazInfo + header.bfOffBits - sizeof(BITMAPFILEHEADER);

	BITMAPINFOHEADER* info = (BITMAPINFOHEADER*)obrazInfo;

	fileWidth = info->biWidth;
	fileHeight = info->biHeight;
	typeOfImage = info->biBitCount;

	f.Close();
	return true;
}

bool MyClass::PaintDIB(HDC kontekst, CRect r)
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
		obrazBajty,
		(BITMAPINFO*)obrazInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	return true;
}

//do wyœwietlania wczytanych obrazów,
bool MyClass::GetPixel1(int x, int y)
{
	long int index = y * ((fileWidth + 31) / 32) * 4 + x / 8;
	BYTE byte = ((BYTE*)obrazBajty)[index];
	bool bit = byte & (0x80 >> (x % 8));

	return bit;
}
//do odczytywania wartoœci pikseli w bitmapach 1 - bitowych,
BYTE MyClass::GetPixel8(int x, int y)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	return ((BYTE*)obrazBajty)[index];
}
//do odczytywania wartoœci pikseli w bitmapach 8 - bitowych,
RGBTRIPLE MyClass::GetPixel24(int x, int y)
{
	int index = y * ((24 * fileWidth + 31) / 32) * 4 + 3 * x;
	BYTE* color = (BYTE*)obrazBajty + index;
	return ((RGBTRIPLE*)color)[0];
}
//do odczytywania wartoœci pikseli w bitmapach 24 - bitowych,

bool MyClass::SetPixel8(int x, int y, BYTE val)
{
	int index = y * ((8 * fileWidth + 31) / 32) * 4 + x;
	((BYTE*)obrazBajty)[index] = val;
	return true;
}

bool MyClass::CreateGreyscaleDIB(CRect rozmiar_obrazu, int xPPM, int yPPM)
{
	int rozmiar_palety = 256 * sizeof(RGBQUAD);

	int szerokosc = fileWidth;
	int wysokosc = fileHeight;
	int liczba_pixeli = szerokosc * wysokosc;

	int rozmiar_tablicy_pixeli = (((szerokosc * 8 + 31) / 32) * 4) * wysokosc;
	DWORD rozmiar_pamieci = sizeof(BITMAPINFOHEADER) + rozmiar_palety + rozmiar_tablicy_pixeli;
	LPVOID obraz = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, rozmiar_pamieci);

	BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)obraz;
	info_header->biBitCount = 8;
	info_header->biHeight = wysokosc;
	info_header->biWidth = szerokosc;
	info_header->biSize = sizeof(BITMAPINFOHEADER);
	info_header->biPlanes = 1;
	info_header->biCompression = BI_RGB;
	info_header->biSizeImage = rozmiar_tablicy_pixeli;
	info_header->biXPelsPerMeter = ((BITMAPINFOHEADER*)obrazInfo)->biXPelsPerMeter;
	info_header->biYPelsPerMeter = ((BITMAPINFOHEADER*)obrazInfo)->biYPelsPerMeter;
	info_header->biClrUsed = 0;
	info_header->biClrImportant = 0;

	RGBQUAD* paleta_kolorow = (RGBQUAD*)((BYTE*)obraz + info_header->biSize);

	RGBQUAD kolor;
	for (int i = 0; i < 256; i++)
	{
		kolor.rgbBlue = i;
		kolor.rgbGreen = i;
		kolor.rgbRed = i;
		kolor.rgbReserved = 0;
		paleta_kolorow[i] = kolor;
	}

	BITMAPINFO* bitmap_info = (BITMAPINFO*)obraz;

	BYTE* tablica_pixeli = (BYTE*)paleta_kolorow + rozmiar_palety;

	int bitMapType = ((BITMAPINFOHEADER*)obrazInfo)->biBitCount;

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
	obrazInfo = bitmap_info;
	obrazKolory = paleta_kolorow;
	obrazBajty = tablica_pixeli;

	sizeOfFile = header.bfSize - sizeof(BITMAPFILEHEADER);


	return true;
}

bool MyClass::SaveDIB(CString sciezka_do_pliku)
{
	CFile f;
	f.Open(sciezka_do_pliku, CFile::modeCreate);
	f.Close();
	f.Open(sciezka_do_pliku, CFile::modeWrite);

	f.Write(&header, sizeof(BITMAPFILEHEADER));
	f.Write(obrazInfo, sizeOfFile);

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

void MyClass::ChangeBrightness(float value) {
	for (int y = 0; y < fileHeight; y++) {
		for (int x = 0; x < fileWidth; x++) {
			SetPixel8(x, y, Floor(GetPixel8(x, y) + value));
		}
	}


}
void MyClass::ChangeKontrast(float value)
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

void MyClass::ChangePotega(float value) {
	float newValue = value /= 10.0;

	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			SetPixel8(x, y, Floor((float)pow((GetPixel8(x, y)), newValue)));
		}
	}
}

void MyClass::Negatyw() {
	for (int y = 0; y < fileHeight; y++)
	{
		for (int x = 0; x < fileWidth; x++)
		{
			SetPixel8(x, y, Floor(255-GetPixel8(x,y)));
		}
	}
}

void MyClass::CalculateHistogram(int startX, int startY, int endX, int endY) {
	histogram = new float[256];
	for (int x = 0; x < 256; x++)
		histogram[x] = 0;
	int width = endX - startX;
	int height = endY - startY;
	int histogramCount = width * height;

	for (int x = startX; x < endX; x++)
	{
		for (int y = startY; y < endY; y++)
		{
			int byteValue = GetPixel8(x, y);
			histogram[byteValue]++;
		}
	}
	for (int x = 0; x < 256; x++)
		histogram[x] = histogram[x] / histogramCount;
}

void MyClass::ShowHistogram() {
	HistogramDialog dialog;
	dialog.values = histogram;
	dialog.DoModal();
}