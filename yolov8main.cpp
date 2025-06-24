// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

//modified 1-14-2023 Q-engineering

#include "yoloV8.h"
#include "json.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fcgi_stdio.h>
#include <iostream>
#include <stdio.h>
#include <vector>

using json = nlohmann::json;

YoloV8 yolov8;
int target_size = 640; //416; //320;  must be divisible by 32.

int main()
{
    std::ios::sync_with_stdio(false); // cout/printf 버퍼 동기화 방지

    yolov8.load(target_size); // 모델 한 번만 로드

    while (FCGI_Accept() >= 0) {
        std::cout << "Content-Type: application/json\r\n\r\n";

        cv::VideoCapture cap("/dev/video0", cv::CAP_V4L2); // 카메라 장치 열기
        if (!cap.isOpened()) {
            json err = { {"error", "Failed to open camera (/dev/video0)"} };
            std::cout << err.dump(4) << std::endl;
            continue;
        }

        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            json err = { {"error", "Captured frame is empty"} };
            std::cout << err.dump(4) << std::endl;
            continue;
        }

        std::vector<Object> objects;
        yolov8.detect(frame, objects);

        json response;
        response["detected_count"] = objects.size();
        for (size_t i = 0; i < objects.size(); ++i) {
            const Object& obj = objects[i];
            response["objects"].push_back({
                {"id", i},
                {"label", obj.label},
                {"prob", obj.prob},
                {"bbox", {
                    {"x", obj.rect.x},
                    {"y", obj.rect.y},
                    {"width", obj.rect.width},
                    {"height", obj.rect.height}
                }}
            });
        }

        std::cout << response.dump(4) << std::endl;
    }

    return 0;
}

