#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <iomanip>

// Função para calcular a distância
float calculateDistance(float focal_length_px, float object_height_mm, float object_height_px, float marker_height_from_camera_m, float camera_height_from_ground_m) {
    float distance_to_camera_m = marker_height_from_camera_m - camera_height_from_ground_m;
    return (focal_length_px * object_height_mm * camera_height_from_ground_m) / (object_height_px * 1000.0 * distance_to_camera_m);
}

int main() {

    // Definir os parâmetros da câmera
    float focal_length_px = 695.802000; // Distância focal em pixels
    float object_height_mm = 103.0; // Altura do objeto em milímetros
    float camera_height_from_ground_m = 0.75; // Altura da câmera em relação ao chão (em metros)

    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 695.802000, 0.000000, 328.281893,
                                                        0.000000, 696.308095, 233.812776,
                                                        0.000000, 0.000000, 1.000000);

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(16));

    cv::VideoCapture in_video(2);
    if (!in_video.isOpened()) {
        std::cerr << "failed to open video input" << std::endl;
        return 1;
    }

    cv::Mat image, image_copy;

    int wait_time = 10; // Wait for 10 milliseconds between frames
    
    while (in_video.grab()) {
        in_video.retrieve(image);
        image.copyTo(image_copy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        for (size_t i = 0; i < ids.size(); ++i) {
            // Calculate distance to the ArUco using the height of the ArUco from the camera and camera height from the ground
            float marker_height_from_camera_m = 1.10; // Altura da ArUco em relação à câmera (em metros)
            float object_height_px = corners[i][0].y - corners[i][2].y; // Assuming vertical markers
            float distance = calculateDistance(focal_length_px, object_height_mm, object_height_px, marker_height_from_camera_m, camera_height_from_ground_m);

            std::cout << "Distance to ArUco " << ids[i] << ": " << distance << " m" << std::endl;
        }

        cv::imshow("Pose estimation", image_copy);
        char key = (char)cv::waitKey(wait_time);
        if (key == 27)
            break;
    }

    in_video.release();

    return 0;
}

