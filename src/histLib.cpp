//-----------------------------------------------------------------------------
// Copyright (c) 2012, Paul Filitchkin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this list of
//      conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this list of
//      conditions and the following disclaimer in the documentation and/or other materials
//      provided with the distribution.
//
//    * Neither the name of the organization nor the names of its contributors may be used
//      to endorse or promote products derived from this software without specific prior written
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "histLib.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHistLib::CHistLib() :
  mHistImageHeight(300),
  mHistImageBorder(15),
  mBinCount(256),
  mHistPlotColor(HIST_LIB_COLOR_WHITE),
  mHistAxisColor(HIST_LIB_COLOR_WHITE),
  mHistBackgroundColor(HIST_LIB_COLOR_BLACK),
  mDrawXAxis(true)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHistLib::~CHistLib()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::SetHistImageHeight(unsigned HistImageHeight)
{
  if ((HistImageHeight > 0) && (HistImageHeight <= 2048))
  {
    mHistImageHeight = HistImageHeight;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::SetBinCount(unsigned BinCount)
{
  if ((BinCount > 0) && (BinCount <= 256))
  {
    mBinCount = BinCount;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::SetPlotColor(cv::Scalar Color)
{
  mHistPlotColor = Color;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::SetAxisColor(cv::Scalar Color)
{
  mHistAxisColor = Color;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::SetBackgroundColor(cv::Scalar Color)
{
  mHistBackgroundColor = Color;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned CHistLib::GetHistImageHeight() const
{
  return mHistImageHeight;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned CHistLib::GetBinCount() const
{
  return mBinCount;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
cv::Scalar CHistLib::GetPlotColor() const
{
  return mHistPlotColor;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
cv::Scalar CHistLib::GetAxisColor() const
{
  return mHistAxisColor;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
cv::Scalar CHistLib::GetBackgroundColor() const
{
  return mHistBackgroundColor;
}

//-----------------------------------------------------------------------------
// Description:
//   Overloaded function using C++ libraries/conventions. Here Hist can be
//   either a row or column vector.
//-----------------------------------------------------------------------------
void CHistLib::DrawHistogram(
  const Mat& Hist,
  Mat& HistImage,
  const cv::Scalar& Color)
{

  bool rowVector;
  unsigned HistLength;

  // Make sure we have a row or column vector
  if (Hist.rows > Hist.cols)
  {
    if ((Hist.rows < 2) || (Hist.cols != 1))
    {
      return;
    }
    rowVector = false;
    HistLength = Hist.rows;
  }
  else
  {
    if ((Hist.rows != 1) || (Hist.cols < 2))
    {
      return;
    }
    rowVector = true;
    HistLength = Hist.cols;
  }

  // Should do nothing if the input is already the correct size/type
  HistImage.create(
    2 * mHistImageBorder + mHistImageHeight,
    2 * mHistImageBorder + 3 * HistLength,
    CV_8UC3);

  HistImage.setTo(mHistBackgroundColor);

  // Draw the bins
  unsigned binValue = 0;

  switch (Hist.type())
  {
    // When Hist contains floats
    case CV_32F:
      if (rowVector)
      {
        for (int i = 0; i < Hist.cols; i++)
        {
          DrawHistBin(HistImage, Hist.at<float>(0, i), i * 3, mHistPlotColor);
        }
      }
      else
      {
        for (int i = 0; i < Hist.rows; i++)
        {
          DrawHistBin(HistImage, Hist.at<float>(i, 0), i * 3, mHistPlotColor);
        }
      }
    break;

    // When Hist contains doubles
    case CV_64F:
      if (rowVector)
      {
        for (int i = 0; i < Hist.cols; i++)
        {
          DrawHistBin(HistImage, Hist.at<double>(0, i), i * 3, mHistPlotColor);
        }
      }
      else
      {
        for (int i = 0; i < Hist.rows; i++)
        {
          DrawHistBin(HistImage, Hist.at<double>(i, 0), i * 3, mHistPlotColor);
        }
      }
    break;

    // When Hist contains ints
    case CV_32S:
      if (rowVector)
      {
        for (int i = 0; i < Hist.cols; i++)
        {
          DrawHistBin(HistImage, Hist.at<int>(0, i), i * 3, mHistPlotColor);
        }
      }
      else
      {
        for (int i = 0; i < Hist.rows; i++)
        {
          DrawHistBin(HistImage, Hist.at<int>(i, 0), i * 3, mHistPlotColor);
        }
      }
    break;

    default:
      return;
  }

  if (mDrawXAxis)
  {
    DrawHistBar(HistImage, HistLength);
  }
}

//-----------------------------------------------------------------------------
// Desrcription:
//   Helper function that draws a bin on the histogram image
//-----------------------------------------------------------------------------
void CHistLib::DrawHistBin(
  Mat& HistLayer,
  int Value,
  unsigned x,
  const cv::Scalar& Color)
{
  unsigned PixelHeight = (unsigned)Value;
  if (PixelHeight)
  {
    const unsigned BaseX = x + mHistImageBorder;
    const unsigned BaseY = mHistImageBorder + mHistImageHeight;

    line(
      HistLayer,
      Point(BaseX, BaseY),
      Point(BaseX, BaseY - PixelHeight),
      Color);
  }
}

//-----------------------------------------------------------------------------
// Desrcription:
//   Overloaded function that accepts a float for the bin height
//-----------------------------------------------------------------------------
void CHistLib::DrawHistBin(
  Mat& HistLayer,
  float Value,
  unsigned x,
  const cv::Scalar& Color)
{
  unsigned PixelHeight = (unsigned)cvRound(Value);
  if (PixelHeight)
  {
    const unsigned BaseX = x + mHistImageBorder;
    const unsigned BaseY = mHistImageBorder + mHistImageHeight;

    line(
      HistLayer,
      Point(BaseX, BaseY),
      Point(BaseX, BaseY - PixelHeight),
      Color);
  }
}

//-----------------------------------------------------------------------------
// Desrcription:
//   Overloaded function that accepts a double for the bin height
//-----------------------------------------------------------------------------
void CHistLib::DrawHistBin(
  Mat& HistLayer,
  double Value,
  unsigned x,
  const cv::Scalar& Color)
{
  unsigned PixelHeight = (unsigned)cvRound(Value);
  if (PixelHeight)
  {
    const unsigned BaseX = x + mHistImageBorder;
    const unsigned BaseY = mHistImageBorder + mHistImageHeight;

    line(
      HistLayer,
      Point(BaseX, BaseY),
      Point(BaseX, BaseY - PixelHeight),
      Color);
  }
}

//-----------------------------------------------------------------------------
// Description:
//   Additional info:
//   http://opencv.willowgarage.com/documentation/cpp/imgproc_histograms.html
//-----------------------------------------------------------------------------
void CHistLib::DrawHistogramBGR(const Mat& ImageBGR, Mat& ImageHist)
{
  // Initialize histogram settings
  int histSize[] = {mBinCount};
  float Range[] = {0, 256}; //{0, 256} = 0 to 255
  const float* Ranges[] = {Range};
  int chanB[] = {0};
  int chanG[] = {1};
  int chanR[] = {2};

  MatND HistB;
  MatND HistG;
  MatND HistR;

  calcHist(&ImageBGR, 1, chanB, Mat(), // do not use mask
           HistB, 1, histSize, Ranges,
           true, // the histogram is uniform
           false);

  calcHist(&ImageBGR, 1, chanG, Mat(), // do not use mask
           HistG, 1, histSize, Ranges,
           true, // the histogram is uniform
           false);

  calcHist(&ImageBGR, 1, chanR, Mat(), // do not use mask
           HistR, 1, histSize, Ranges,
           true, // the histogram is uniform
           false);

  double maxB = 0;
  double maxG = 0;
  double maxR = 0;

  minMaxLoc(HistB, 0, &maxB, 0, 0);
  minMaxLoc(HistG, 0, &maxG, 0, 0);
  minMaxLoc(HistR, 0, &maxR, 0, 0);

  double maxBGR = max(maxB, max(maxG, maxR));

  for (int i = 0; i < HistB.rows; i++)
  {
    HistB.at<float>(i,0) = (float)mHistImageHeight*HistB.at<float>(i,0)/(float)maxBGR;
    HistG.at<float>(i,0) = (float)mHistImageHeight*HistG.at<float>(i,0)/(float)maxBGR;
    HistR.at<float>(i,0) = (float)mHistImageHeight*HistR.at<float>(i,0)/(float)maxBGR;
  }

  DrawHistogram(HistB, ImageHist, HIST_LIB_COLOR_BLUE);
  DrawHistogram(HistG, ImageHist, HIST_LIB_COLOR_GREEN);
  DrawHistogram(HistR, ImageHist, HIST_LIB_COLOR_RED);
}

//-----------------------------------------------------------------------------
// Description:
//   Overloaded function using the C++ formats/functions
//-----------------------------------------------------------------------------
void CHistLib::DrawHistogramGray(const Mat& ImageBGR, Mat& ImageHist)
{
  // Create 1 channel image for grayscale representation
  Mat ImageGray = Mat(ImageBGR.rows, ImageBGR.cols, CV_8UC1);
  cvtColor(ImageBGR, ImageGray, CV_BGR2GRAY);

  // Initialize histogram settings
  int histSize[] = {mBinCount};
  float Range[] = {0, 256}; //{0, 256} = 0 to 255
  const float *Ranges[] = {Range};
  int channels[] = {0};

  MatND Hist;

  calcHist(&ImageGray, 1, channels, Mat(), // do not use mask
           Hist, 1, histSize, Ranges,
           true, // the histogram is uniform
           false);

  double maxVal=0;
  minMaxLoc(Hist, 0, &maxVal, 0, 0);

  for (int i = 0; i < Hist.rows; i++)
  {
    Hist.at<float>(i,0) =
      (float)mHistImageHeight*Hist.at<float>(i,0)/(float)maxVal;
  }

  DrawHistogram(Hist, ImageHist);
}

//-----------------------------------------------------------------------------
// Description:
//   Overloaded function, uses newer formats/functions
//-----------------------------------------------------------------------------
void CHistLib::DrawHistBar(Mat& HistImage, unsigned BinCount)
{
  // Draw the horizontal axis
  line(
      HistImage,
      Point(mHistImageBorder, mHistImageBorder + mHistImageHeight),
      Point(mHistImageBorder + 3 * BinCount, mHistImageBorder + mHistImageHeight),
      mHistAxisColor, 0);

  // Label initial bin
  putText(HistImage,
          "0", Point(mHistImageBorder - 3,
          mHistImageBorder + mHistImageHeight + 10),
          FONT_HERSHEY_SIMPLEX, 0.3f, mHistAxisColor);

  // Create text to display number of histogram bins
  stringstream mBinCountSS;
  mBinCountSS << (BinCount-1);

  // Label last bin
  putText(
    HistImage,
    mBinCountSS.str().c_str(),
    Point(mHistImageBorder + 3*BinCount - 10, mHistImageBorder + mHistImageHeight + 10),
    FONT_HERSHEY_SIMPLEX, 0.3f, mHistAxisColor);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::NormalizeClipImageBGR(
  const Mat& ImageBGR,
  Mat& ImageBGRNorm,
  double clipPercent)
{

  Mat ImageHSV     = Mat(ImageBGR.size(), CV_8UC3);
  Mat ImageHSVNorm = Mat(ImageBGR.size(), CV_8UC3);

  cvtColor(ImageBGR, ImageHSV, CV_BGR2HSV);

  unsigned char* data     = ImageHSV.data;
  unsigned char* dataNorm = ImageHSVNorm.data;

  unsigned bins[mBinCount];
  memset(bins, 0, mBinCount*sizeof(unsigned));
  unsigned max = mBinCount-1;
  unsigned min = 0;

  for (int i = 0; i < ImageHSV.rows*ImageHSV.cols; i++)
  {
    bins[data[3*i+2]]++;
  }

  // Maximum number of pixels to remove from the histogram
  // This is calculated by taking a percentage of the total number of pixels
  const double clipFraction = clipPercent/100.0f;
  unsigned pixelsToClip = cvRound(clipFraction*(double)(ImageHSV.rows*ImageHSV.cols));
  unsigned pixelsToClipHalf = cvRound((double)pixelsToClip/2);
  unsigned binSum = 0;

  // Find the lower pixel bound
  if (bins[0] < pixelsToClipHalf)
  {
    binSum = bins[0];
    for (unsigned i = 1; i < 255; i++)
    {
      binSum = binSum + bins[i];
      if (binSum > pixelsToClipHalf)
      {
        min = i;
        break;
      }
    }
  }

  // Find the upper pixel bound
  if (bins[255] < pixelsToClipHalf)
  {
    binSum = bins[255];
    for (unsigned i = 255; i > 1; i--)
    {
      binSum = binSum + bins[i];
      if (binSum > pixelsToClipHalf)
      {
        max = i;
        break;
      }
    }
  }

  for ( int i = 0; i < ImageHSV.rows*ImageHSV.cols; i++)
  {
    dataNorm[3*i]   = data[3*i];// Hue
    dataNorm[3*i+1] = data[3*i+1];//Saturation

     // Value
    double newPixelDouble =
      (double)((double)data[3*i+2]-(double)min)*(255.0f/(double)(max-min));

    unsigned char newPixelByte;

    if (newPixelDouble > 255) // Take care of positive clipping
    {
      newPixelByte = 255;
    }
    else if(newPixelDouble < 0) // Take care of negative clipping
    {
      newPixelByte = 0;
    }
    else // If there is no clipping
    {
      newPixelByte = cvRound(newPixelDouble);
    }

    dataNorm[3*i+2] = newPixelByte;
  }
  cvtColor(ImageHSVNorm, ImageBGRNorm, CV_HSV2BGR);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHistLib::NormalizeImageBGR(const Mat& ImageBGR, Mat& ImageBGRNorm)
{
  Mat ImageHSV     = Mat(ImageBGR.size(), CV_8UC3);
  Mat ImageHSVNorm = Mat(ImageBGR.size(), CV_8UC3);

  cvtColor(ImageBGR, ImageHSV, CV_BGR2HSV);

  unsigned char* data     = ImageHSV.data;
  unsigned char* dataNorm = ImageHSVNorm.data;

  // The normalization procedure here is performed with efficiency in mind
  // Find min/max from the value channel
  unsigned char min = 255;
  unsigned char max = 0;
  for ( int i = 0; i < ImageHSV.rows*ImageHSV.cols; i++)
  {
    if( data[3*i+2] > max) max = data[3*i+2];
    if( data[3*i+2] < min) min = data[3*i+2];
  }

  for ( int i = 0; i < ImageHSV.rows*ImageHSV.cols; i++)
  {
    dataNorm[3*i]   = data[3*i];// Hue
    dataNorm[3*i+1] = data[3*i+1];//Saturation

    double newPixelDouble =
      (double)((double)data[3*i+2]-(double)min)*(255.0f/(double)(max-min));

    unsigned char newPixelByte;

    if (newPixelDouble > 255) // Take care of positive clipping
    {
      newPixelByte = 255;
    }
    else if(newPixelDouble < 0) // Take care of negative clipping
    {
      newPixelByte = 0;
    }
    else // If there is no clipping
    {
      newPixelByte = cvRound(newPixelDouble);
    }

     // Value
    dataNorm[3*i+2] = newPixelByte;

  }
  cvtColor(ImageHSVNorm, ImageBGRNorm, CV_HSV2BGR);
}
