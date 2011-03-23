/*
  Convert a raw bayer image from mt9p031 at full size into a rgb (bgr)
  image for display and also save a jpg version. 10 -> 8 bit conversion
  is necessary to use the built-in cvCvtColor(). The implementation of
  that algo is standard though and 10 bit could be conserved. This is
  just a quick program for testing.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CV_NO_BACKWARD_COMPATIBILITY
#include <cv.h>
#include <highgui.h>

#define IMAGE_WIDTH 2592
#define IMAGE_HEIGHT 1944

IplImage *load_raw_image(const char *s);
char *load_raw_data(const char *s, int size);
IplImage *convert_image(IplImage *bayer);
void save_image(IplImage *img, const char *orig_file);

int main(int argc, char **argv)
{
	IplImage *bayer, *rgb;

	if (argc < 2) {
		printf("Usage: %s <image file>\n", argv[0]);
		exit(1);
	}

	bayer = load_raw_image(argv[1]);

	if (!bayer) {
		printf("Failed to load raw image %s\n", argv[1]);
		exit(1);
	}

	rgb = convert_image(bayer);

	if (rgb) {
		cvNamedWindow("main", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("main", 10, 10);
		cvShowImage("main", rgb);
		cvWaitKey(0);
		cvDestroyWindow("main");	

		save_image(rgb, argv[1]);
	}
	
	cvReleaseImage(&bayer); 

	if (rgb)
		cvReleaseImage(&rgb);

	return 0;
}

void save_image(IplImage *img, const char *orig_file)
{
	char *p;
	char *s = strdup(orig_file);
	
	if (!s)
		return;

	p = strrchr(s, '.');

	if (!p)
		return;

	strcpy(p, ".jpg");

	cvSaveImage(s, img, 0);

	free(s);
}

IplImage *convert_image(IplImage *bayer)
{
	IplImage *dst, *src;
	
	src = cvCreateImage(cvGetSize(bayer), IPL_DEPTH_8U, 1);

	if (!src) {
		printf("cvCreateImage() - src - failed\n");
		return NULL;
	}

	cvConvertScale(bayer, src, 0.25, 0.0);

	dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	if (!dst) 
		printf("cvCreateImage - dst - failed\n");
	else {
		//cvCvtColor(src, dst, CV_BayerGB2RGB);
		//cvCvtColor(src, dst, CV_BayerBG2RGB);
		//cvCvtColor(src, dst, CV_BayerRG2RGB);
		// -works cvCvtColor(src, dst, CV_BayerGR2RGB);
		
		//cvCvtColor(src, dst, CV_BayerGR2BGR);
		//cvCvtColor(src, dst, CV_BayerRG2BGR);
		cvCvtColor(src, dst, CV_BayerGB2BGR);
	}

	cvReleaseImage(&src);

	return dst;
}

IplImage *load_raw_image(const char *s)
{
	IplImage *img;
	CvSize sz = { IMAGE_WIDTH, IMAGE_HEIGHT };

	img = cvCreateImage(sz, IPL_DEPTH_16U, 1);

	if (!img) {
		printf("cvCreateImage() failed\n");
		return NULL;
	}

	img->imageData = load_raw_data(s, sz.width * sz.height * 2);
	
	if (!img->imageData) {
		cvReleaseImage(&img);
		return NULL;
	}
	
	//img->imageData = img->imageDataOrigin;

	return img;
}

char *load_raw_data(const char *s, int size)
{
	int fd, len;
	char *dat = malloc(size);

	if (!dat) {
		perror("malloc");
		return NULL;
	}

	fd = open(s, O_RDONLY);

	if (fd < 0) {
		perror("open");
		free(dat);
		return NULL;
	}

	len = read(fd, dat, size);

	if (len != size) {
		printf("read tried %d got %d\n", size, len);
		free(dat);
		return NULL;
	}

	close(fd);

	return dat;
}


	/*
	cvNamedWindow("main", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("shapegen", 10, 10);
	cvShowImage("main", img);
	cvWaitKey(0);
	cvDestroyWindow("main");	
	*/
