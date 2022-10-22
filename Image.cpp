#include "Image.h"
#include <stdio.h>
#include<string>
#include <algorithm>

#define PI 3.1415926535
Image::~Image()
{

}
Image::Image(string fileName)
{
	switch (getExtension(fileName))
	{
	case Ext::BMP:
		if (!BitmapRead(fileName))
		{
			printf("Reading failed!");
		}
		break;
	case Ext::FIPD:
		if (!FIPDRead(fileName))
		{
			printf("Reading failed!");
		}
		break;
	}
}
Image::Ext Image::getExtension(string fileName)
{
	int dotIndex = fileName.find_last_of(".");
	string str = fileName.substr(dotIndex);

	if (str == ".bmp" || str == ".BMP")
	{
		return Image::Ext::BMP;
	}
	else if (str == ".fipd" || str == ".FIPD")
	{
		return Image::Ext::FIPD;
	}
	else if (str == ".png" || str == ".PNG")
	{
		return Image::Ext::PNG;
	}
	else if (str == ".jpeg" || str == ".JPEG" || str == ".jpg" || str == ".JPG")
	{
		return Image::Ext::JPEG;
	}
	else if (str == ".gif" || str == ".GIF")
	{
		return Image::Ext::GIF;
	}
	else if (str == ".tif" || str == ".TIF" || str == ".tiff" || str == ".TIFF")
	{
		return Image::Ext::TIFF;
	}
}
bool Image::Save(string fileName)
{
	switch (getExtension(fileName))
	{
	case Ext::BMP:
		return BitmapSave(fileName);
		break;
	case Ext::FIPD:
		return FIPDSave(fileName);
	}
}
bool Image::FIPDRead(string fileName)
{
	return true;
}
bool Image::FIPDSave(string fileName)
{
	return true;
}
bool Image::BitmapRead(string fileName)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	int bitCount;
	FILE* file = fopen(fileName.c_str(), "rb");

	if (file == 0)
	{
		return false;
	}

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	XPixelsPerMeter = infoHeader.biXPelsPerMeter;
	YPixelsPerMeter = infoHeader.biYPelsPerMeter;
	bitCount = infoHeader.biBitCount;

	int lineByte = (width * bitCount / 8 + 3) / 4 * 4;
	int skip = lineByte - width * bitCount / 8;
	LPRGBQUAD pRGBQuad = new RGBQUAD[256];
	switch (bitCount)
	{
	case 1:		//2色

		break;
	case 8:		//256色

		fread(pRGBQuad, sizeof(RGBQUAD), 256, file);

		redData = Matrix(height, width);
		greenData = Matrix(height, width);
		blueData = Matrix(height, width);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* temp = new unsigned char;
				fread(temp, sizeof(unsigned char), 1, file);


				//索引转换RGB
				blueData[i][j] = pRGBQuad[*temp].rgbBlue;
				greenData[i][j] = pRGBQuad[*temp].rgbGreen;
				redData[i][j] = pRGBQuad[*temp].rgbRed;
			}
			fseek(file, skip, SEEK_CUR);
		}
		break;
	case 24:	//888真彩色
		redData = Matrix(height, width);
		greenData = Matrix(height, width);
		blueData = Matrix(height, width);
		type = Type::RGB;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* temp = new unsigned char[3];
				fread(temp, sizeof(unsigned char), 3, file);

				blueData[i][j] = temp[0];
				greenData[i][j] = temp[1];
				redData[i][j] = temp[2];
			}
			fseek(file, skip, SEEK_CUR);
		}
		break;
	case 32:	//8888Alpha通道
		redData = Matrix(height, width);
		greenData = Matrix(height, width);
		blueData = Matrix(height, width);
		alphaData = Matrix(height, width);
		type = Type::RGBA;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* temp = new unsigned char[4];
				fread(temp, sizeof(unsigned char), 4, file);

				blueData[i][j] = temp[0];
				greenData[i][j] = temp[1];
				redData[i][j] = temp[2];
				alphaData[i][j] = temp[3];
			}
		}
		break;
	default:
		return false;
		break;
	}

	return true;
}
bool Image::BitmapSave(string fileName)
{
	FILE* file = fopen(fileName.c_str(), "wb");
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	int bitCount = 0;
	if (file == 0)
	{
		return false;
	}
	switch (type)
	{
	case Type::RGB:
		bitCount = 24;
		break;
	case Type::RGBA:
		bitCount = 24;
		break;
	case Type::GRAY:
		bitCount = 24;
		break;
	case Type::BW:
		bitCount = 24;
		break;
	}
	infoHeader.biBitCount = bitCount;
	infoHeader.biSize = 0x28;
	infoHeader.biPlanes = 1;
	infoHeader.biCompression = 0;
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biSizeImage = width * height * bitCount / 8;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	infoHeader.biXPelsPerMeter = XPixelsPerMeter;
	infoHeader.biYPelsPerMeter = YPixelsPerMeter;
	fileHeader.bfOffBits = 54;
	fileHeader.bfSize = infoHeader.biSizeImage + 54;
	fileHeader.bfType = 0x4D42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	int lineByte = (width * bitCount / 8 + 3) / 4 * 4;
	int skip = lineByte - width * bitCount / 8;
	unsigned char* left = new unsigned char[skip];
	for (int i = 0; i < skip; i++)
	{
		left[i] = 0;
	}
	switch (bitCount)
	{
	case 24:	//888真彩色
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* temp = new unsigned char[3];

				temp[0] = blueData[i][j];
				temp[1] = greenData[i][j];
				temp[2] = redData[i][j];

				fwrite(temp, sizeof(unsigned char), 3, file);
			}
			fwrite(left, sizeof(unsigned char), skip, file);
		}
		break;
	case 32:	//8888Alpha通道
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* temp = new unsigned char[4];

				temp[0] = blueData[i][j];
				temp[1] = greenData[i][j];
				temp[2] = redData[i][j];
				temp[3] = alphaData[i][j];

				fwrite(temp, sizeof(unsigned char), 4, file);
			}
		}
		break;
	}
}

