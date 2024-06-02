// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

wchar_t* projectPath;
#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;
struct ImageData {
	std::string path;
	std::string label;
};
void listFilesInDirectory(const std::string& directoryPath, std::vector<ImageData>& trainList, std::vector<ImageData>& testList)
{
	try
	{
		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			if (fs::is_regular_file(entry))
			{
				ImageData imageData;
				imageData.path = entry.path().string();

				std::string directoryName = directoryPath.substr(directoryPath.find_last_of("/\\") + 1);

				size_t lastSpacePos = directoryName.find_last_of(" ");
				if (lastSpacePos != std::string::npos && lastSpacePos + 1 < directoryName.size()) {
					imageData.label = directoryName.substr(lastSpacePos + 1);
				}
				else {
					imageData.label = directoryName;
				}

				if (directoryPath.find("train") != std::string::npos) {
					trainList.push_back(imageData);
				}
				else if (directoryPath.find("test") != std::string::npos) {
					testList.push_back(imageData);
				}

			}

			else if (fs::is_directory(entry))
			{
				if (entry.path().filename() == "train") {
					listFilesInDirectory(entry.path().string(), trainList, testList);
				}
				else if (entry.path().filename() == "test") {
					listFilesInDirectory(entry.path().string(), trainList, testList);
				}
				else {
					listFilesInDirectory(entry.path().string(), trainList, testList);

				}
			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}
}
std::vector<std::string> uniqueLabels;

void addUniqueLabels(const std::vector<ImageData>& files)
{
	for (const auto& imageData : files)
	{
		if (std::find(uniqueLabels.begin(), uniqueLabels.end(), imageData.label) == uniqueLabels.end()) {
			uniqueLabels.push_back(imageData.label);
		}
	}
}
int totalImages = 0;
int okImages = 0;

void addUniqueLabelsAndCount(const std::vector<ImageData>& files)
{
	std::srand(std::time(nullptr));

	for (const auto& imageData : files)
	{
		if (std::find(uniqueLabels.begin(), uniqueLabels.end(), imageData.label) == uniqueLabels.end()) {
			uniqueLabels.push_back(imageData.label);
		}

		int randomIndex = std::rand() % uniqueLabels.size();

		if (imageData.label == uniqueLabels[randomIndex]) {
			okImages++;
		}

		totalImages++;
	}
}
double testFilesAcurate(int okImages, int totalImages)
{
	return (double)okImages / (double)totalImages;
}

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

void testTrainFilesCount(const std::vector<ImageData>& trainFiles) {
	assert(trainFiles.size() == 7624 && "Train files count is not equal to 7624");
}
void testTestFilesCount(const std::vector<ImageData>& testFiles) {
	assert(testFiles.size() == 265 && "Test files count is not equal to 265");
}
int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	std::string directoryPath = "archive";
	std::vector<ImageData> trainFiles;
	std::vector<ImageData> testFiles;
	

	listFilesInDirectory(directoryPath, trainFiles, testFiles);
	testTrainFilesCount(trainFiles);
	testTestFilesCount(testFiles);
	addUniqueLabelsAndCount(trainFiles);
	addUniqueLabelsAndCount(testFiles);
	addUniqueLabels(trainFiles);
	addUniqueLabels(testFiles);
	double number = testFilesAcurate(okImages, totalImages);
	printf("ACC: %f",number);




	return 0;
}