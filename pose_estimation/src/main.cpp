#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <iomanip>

// Função para calcular a distância
float calculateDistance(float focal_length_px, float object_height_mm, int image_height_px, float object_height_px, float sensor_height_mm) {
    float distance_m = (focal_length_px * object_height_mm * image_height_px) / (object_height_px * sensor_height_mm);
    return distance_m / 1000.0; // Converter milímetros para metros
}

// Função para calcular o erro de distância
float calculateDistanceError(float actual_distance, float measured_distance) {
    return std::abs(actual_distance - measured_distance);
}

int main() {

    // Definir os parâmetros da câmera
    float focal_length_px = 695.802000; // Distância focal em pixels
    float sensor_height_mm = 152.4; // Altura do sensor em milímetros
    float object_height_mm = 103.0; // Altura do objeto em milímetros
    int image_height_px = 480; // Altura da imagem em pixels
    
    int max_distance_aruco_id = 7;
    float max_distance_from_camera_m = 3.0; // 3 metros
    float max_distance_from_ground_m = 1.40; // 1,40 metros
     float marker_length_m = 0.102;


    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 695.802000, 0.000000, 328.281893,
                                                        0.000000, 696.308095, 233.812776,
                                                        0.000000, 0.000000, 1.000000);

    cv::Mat dist_coeffs = (cv::Mat_<double>(1, 5) << 0.072282, -0.037522, 0.001242, -0.004470, 0.000000);

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(16));

    std::cout << "camera_matrix\n" << camera_matrix << std::endl;
    std::cout << "\ndist_coeffs\n" << dist_coeffs << std::endl;

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

        // If at least one marker detected
        if (ids.size() > 0) {
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, marker_length_m,
                camera_matrix, dist_coeffs, rvecs, tvecs);

             int max_distance_aruco_index = -1;
            for (size_t i = 0; i < ids.size(); ++i) {
                if (ids[i] == max_distance_aruco_id) {
                    max_distance_aruco_index = i;
                    break;
                }
            }

            if (max_distance_aruco_index != -1) {
                // Calculate distance to the ArUco of max distance
                float focal_length = camera_matrix.at<double>(0, 0); // Obtém o valor da matriz de câmera (focal_length)
                float object_height_mm = max_distance_from_ground_m * 1000.0; // Converter metros para milímetros
                float sensor_height_mm = max_distance_from_camera_m * 1000.0; // Converter metros para milímetros

                // Get the bounding rectangle of the marker
                cv::Rect marker_rect = cv::boundingRect(corners[max_distance_aruco_index]);
                float object_height_px = static_cast<float>(marker_rect.height);

                // Convert object height to meters
                float object_height_m = object_height_mm / 1000.0;

                // Calculate image height in meters using the scale factor from the marker size
                float image_height_m = object_height_m * image.rows / object_height_px;

                float distance = calculateDistance(focal_length, object_height_m, image_height_m, object_height_px, sensor_height_mm / 1000.0);

                std::cout << "Distance to ArUco " << max_distance_aruco_id << ": " << distance << " m" << std::endl;

                // Calculate distance error (example using actual_distance = max_distance_from_camera_m)
                float actual_distance = max_distance_from_camera_m;
                float distance_error = calculateDistanceError(actual_distance, distance);
                std::cout << "Distance Error: " << distance_error << " m" << std::endl;
            }

                    

            // Draw axis for each marker
            for (int i = 0; i < ids.size(); i++) {
                cv::aruco::drawAxis(image_copy, camera_matrix, dist_coeffs, rvecs[i], tvecs[i], 0.1);

                // Esta seção irá imprimir os dados para todos os marcadores detectados.
                // Se você tiver mais de um marcador, é recomendado modificar esta seção
                // para imprimir os dados de um marcador específico ou imprimir os dados
                // de cada marcador separadamente.
                std::ostringstream vector_to_marker;

                vector_to_marker << std::fixed << std::setprecision(4) << "x: " << std::setw(8) << tvecs[0](0);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, cv::LINE_AA);

                vector_to_marker.str(std::string());
                vector_to_marker << std::fixed << std::setprecision(4) << "y: " << std::setw(8) << tvecs[0](1);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, cv::LINE_AA);

                vector_to_marker.str(std::string());
                vector_to_marker << std::fixed << std::setprecision(4) << "z: " << std::setw(8) << tvecs[0](2);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, cv::LINE_AA);
            }
        }

        cv::imshow("Pose estimation", image_copy);
        char key = (char)cv::waitKey(wait_time);
        if (key == 27)
            break;
    }

    in_video.release();

    return 0;
}