Image Image::ToGray()
{
	Image dst = *this;

	switch (type)
	{
	case Image::Type::RGB:case Image::Type::RGBA:
		dst.type = Type::GRAY;
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				dst.blueData[i][j] = dst.greenData[i][j] = dst.redData[i][j] = unsigned char(0.299 * this->redData[i][j] + 0.587 * this->greenData[i][j] + 0.114 * this->blueData[i][j]);
			}
		}
		break;
	case Image::Type::GRAY:case Image::Type::BW:
		return dst;
		break;
	}

	return dst;
}
Image Image::ToBW()
{
	return ToBW(Otsu(this->GrayHist()));
}
Image Image::ToBW(unsigned threshold)
{
	Image src;
	Image dst = *this;
	if (type == Type::RGB || type == Type::RGBA)
	{
		src = this->ToGray();
	}
	else {
		src = *this;
	}
	dst.type = Type::BW;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (dst.blueData[i][j] >= threshold)
			{
				dst.blueData[i][j] = 255;
				dst.greenData[i][j] = 255;
				dst.redData[i][j] = 255;
			}
			else {
				dst.blueData[i][j] = 0;
				dst.greenData[i][j] = 0;
				dst.redData[i][j] = 0;
			}
		}
	}
	return dst;
}
vector<double> Image::GrayHist()
{
	vector<double> res(256);
	Image src;

	switch (type)
	{
	case Image::Type::RGB:case Image::Type::RGBA:
		src = this->ToGray();
		break;
	case Image::Type::GRAY:case Image::Type::BW:
		src = *this;
		break;
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			res[src.redData[i][j]]++;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		res[i] = res[i] / ((double)width * (double)height);
	}
	return res;
}
vector<double> Image::RedHist()
{
	vector<double> res(256);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			res[redData[i][j]]++;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		res[i] = res[i] / ((double)width * (double)height);
	}
	return res;
}
vector<double> Image::GreenHist()
{
	vector<double> res(256);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			res[greenData[i][j]]++;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		res[i] = res[i] / ((double)width * (double)height);
	}
	return res;
}
vector<double> Image::BlueHist()
{
	vector<double> res(256);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			res[blueData[i][j]]++;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		res[i] = res[i] / ((double)width * (double)height);
	}
	return res;
}
unsigned char Image::Otsu(vector<double> hist)
{
	double mG = 0;
	for (int i = 0; i < 256; i++)
	{
		mG += i * hist[i];
	}

	vector<double> result(256);
	for (int k = 0; k < 256; k++)
	{
		double p1 = 0;
		double m = 0;

		for (int i = 0; i < k; i++)
		{
			p1 += hist[i];
			m += i * hist[i];
		}
		if (p1 == 0)
		{
			result[k] = 0;
		}
		else {
			result[k] = (mG * p1 - m) * (mG * p1 - m) / (p1 * (1 - p1));
		}
	}
	int maxPosition = max_element(result.begin(), result.end()) - result.begin();

	return maxPosition;
}
int Image::GetWidth()
{
	return this->width;
}
int Image::GetHeight()
{
	return this->height;
}
Image::Type Image::GetType()
{
	return this->type;
}
int Image::GetXPixelPerMeter()
{
	return this->XPixelsPerMeter;
}
int Image::GetYPixelPerMeter()
{
	return this->YPixelsPerMeter;
}


