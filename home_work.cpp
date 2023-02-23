#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
using namespace cv;

#define KEY_ESC 27

using Pixel = cv::Vec<uchar, 3>; // G,B,R

uchar gray(Pixel p) {
    return 0.3*p[2] + 0.59*p[1] + 0.11*p[0];
}

void info(void) {
    std::cout << "EML 6805 Robot Design" << std::endl
              << "Florida International University" << std::endl
              << "Department of Mechanical and Materials Engineering" << std::endl
              << "Philip Moore <pmoor027@fiu.edu>" << std::endl
              << "Miami, Spring 2023" << std::endl
              << std::endl;
}

void plotRow(cv::Mat &image, std::vector<uchar> row_colors, int y, float scale, cv::Scalar color) {
    std::vector<cv::Point> points;
    for (int x = 0; x < row_colors.size(); ++x)
        points.push_back( cv::Point(x, y - scale*row_colors[x]) );
    cv::polylines(image, points, false, color, 2);
}

// int main(int argc, char **argv) {
//     info();
//     // Create a capture object from device number (or video filename)
//     cv::VideoCapture cap(0);
//     // Check if any error happened
//     if(!cap.isOpened()) {
//         std::cout << "Ops, capture cannot be created!" << std::endl;
//         return -1;
//     }
//     std::cout << "Press 'Esc' to quit..." << std::endl;
//     while(true) {
//         // Create an image object 
//         cv::Mat image;
//         // Capture frame
//         bool okay = cap.read( image );
//         // Skip if any capture error happened (or video ended)
//         if(okay) 
//             cv::imshow("Image", image);
//         // Break loop if the key 'ESC' was pressed
//         if(cv::waitKey(30) == KEY_ESC)
//             break;
//     }
//     return 0;
// }


int main(int argc, char** argv) {
    
    // Load image from command line arguments
    std::string filename;
    if (argc >= 2)
        filename = argv[argc-1];
    else
        filename = "../baboon.jpg";
    // Load image from file
    const cv::Mat image = cv::imread(filename);
    if (image.empty()) {
        std::cout << "Image '" << filename << "' not found!" << std::endl;
        return -1;
    }

    cv::Mat line_hsv;
    cvtColor(image, line_hsv, cv::COLOR_BGR2HSV);

    // Check the if image type is compatible with CV_8UC3
    std::string type = cv::typeToString( image.type() );
    std::cout << type << std::endl
              << image.rows << "x" << image.cols
              << std::endl;
    if (type != "CV_8UC3") {
        std::cout << "Ops, format '" << type << "' not supported!" << std::endl;
        return 1;
    }
    // Show image
    // cv::imshow("EML4840", image);
    // Create windows with trackbar
    cv::namedWindow("EML4840");
    // Trackers
    int track_row = 70;     // Percentage
    int track_scale = 40;   // Percentage
    int track_resize = 100;
    cv::createTrackbar("Row", "EML4840", &track_row, 100);
    cv::createTrackbar("Scale", "EML4840", &track_scale, 100);
    cv::createTrackbar("resize", "EML4840", &track_resize, 100);
    // Menu
    bool show_red = true;
    bool show_blue = true;
    bool show_green = true;
    bool show_gray = true;
    std::cout << "Press:" << std::endl
              << "s            : to save image" << std::endl
              << "r, g, b, or k: to show colors" << std::endl
              << "q or ESC     : to quit" << std::endl;
    // Create vectors to store the graphs
    std::vector<uchar> r, g, b, k;
    std::vector<uchar> h, s, v;
    // Run until 'q' is pressed...
    bool running = true;
    while( running ) {
        // Clear colors
        r.clear();
        g.clear();
        b.clear();
        k.clear();
        h.clear();
        s.clear();
        v.clear();
        // Update scale
        float scale = 0.01*track_scale;
        // Pixel scanine
        int y = 0.01*track_row*(image.rows-1);
        for (int x = 0; x < image.cols; ++x ) {
            Pixel pixel = image.at<Pixel>( cv::Point(x, y) );
            r.push_back( pixel[2] );
            g.push_back( pixel[1] );
            b.push_back( pixel[0] );
            k.push_back( gray(pixel) );

        }
        if (show_red)   plotRow(image, r, y, scale, cv::Scalar(0,0,255));
        if (show_green) plotRow(image, g, y, scale, cv::Scalar(0,255,0));
        if (show_blue)  plotRow(image, b, y, scale, cv::Scalar(255,0,0));
        if (show_gray)  plotRow(image, k, y, scale, cv::Scalar(0,0,0));
        cv::line(canvas, cv::Point(0, y), cv::Point(image.cols, y), cv::Scalar(0,0,0), 2);
        // Menu
        int key = cv::waitKey(10);
        switch(key) {
        case 's':
            cv::imwrite("../output.jpg", image);
            break;
        case 'q':
        case KEY_ESC:
            running = false;
            break;
        case 'r':
            show_red = !show_red;
            break;
        case 'g':
            show_green = !show_green;
            break;
        case 'b':
            show_blue = !show_blue;
            break;
        case 'k':
            show_gray = !show_gray;
            break;
        }
        // Show image
        cv::resize(canvas, canvas, cv::Size(), 0.01*track_resize, 0.01*track_resize);
        cv::imshow("EML4840", canvas);
    }
    // Close all windows
    cv::destroyAllWindows();
    return 0;
}

