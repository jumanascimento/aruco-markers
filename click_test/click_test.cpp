#include <opencv2/opencv.hpp>
#include <iostream>

cv::Rect2d bbox;
bool drawing_box = false;

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    cv::Mat* frame = static_cast<cv::Mat*>(userdata);

    if (event == cv::EVENT_LBUTTONDOWN) {
        drawing_box = true;
        bbox = cv::Rect2d(x, y, 0, 0);
    } else if (event == cv::EVENT_LBUTTONUP) {
        drawing_box = false;
        cv::rectangle(*frame, bbox, cv::Scalar(0, 255, 0), 2);
        cv::imshow("Image", *frame);
    } else if (event == cv::EVENT_MOUSEMOVE && drawing_box) {
        bbox.width = x - bbox.x;
        bbox.height = y - bbox.y;
        cv::Mat frame_copy = frame->clone();
        cv::rectangle(frame_copy, bbox, cv::Scalar(0, 255, 0), 2);
        cv::imshow("Image", frame_copy);
    }
}

int main() {
    cv::Mat image = cv::imread("/home/julia/Documents/click_test/image/tagtest.png");
    if (image.empty()) {
        std::cerr << "Failed to load image." << std::endl;
        return 1;
    }

    cv::namedWindow("Image");
    cv::setMouseCallback("Image", mouseCallback, &image);

    while (true) {
        cv::imshow("Image", image);
        char key = cv::waitKey(1);
        if (key == 27)  // ESC key
            break;
    }

    cv::destroyAllWindows();

    return 0;
}