Image Image::MorOperation(Matrix structure_element, int centerX, int centerY, Image::Morphology op)
{
	Image src;
	Image dst;
	dst.width = this->width;
	dst.height = this->height;
	dst.XPixelsPerMeter = this->XPixelsPerMeter;
	dst.YPixelsPerMeter = this->YPixelsPerMeter;
	dst.type = Type::BW;
	dst.blueData = dst.greenData = dst.redData = Matrix(this->height, this->width);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			dst.blueData[i][j] = dst.greenData[i][j] = dst.redData[i][j] = 255;
		}
	}

	if (this->type == Type::BW)
	{
		src = *this;
	}
	else {
		src = this->ToBW();
	}

	switch (op)
	{
	case Image::Morphology::ERODE:
		for (int i = 0; i <= height - structure_element.Row(); i++)
		{
			for (int j = 0; j <= width - structure_element.Colomn(); j++)
			{
				bool pass = true;
				for (int m = 0; m < structure_element.Row(); m++)
				{
					for (int n = 0; n < structure_element.Colomn(); n++)
					{
						if (src.redData[i + m][j + n] == 0 && structure_element[m][n] == 0)
						{
							pass = false;
						}
						if (src.redData[i + m][j + n] == 255 && structure_element[m][n] == 1)
						{
							pass = false;
						}
					}
				}
				if (pass)
				{
					dst.blueData[i + centerX][j + centerY] = dst.greenData[i + centerX][j + centerY] = dst.redData[i + centerX][j + centerY] = 0;
				}
			}
		}
		break;
	case Image::Morphology::DILATE:
		for (int i = 0; i <= height - structure_element.Row(); i++)
		{
			for (int j = 0; j <= width - structure_element.Colomn(); j++)
			{
				if (src.redData[i + centerX][j + centerY] == 0)
				{
					for (int m = 0; m < structure_element.Row(); m++)
					{
						for (int n = 0; n < structure_element.Colomn(); n++)
						{
							if (structure_element[m][n] == 1)
							{
								dst.blueData[i + m][j + n] = dst.greenData[i + m][j + n] = dst.redData[i + m][j + n] = 0;
							}
						}
					}
				}

			}
		}
		break;
	case Image::Morphology::OPEN:
		dst = MorOperation(structure_element, centerX, centerY, Image::Morphology::ERODE).MorOperation(structure_element, centerX, centerY, Image::Morphology::DILATE);
		break;
	case Image::Morphology::CLOSE:
		dst = MorOperation(structure_element, centerX, centerY, Image::Morphology::DILATE).MorOperation(structure_element, centerX, centerY, Image::Morphology::ERODE);
		break;
	}

	return dst;
}

