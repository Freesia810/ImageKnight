#pragma once
#include "Matrix.h"
class Image
{
public:
	enum class Type { RGB, RGBA, GRAY, BW };
	enum class Space { RGB, YUV, HSI, CMY, Lab };
	enum class Morphology { ERODE, DILATE, OPEN, CLOSE };
	enum class EnhanceType { LOG, GRMMA, LINEAR, HIST };
	enum class FilterType { AVERAGE, GAUSSIAN, MEDIAN };
	enum class SharpenType { LAPLACIAN_4, LAPLACIAN_8 };

	Image(string fileName);
	Image() :height(0), width(0), type(Type::RGB), XPixelsPerMeter(0), YPixelsPerMeter(0) {};
	~Image();

	Image ToGray();
	Image ToBW();
	Image ToBW(unsigned threshold);

	void GetData(Space sp, Matrix* mats);
	void SetData(Space sp, Matrix* mats);

	void SetType(Type type);

	void GetBackground(int R, int G, int B);
	void SetBackground(int R, int G, int B);

	Image Enhance(EnhanceType name, double parm);
	Image Enhance(EnhanceType name);

	Image Sharpen(Matrix& mask); //锐化
	Image Sharpen(SharpenType st); //锐化模板
	Image Filter(Matrix& mask); //滤波
	Image Filter(FilterType ft, int n = 3, double sigma = 0.8); //滤波模板

	Image MorOperation(Matrix structure_element, int centerX, int centerY, Morphology op);

	Image ReSize(int reHeight, int reWidth);
	Image ReSize(double ratio);
	Image Translate(double dX, double dY);
	Image Mirror(bool XY); //X---0 Y---1
	Image Rotate(double theta); //顺时针 0~360
	Image Shear(bool XY, double theta); //0--水平 1--垂直

	int GetWidth();
	int GetHeight();
	int GetXPixelPerMeter();
	int GetYPixelPerMeter();
	Type GetType();

	vector<double> GrayHist();
	vector<double> RedHist();
	vector<double> GreenHist();
	vector<double> BlueHist();

	bool Save(string fileName);
private:
	int width;
	int height;
	int backgroundColorR = 0;
	int backgroundColorG = 0;
	int backgroundColorB = 0;
	int XPixelsPerMeter;
	int YPixelsPerMeter;
	Matrix redData;
	Matrix greenData;
	Matrix blueData;
	Matrix alphaData;
protected:
	Type type;
	enum class Ext { FIPD, BMP, PNG, JPEG, TIFF, GIF };
	Image::Ext getExtension(string fileName);
	bool BitmapRead(string fileName);
	bool BitmapSave(string fileName);
	bool FIPDRead(string fileName);
	bool FIPDSave(string fileName);
	unsigned char Otsu(vector<double> hist);
};
