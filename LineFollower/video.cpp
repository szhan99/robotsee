#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#define KEY_ESC 27

using Pixel = cv::Vec<uchar, 3>; // G,B,R

uchar gray(Pixel p) {
    return 0.3*p[2] + 0.59*p[1] + 0.11*p[0];
}

int point1 = 0;
int point2 = 0;
int pointmiddle = 0;
int threshold = 200;

void info(void) {
    std::cout << "EML 6805 Robot Design" << std::endl
              << "Florida International University" << std::endl
              << "Department of Mechanical and Materials Engineering" << std::endl
              << "Susan Zhang <szhan040@fiu.edu>" << std::endl
              << "Miami, Spring 2023" << std::endl
              << std::endl;
}

void plotRow(cv::Mat &image, std::vector<uchar> row_colors, int y, float scale, cv::Scalar color) {
    std::vector<cv::Point> points;
    for (int x = 0; x < row_colors.size(); ++x)
        points.push_back( cv::Point(x, y - scale*row_colors[x]) );
    cv::polylines(image, points, false, color, 2);
}

int main(int argc, char** argv) {
    info();
    //Create a capture object from device number (or video filename)
    cv::VideoCapture cap("../line.mp4");
    // Check if any error happened
    if(!cap.isOpened()) {
        std::cout << "Oops, capture cannot be created!" << std::endl;
        return -1;
    }

    std::cout << "Press 'ESC' to quit.. " << std::endl;

    // Create windows with trackbar
    cv::namedWindow("EML6805");
    // Trackers
    int track_row = 70;     // Percentage
    int track_scale = 40;   // Percentage
    int track_resize = 100;
    cv::createTrackbar("Row", "EML6805", &track_row, 100);
    cv::createTrackbar("Scale", "EML6805", &track_scale, 100);
    cv::createTrackbar("resize", "EML6805", &track_resize, 100);
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
    // Run until 'q' is pressed...
    bool running = true;

        while( running ) {
        // Create an image object
            cv::Mat image;
            // Capture frame
            bool okay = cap.read(image);
            // Skip if any capture error happened (or video ended)
            std::string type = cv::typeToString(image.type());
            if (type != "CV_8UC3") {
                std::cout << "Ops, format '" << type << "' not supported!" << std::endl;
                return 1;
        } 
            // if(okay)
            //     cv::imshow("Image", image);
        // Break loop if the key "ESC" was pressed
            if (cv::waitKey(30) == KEY_ESC)
                break;

        // Clear colors
            r.clear();
            g.clear();
            b.clear();
            k.clear();
            // Update scale
            float scale = 0.01*track_scale;

            // Pixel scanine
            bool flag1 = false;
            bool flag2 = false;

            int y = 0.01*track_row*(image.rows-1);

        for (int x = 0; x < image.cols; ++x ) {
            Pixel pixel = image.at<Pixel>( cv::Point(x, y) );
            r.push_back( pixel[2] );
            g.push_back( pixel[1] );
            b.push_back( pixel[0] );
            k.push_back( gray(pixel) );

        // Marks one end of the tape "certain blue threshold with a circle"
            if (pixel[0] > threshold && pixel[1] < threshold && pixel[2] < threshold && !flag1)
            {
                cv::circle(image, cv::Point(x, y), 5, cv::Scalar(255, 255, 255), 1);
                flag1 = true;
                flag2 = false;

                point1 = x;
            }

            if(pixel[0] < threshold && flag1 && !flag2)
            {
                cv::circle(image, cv::Point(x, y), 5, cv::Scalar(255, 255, 255), 1);
                flag2 = true;
                flag1 = true;

                point2 = x;
            }

            pointmiddle = (point1 + point2)/ 2;

            cv::Point pb(pointmiddle, y-10);
            cv::Point pt(pointmiddle, y+10);

            cv::line(image, pb, pt, cv::Scalar(255, 255, 255), 1, 8, 0);
        }

            // clone image and keep the orginal for processing!
        cv::Mat canvas = image.clone();
        if (show_red)   plotRow(canvas, r, y, scale, cv::Scalar(0,0,255));
        if (show_green) plotRow(canvas, g, y, scale, cv::Scalar(0,255,0));
        if (show_blue)  plotRow(canvas, b, y, scale, cv::Scalar(255,0,0));
        if (show_gray)  plotRow(canvas, k, y, scale, cv::Scalar(0,0,0));
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
        cv::imshow("EML6805", canvas);

    }
    // Close all windows
    cv::destroyAllWindows();
    return 0;
}