void Image::SetType(Image::Type type)
{
	this->type = type;
}
void Image::GetData(Image::Space sp, Matrix* mats)
{
	switch (sp)
	{
	case Image::Space::RGB:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				mats[0][i][j] = unsigned char(redData[i][j]);
				mats[1][i][j] = unsigned char(greenData[i][j]);
				mats[2][i][j] = unsigned char(blueData[i][j]);
			}
		}
		break;
	case Image::Space::YUV:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				mats[0][i][j] = unsigned char(0.299 * redData[i][j] + 0.587 * greenData[i][j] + 0.114 * blueData[i][j]);    //Y
				mats[1][i][j] = -0.169 * redData[i][j] - 0.331 * greenData[i][j] + 0.500 * blueData[i][j];   //U
				mats[2][i][j] = 0.500 * redData[i][j] - 0.419 * greenData[i][j] - 0.081 * blueData[i][j] + 128;     //V
			}
		}
		break;
	case Image::Space::HSI:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				double R = redData[i][j];
				double G = greenData[i][j];
				double B = blueData[i][j];
				double min = min(B, min(R, G));
				double theta = 180 * acos((0.5 * (2 * R - G - B)) / (sqrt((R - G) * (R - G) + (R - B) * (G - B)))) / PI;
				mats[0][i][j] = B > G ? 360 - theta : theta;
				mats[1][i][j] = 1 - 3 * min / (redData[i][j] + greenData[i][j] + blueData[i][j]); //S
				mats[2][i][j] = unsigned char(0.299 * this->redData[i][j] + 0.587 * this->greenData[i][j] + 0.114 * this->blueData[i][j]);
				//mats[2][i][j] = (redData[i][j] + greenData[i][j] + blueData[i][j]) / 3.0;     //I
			}
		}
		break;
	case Image::Space::CMY:
		break;
	case Image::Space::Lab:
		break;
	default:
		break;
	}
}
void Image::SetData(Image::Space sp, Matrix* mats)
{
	switch (sp)
	{
	case Image::Space::RGB:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				redData[i][j] = unsigned char(mats[0][i][j]);
				greenData[i][j] = unsigned char(mats[1][i][j]);
				blueData[i][j] = unsigned char(mats[2][i][j]);
			}
		}
		break;
	case Image::Space::YUV:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				redData[i][j] = unsigned char(mats[0][i][j] + 1.403 * (mats[2][i][j] - 128));
				greenData[i][j] = unsigned char(mats[0][i][j] - 0.343 * (mats[1][i][j] - 128) - 0.714 * (mats[2][i][j] - 128));
				blueData[i][j] = unsigned char(mats[0][i][j] + 1.770 * (mats[1][i][j] - 128));
			}
		}
		break;
	case Image::Space::HSI:
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				double H = mats[0][i][j];
				double S = mats[1][i][j];
				double I = mats[2][i][j];
				if (H < 120)
				{
					H = H * PI / 180;
					blueData[i][j] = I * (1 - S);
					redData[i][j] = I * (1 + S * cos(H) / cos(PI / 3 - H));
					greenData[i][j] = 3 * I - redData[i][j] - blueData[i][j];
				}
				else if (H >= 240)
				{
					H -= 240;
					H = H * PI / 180;
					greenData[i][j] = I * (1 - S);
					blueData[i][j] = I * (1 + S * cos(H) / cos(PI / 3 - H));
					redData[i][j] = 3 * I - greenData[i][j] - blueData[i][j];
				}
				else {
					H -= 120;
					H = H * PI / 180;
					redData[i][j] = I * (1 - S);
					greenData[i][j] = I * (1 + S * cos(H) / cos(PI / 3 - H));
					blueData[i][j] = 3 * I - redData[i][j] - greenData[i][j];
				}
			}
		}
		break;
	case Image::Space::CMY:
		break;
	case Image::Space::Lab:
		break;
	default:
		break;
	}
}

Image Image::Enhance(Image::EnhanceType name, double parm)
{
	Image dst = *this;

	switch (name)
	{
	case Image::EnhanceType::LOG:   //default 255
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				dst.redData[i][j] = unsigned char(255 * log(1 + parm * (redData[i][j] / 255.0)) / log(parm + 1));
				dst.greenData[i][j] = unsigned char(255 * log(1 + parm * (greenData[i][j] / 255.0)) / log(parm + 1));
				dst.blueData[i][j] = unsigned char(255 * log(1 + parm * (blueData[i][j] / 255.0)) / log(parm + 1));
			}
		}
		break;
	case Image::EnhanceType::GRMMA:  //default 1
		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				dst.redData[i][j] = unsigned char(255 * pow(redData[i][j] / 255, parm));
				dst.greenData[i][j] = unsigned char(255 * pow(greenData[i][j] / 255, parm));
				dst.blueData[i][j] = unsigned char(255 * pow(blueData[i][j] / 255, parm));
			}
		}
		break;
	case Image::EnhanceType::LINEAR:
		break;
	case Image::EnhanceType::HIST:
		if (type == Image::Type::GRAY || type == Image::Type::BW)
		{
			vector<double> ghist = this->GrayHist();
			vector<double> accuhist(256);
			double sum = 0;

			for (int i = 0; i < 256; i++)
			{
				sum += ghist[i];
				accuhist[i] = sum;
			}
			for (int i = 0; i < this->height; i++)
			{
				for (int j = 0; j < this->width; j++)
				{
					dst.redData[i][j] = dst.greenData[i][j] = dst.blueData[i][j] = unsigned char(255 * accuhist[redData[i][j]]);
				}
			}
		}
		else {
			vector<double> rhist = this->RedHist();
			vector<double> accurhist(256);
			double rsum = 0;

			for (int i = 0; i < 256; i++)
			{
				rsum += rhist[i];
				accurhist[i] = rsum;
			}

			vector<double> greenhist = this->GreenHist();
			vector<double> accugreenhist(256);
			double greensum = 0;

			for (int i = 0; i < 256; i++)
			{
				greensum += greenhist[i];
				accugreenhist[i] = greensum;
			}

			vector<double> bhist = this->BlueHist();
			vector<double> accubhist(256);
			double bsum = 0;

			for (int i = 0; i < 256; i++)
			{
				bsum += bhist[i];
				accubhist[i] = bsum;
			}

			for (int i = 0; i < this->height; i++)
			{
				for (int j = 0; j < this->width; j++)
				{
					dst.redData[i][j] = 255 * accurhist[unsigned char(redData[i][j])];
					dst.greenData[i][j] = 255 * accugreenhist[unsigned char(greenData[i][j])];
					dst.blueData[i][j] = 255 * accubhist[unsigned char(blueData[i][j])];
				}
			}
		}
		break;
	}

	return dst;
}
Image Image::Enhance(Image::EnhanceType name)
{
	switch (name)
	{
	case Image::EnhanceType::LOG:
		return Enhance(Image::EnhanceType::LOG, 255);
		break;
	case Image::EnhanceType::GRMMA:
		return Enhance(Image::EnhanceType::GRMMA, 1);
		break;
	case Image::EnhanceType::LINEAR:
		break;
	case Image::EnhanceType::HIST:
		return Enhance(Image::EnhanceType::HIST, 1);
		break;
	}
}

