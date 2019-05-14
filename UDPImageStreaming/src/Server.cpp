/**
 * OpenCV video streaming over TCP/IP
 * Server: Captures video from a D435 camera and send it to a client.
 * Modified from: https://gist.github.com/djalmabright/490423efd107a03f48122c50b725e936
 */

#include "opencv2/opencv.hpp"
#include "librealsense2/rs.hpp"
#include <iostream>

#ifdef _WIN64
#include <winsock.h>         // For socket(), connect(), send(), and recv()
#include <windows.h>
typedef int socklen_t;
typedef char raw_type;       // Type used for raw data on this platform
#else
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
typedef void raw_type;       // Type used for raw data on this platform
#endif

#include <string.h>

using namespace cv;

rs2::pipeline g_pipe;

#ifdef _WIN64
DWORD WINAPI display(LPVOID lpParameter);
#else
void* display(void*);
#endif


int main(int argc, char** argv)
{
	//-------------------------------------------------------------------------
	// Networking stuff: socket, bind, listen.
	//-------------------------------------------------------------------------
	int localSocket,
		remoteSocket,
		port = 4097;

	struct sockaddr_in localAddr,
		remoteAddr;
#ifdef _WIN64
	DWORD thread_id;
#else
	pthread_t thread_id;
#endif // _WIN64

	int addrLen = sizeof(struct sockaddr_in);

	if ((argc > 1) && (strcmp(argv[1], "-h") == 0)) {
		std::cerr << "usage: ./UDPImageStreaming-Server [port]\n" <<
			"port: socket port (default: 4097)" << std::endl;
		exit(1);
	}

	if (argc == 2) port = atoi(argv[1]);

	localSocket - socket(AF_INET, SOCK_STREAM, 0);
	if (localSocket == -1) {
		perror("socket() call failed!!");
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = htons(port);

	if (bind(localSocket, (struct sockaddr*) & localAddr, sizeof(localAddr)) < 0) {
		perror("Can't bind() socket");
		exit(1);
	}

	// Listening.
	listen(localSocket, 3);

	std::cout << "Waiting for connections...\n"
		<< "Server Port: " << port << std::endl;

	// Accept connection from an incoming client.
	while (1) {
		remoteSocket = accept(localSocket, (struct sockaddr*) & remoteAddr, (socklen_t*)& addrLen);

		if (remoteSocket < 0) {
			perror("Accept failed!");
			exit(1);
		}

		std::cout << "Connection accepted" << std::endl;
#ifdef _WIN64
		HANDLE myHandle = CreateThread(0, 0, display, &remoteSocket, 0, &thread_id);
#else
		pthread_create(&thread_id, NULL, display, &remoteSocket);
#endif
	}
	return 0;
}


#ifdef _WIN64
DWORD WINAPI display(LPVOID ptr) {
#else
void* display(void* ptr) {
#endif
	int socket = *(int*)ptr;
	//-------------------------------------------------------------------------

	Mat img, imgGray;
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
	g_pipe.start(cfg);
	rs2::frameset frames;
	for (int i = 0; i < 30; i++) {
		frames = g_pipe.wait_for_frames();
	}

	img = Mat::zeros(480, 640, CV_8UC1);
	int imgSize = img.total() * img.elemSize();
	int bytes = 0;
	int key;

	// Make img continuous.
	if (!img.isContinuous()) {
		img = img.clone();
		imgGray = img.clone();
	}
	std::cout << "Image Size: " << imgSize << std::endl;

	while (1) {
		/* Get a frame from camera */
		frames = g_pipe.wait_for_frames();
		
		img = Mat(Size(640, 480), CV_8UC3, (void*)frames.get_color_frame().get_data(), Mat::AUTO_STEP);

		/* Do video processing here */
		cvtColor(img, imgGray, COLOR_BGR2GRAY);

		// Send processed image.
		if ((bytes = send(socket, (const char*)imgGray.data, imgSize, 0)) < 0) {
			std::cerr << "bytes = " << bytes << std::endl;
			break;
		}
	}
}
