
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace cv;
using namespace std;

void mat_to_image(cv::Mat mat, float* buf)
{
    int w = mat.cols;
    int h = mat.rows;
    int c = mat.channels();
    unsigned char *data = (unsigned char *)mat.data;
    int step = mat.step;
    for (int y = 0; y < h; ++y) {
        for (int k = 0; k < c; ++k) {
            for (int x = 0; x < w; ++x) {
                buf[k*w*h + y*w + x] = data[y*step + x*c + k] / 255.0f;
            }
        }
    }
}

int main(int argc, char** argv) {
    ifstream in;

    if (argc != 2) {
        cout << "usage: test_conv <Image_Path>" << endl;;
        return -1;
    }

    string image_dir = SOURCE_DIR + string("/images");
    
    in.open(argv[1], ifstream::ate|ifstream::binary);
    if (!in.is_open()) {
        cout << "failed to open file" << endl;
        return -1;
    }
    int size = in.tellg();
    char* buf = new char[size];
    in.seekg(ios_base::beg);
    in.read(buf, size);

    Mat image_raw(416, 416, CV_8UC3, buf);

    auto start = std::chrono::system_clock::now();
    float* buf_float = new float[size];
    
    mat_to_image(image_raw, buf_float);
    auto end = chrono::system_clock::now();
    std::chrono::duration<double> dur = end - start;
    cout << "color+type duration: " << dur.count() << endl;

    start = std::chrono::system_clock::now();
    Mat large(2048,2560,CV_8UC3);
    Mat out(416,416,CV_8UC3);
    resize(large,out, Size(416,416));
    end = chrono::system_clock::now();
    dur = end - start;
    cout << "resize duration: " << dur.count() << endl;

    imshow("test", image_raw);
    waitKey(0);

    delete buf_float;
    delete buf;
}