Image Image::ReSize(int reHeight, int reWidth)
{
	Image dst = *this;

	dst.height = reHeight;
	dst.width = reWidth;
	dst.redData = Matrix(dst.height, dst.width);
	dst.greenData = Matrix(dst.height, dst.width);
	dst.blueData = Matrix(dst.height, dst.width);
	for (int dstX = 0; dstX < reHeight; dstX++)
	{
		for (int dstY = 0; dstY < reWidth; dstY++)
		{
			double srcX = height * dstX / reHeight;
			double srcY = width * dstY / reWidth;
			double X = floor(srcX);
			double deltaX = srcX - X;
			double Y = floor(srcY);
			double deltaY = srcY - Y;

			if (X == 0 && Y == 0)
			{
				dst.redData[dstX][dstY] = redData[X][Y] * deltaX * deltaY;
				dst.greenData[dstX][dstY] = greenData[X][Y] * deltaX * deltaY;
				dst.blueData[dstX][dstY] = blueData[X][Y] * deltaX * deltaY;
			}
			else if (X == height && Y == width)
			{
				dst.redData[dstX][dstY] = redData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY);
				dst.greenData[dstX][dstY] = greenData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY);
				dst.blueData[dstX][dstY] = blueData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY);
			}
			else if (X == 0 && Y == width)
			{
				dst.redData[dstX][dstY] = redData[X][Y - 1] * deltaX * (1 - deltaY);
				dst.greenData[dstX][dstY] = greenData[X][Y - 1] * deltaX * (1 - deltaY);
				dst.blueData[dstX][dstY] = blueData[X][Y - 1] * deltaX * (1 - deltaY);
			}
			else if (X == height && Y == 0)
			{
				dst.redData[dstX][dstY] = redData[X - 1][Y] * (1 - deltaX) * deltaY;
				dst.greenData[dstX][dstY] = greenData[X - 1][Y] * (1 - deltaX) * deltaY;
				dst.blueData[dstX][dstY] = blueData[X - 1][Y] * (1 - deltaX) * deltaY;
			}
			else if (X == 0 && X != height && Y != width)
			{
				dst.redData[dstX][dstY] = redData[X][Y - 1] * deltaX * (1 - deltaY) + redData[X][Y] * deltaX * deltaY;
				dst.greenData[dstX][dstY] = greenData[X][Y - 1] * deltaX * (1 - deltaY) + greenData[X][Y] * deltaX * deltaY;
				dst.blueData[dstX][dstY] = blueData[X][Y - 1] * deltaX * (1 - deltaY) + blueData[X][Y] * deltaX * deltaY;
			}
			else if (Y == 0 && Y != width && X != height)
			{
				dst.redData[dstX][dstY] = redData[X - 1][Y] * deltaX * (1 - deltaY) + redData[X][Y] * deltaX * deltaY;
				dst.greenData[dstX][dstY] = greenData[X - 1][Y] * deltaX * (1 - deltaY) + greenData[X][Y] * deltaX * deltaY;
				dst.blueData[dstX][dstY] = blueData[X - 1][Y] * deltaX * (1 - deltaY) + blueData[X][Y] * deltaX * deltaY;
			}
			else if (X == height)
			{
				dst.redData[dstX][dstY] = redData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + redData[X - 1][Y] * (1 - deltaX) * deltaY;
				dst.greenData[dstX][dstY] = greenData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + greenData[X - 1][Y] * (1 - deltaX) * deltaY;
				dst.blueData[dstX][dstY] = blueData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + blueData[X - 1][Y] * (1 - deltaX) * deltaY;
			}
			else if (Y == width)
			{
				dst.redData[dstX][dstY] = redData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + redData[X][Y - 1] * deltaX * (1 - deltaY);
				dst.greenData[dstX][dstY] = greenData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + greenData[X][Y - 1] * deltaX * (1 - deltaY);
				dst.blueData[dstX][dstY] = blueData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + blueData[X][Y - 1] * deltaX * (1 - deltaY);
			}
			else {
				dst.redData[dstX][dstY] = redData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + redData[X][Y - 1] * deltaX * (1 - deltaY) + redData[X - 1][Y] * (1 - deltaX) * deltaY + redData[X][Y] * deltaX * deltaY;
				dst.greenData[dstX][dstY] = greenData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + greenData[X][Y - 1] * deltaX * (1 - deltaY) + greenData[X - 1][Y] * (1 - deltaX) * deltaY + greenData[X][Y] * deltaX * deltaY;
				dst.blueData[dstX][dstY] = blueData[X - 1][Y - 1] * (1 - deltaX) * (1 - deltaY) + blueData[X][Y - 1] * deltaX * (1 - deltaY) + blueData[X - 1][Y] * (1 - deltaX) * deltaY + blueData[X][Y] * deltaX * deltaY;
			}
		}



	}

	return dst;
}
Image Image::ReSize(double ratio)
{
	return Image::ReSize(int(ratio * this->height), int(ratio * this->width));
}
Image Image::Translate(double dX, double dY)
{
	dY = -dY;
	Image dst = *this;
	for (int i = 0; i < dst.height; i++)
	{
		for (int j = 0; j < dst.width; j++)
		{
			if (i - dY < 0 || j - dX < 0 || i - dY >= height || j - dX >= width)
			{
				dst.redData[i][j] = backgroundColorR;
				dst.greenData[i][j] = backgroundColorG;
				dst.blueData[i][j] = backgroundColorB;
			}
			else {
				dst.redData[i][j] = redData[i - dY][j - dX];
				dst.greenData[i][j] = greenData[i - dY][j - dX];
				dst.blueData[i][j] = blueData[i - dY][j - dX];
			}
		}
	}

	return dst;
}
Image Image::Mirror(bool XY)
{
	Image dst = *this;
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			if (!XY) //X-0
			{
				dst.redData[i][j] = redData[i][width - 1 - j];
				dst.greenData[i][j] = greenData[i][width - 1 - j];
				dst.blueData[i][j] = blueData[i][width - 1 - j];
			}
			else {
				dst.redData[i][j] = redData[height - 1 - i][j];
				dst.greenData[i][j] = greenData[height - 1 - i][j];
				dst.blueData[i][j] = blueData[height - 1 - i][j];
			}
		}
	}
	return dst;
}

