# 소스 파일 목록
SRCS = yoloV8.cpp yolov8main.cpp

# 헤더/인클루드 경로
INCLUDES = -I/usr/local/include/opencv4 -I/usr/local/include/ncnn -I.

# ncnn 정적 라이브러리
NCNN_LIB = /usr/local/lib/ncnn/libncnn.a

# OpenCV 옵션 (pkg-config 사용)
OPENCV_FLAGS = `pkg-config --libs --cflags opencv4`

# 공통 컴파일 옵션
CXXFLAGS = -Wall -Wno-unknown-pragmas -fPIE -fopenmp -pthread -DNDEBUG $(INCLUDES)
LDFLAGS = -fopenmp -ldl -lpthread -lgomp -rdynamic $(NCNN_LIB) $(OPENCV_FLAGS) -lfcgi

# 타겟 명칭 및 디렉토리
RELEASE_TARGET = bin/Release/YoloV8
DEBUG_TARGET = bin/Debug/YoloV8
CGI_TARGET = bin/Release/yolov8_ncnn.cgi

# 디렉토리 생성
$(shell mkdir -p bin/Release bin/Debug)

# Release 빌드
release: $(SRCS)
	g++ -O3 $(CXXFLAGS) $^ -o $(RELEASE_TARGET) $(LDFLAGS)

# CGI 빌드
cgi: $(SRCS)
	g++ -O3 $(CXXFLAGS) $^ -o $(CGI_TARGET) $(LDFLAGS)

# Debug 빌드
debug: $(SRCS)
	g++ -g $(CXXFLAGS) $^ -o $(DEBUG_TARGET) $(LDFLAGS)

install: cgi
	# CGI 파일을 /usr/lib/cgi-bin/ 디렉토리에 복사하고 실행
	cp $(CGI_TARGET) /usr/lib/cgi-bin/yolov8_ncnn.cgi
	cp yolov8n.param yolov8n.bin /usr/lib/cgi-bin/
	sudo chmod +x /usr/lib/cgi-bin/yolov8_ncnn.cgi

# clean 타겟
clean:
	rm -f bin/Release/YoloV8 bin/Debug/YoloV8

.PHONY: release debug clean