Image Image::Rotate(double theta)
{
	Image dst = *this;
	theta = -theta;
	double COS = cos(theta * PI / 180);
	double SIN = sin(theta * PI / 180);
	dst.width = ceil(abs(width * COS) + abs(height * SIN));
	dst.height = ceil(abs(height * COS) + abs(width * SIN));
	dst.redData = dst.greenData = dst.blueData = Matrix(dst.height, dst.width);

	for (int i = 1; i <= dst.width; i++)
	{
		for (int j = 1; j <= dst.height; j++)
		{
			double originX = COS * (i - 0.5 * dst.width) - SIN * (0.5 * dst.height - j) + 0.5 * width;
			double originY = -SIN * (i - 0.5 * dst.width) - COS * (0.5 * dst.height - j) + 0.5 * height;
			double X = round(originX);
			double Y = round(originY);
			if (Y < 1 || X < 1 || Y > height || X > width)
			{
				dst.redData[j - 1][i - 1] = backgroundColorR;
				dst.greenData[j - 1][i - 1] = backgroundColorG;
				dst.blueData[j - 1][i - 1] = backgroundColorB;
			}
			else {
				double left = floor(X);
				double right = ceil(X);
				double top = floor(Y);
				double bottom = ceil(Y);
				double deltaX = X - left;
				double deltaY = Y - top;

				dst.redData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * redData[top - 1][left - 1] + deltaX * (1 - deltaY) * redData[top - 1][right - 1] + deltaY * (1 - deltaX) * redData[bottom - 1][left - 1] + deltaX * deltaY * redData[bottom - 1][right - 1];
				dst.greenData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * greenData[top - 1][left - 1] + deltaX * (1 - deltaY) * greenData[top - 1][right - 1] + deltaY * (1 - deltaX) * greenData[bottom - 1][left - 1] + deltaX * deltaY * greenData[bottom - 1][right - 1];
				dst.blueData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * blueData[top - 1][left - 1] + deltaX * (1 - deltaY) * blueData[top - 1][right - 1] + deltaY * (1 - deltaX) * blueData[bottom - 1][left - 1] + deltaX * deltaY * blueData[bottom - 1][right - 1];
			}
		}
	}


	return dst;
}
Image Image::Shear(bool XY, double theta)
{
	if (abs(theta) > 87)
	{
		theta = 87;
	}
	theta = -theta;
	Image dst = *this;
	double TAN = tan(theta / 180 * PI);
	if (XY)
	{
		dst.height = (int)(height + width * abs(TAN));
	}
	else {
		dst.width = (int)(width + height * abs(TAN));
	}

	dst.redData = dst.greenData = dst.blueData = Matrix(dst.height, dst.width);


	for (int i = 1; i <= dst.width; i++)
	{
		for (int j = 1; j <= dst.height; j++)
		{
			double originX;
			double originY;
			if (XY)
			{
				originX = i - 0.5 * dst.width + 0.5 * width;
				originY = -TAN * (i - 0.5 * dst.width) + (-0.5 * dst.height + j) + 0.5 * height;
			}
			else {
				originX = (i - 0.5 * dst.width) - TAN * (0.5 * dst.height - j) + 0.5 * width;
				originY = -0.5 * dst.height + j + 0.5 * height;
			}
			double X = round(originX);
			double Y = round(originY);
			if (Y < 1 || X < 1 || Y > height || X > width)
			{
				dst.redData[j - 1][i - 1] = backgroundColorR;
				dst.greenData[j - 1][i - 1] = backgroundColorG;
				dst.blueData[j - 1][i - 1] = backgroundColorB;
			}
			else {
				double left = floor(X);
				double right = ceil(X);
				double top = floor(Y);
				double bottom = ceil(Y);
				double deltaX = X - left;
				double deltaY = Y - top;

				dst.redData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * redData[top - 1][left - 1] + deltaX * (1 - deltaY) * redData[top - 1][right - 1] + deltaY * (1 - deltaX) * redData[bottom - 1][left - 1] + deltaX * deltaY * redData[bottom - 1][right - 1];
				dst.greenData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * greenData[top - 1][left - 1] + deltaX * (1 - deltaY) * greenData[top - 1][right - 1] + deltaY * (1 - deltaX) * greenData[bottom - 1][left - 1] + deltaX * deltaY * greenData[bottom - 1][right - 1];
				dst.blueData[j - 1][i - 1] = (1 - deltaX) * (1 - deltaY) * blueData[top - 1][left - 1] + deltaX * (1 - deltaY) * blueData[top - 1][right - 1] + deltaY * (1 - deltaX) * blueData[bottom - 1][left - 1] + deltaX * deltaY * blueData[bottom - 1][right - 1];
			}
		}
	}

	return dst;
}

void Image::GetBackground(int R, int G, int B)
{
	R = backgroundColorR;
	G = backgroundColorG;
	B = backgroundColorB;
}

void Image::SetBackground(int R, int G, int B)
{
	backgroundColorR = R;
	backgroundColorG = G;
	backgroundColorB = B;
}



Image Image::Sharpen(Matrix& mask)
{
	Image src;
	if (type == Image::Type::RGB || type == Image::Type::RGBA)
	{
		src = this->ToGray();
	}
	else {
		src = *this;
	}

	Matrix res(height, width);

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			res[i][j] = mask[0][0] * src.redData[i - 1][j - 1] + mask[0][1] * src.redData[i - 1][j] +
				mask[0][2] * src.redData[i - 1][j + 1] + mask[1][0] * src.redData[i][j - 1] +
				mask[1][1] * src.redData[i][j] + mask[1][2] * src.redData[i][j + 1] +
				mask[2][0] * src.redData[i + 1][j - 1] + mask[2][1] * src.redData[i + 1][j] +
				mask[2][2] * src.redData[i + 1][j + 1];
		}
	}

	Image dst = *this;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (mask[1][1] > 0)
			{
				dst.redData[i][j] += res[i][j];
				dst.greenData[i][j] += res[i][j];
				dst.blueData[i][j] += res[i][j];
			}
			else {
				dst.redData[i][j] -= res[i][j];
				dst.greenData[i][j] -= res[i][j];
				dst.blueData[i][j] -= res[i][j];
			}
			dst.redData[i][j] = min(255, max(dst.redData[i][j], 0));
			dst.greenData[i][j] = min(255, max(dst.greenData[i][j], 0));
			dst.blueData[i][j] = min(255, max(dst.blueData[i][j], 0));
		}
	}

	return dst;
}
Image Image::Sharpen(Image::SharpenType st)
{
	if (st == Image::SharpenType::LAPLACIAN_4)
	{
		Matrix mask(3, 3);
		mask = { 0,1,0,1,-4,1,0,1,0 };
		return Sharpen(mask);
	}
	else if (st == Image::SharpenType::LAPLACIAN_8)
	{
		Matrix mask(3, 3);
		mask = { 1,1,1,1,-8,1,1,1,1 };
		return Sharpen(mask);
	}
}
Image Image::Filter(Matrix& mask)
{
	Image dst = *this;
	double sum = 0;

	if (mask.Row() % 2 == 0 || mask.Colomn() % 2 == 0)
	{
		printf("Mask should be odd.");
		return dst;
	}
	for (int i = 0; i < mask.Row(); i++)
	{
		for (int j = 0; j < mask.Colomn(); j++)
		{
			sum += mask[i][j];
		}
	}
	for (int i = 0; i < mask.Row(); i++)
	{
		for (int j = 0; j < mask.Colomn(); j++)
		{
			mask[i][j] /= sum;
		}
	}
	for (int i = (mask.Row() - 1) / 2; i <= height - (mask.Row() + 1) / 2; i++)
	{
		for (int j = (mask.Colomn() - 1) / 2; j <= width - (mask.Row() + 1) / 2; j++)
		{
			double resR = 0;
			double resG = 0;
			double resB = 0;
			int x = 0;

			for (int m = i - (mask.Row() - 1) / 2; m <= i + (mask.Row() - 1) / 2; m++, x++)
			{
				int y = 0;
				for (int n = j - (mask.Colomn() - 1) / 2; n <= j + (mask.Colomn() - 1) / 2; n++, y++)
				{
					resR += dst.redData[m][n] * mask[x][y];
					resG += dst.greenData[m][n] * mask[x][y];
					resB += dst.blueData[m][n] * mask[x][y];
				}
			}
			dst.redData[i][j] = resR;
			dst.greenData[i][j] = resG;
			dst.blueData[i][j] = resB;
		}
	}

	return dst;
}
Image Image::Filter(Image::FilterType ft, int n, double sigma)
{
	if (ft == Image::FilterType::AVERAGE)
	{
		Matrix mask(n, n);
		for (int i = 0; i < n * n; i++)
		{
			mask(i) = 1;
		}
		return Filter(mask);
	}
	else if (ft == Image::FilterType::GAUSSIAN)
	{
		int center = n / 2;
		double x2, y2;
		Matrix mask(n, n);

		for (int i = 0; i < n; i++)
		{
			x2 = pow(i - center, 2);
			for (int j = 0; j < n; j++)
			{
				y2 = pow(j - center, 2);
				double g = exp(-(x2 + y2) / (2 * sigma * sigma));
				g /= 2 * PI * sigma;
				mask[i][j] = g;
			}
		}
		double k = 1 / mask[0][0];
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				mask[i][j] *= k;
				mask[i][j] = floor(mask[i][j]);
			}
		}
		return Filter(mask);
	}
	else if (ft == Image::FilterType::MEDIAN)
	{
		Image dst = *this;

		for (int i = (n - 1) / 2; i <= height - (n + 1) / 2; i++)
		{
			for (int j = (n - 1) / 2; j <= width - (n + 1) / 2; j++)
			{
				vector<double> resR;
				vector<double> resG;
				vector<double> resB;

				for (int x = i - (n - 1) / 2; x <= i + (n - 1) / 2; x++)
				{
					for (int y = j - (n - 1) / 2; y <= j + (n - 1) / 2; y++)
					{
						resR.push_back(redData[x][y]);
						resG.push_back(greenData[x][y]);
						resB.push_back(blueData[x][y]);
					}
				}
				sort(resR.begin(), resR.end());
				sort(resG.begin(), resG.end());
				sort(resB.begin(), resB.end());
				dst.redData[i][j] = resR[(n * n - 1) / 2];
				dst.greenData[i][j] = resG[(n * n - 1) / 2];
				dst.blueData[i][j] = resB[(n * n - 1) / 2];
			}
		}

		return dst;
	}
}
