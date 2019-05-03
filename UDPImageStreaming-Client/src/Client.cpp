#include "PracticalSocket.h"
#include <iostream>
#include <cstdlib>

using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

#define BUF_LEN 65540	// Larger than maximum UDP packet size.
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes

#include <librealsense2/rs.hpp>


Mat CannyThreshold(Mat src) {
	Mat detected_edges, src_gray, dst;
	int lowThreshold = 20;
	int ratio = 3;
	int kernel_size = 3;

	/// Convert to gray scale.
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	/// Reduce noise with a kernel 3x3.
	blur(src_gray, detected_edges, Size(3, 3));
	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);

	src.copyTo(dst, detected_edges);
	return dst;
}

int main(int argc, char* argv[]) {
	if ((argc < 2) || (argc > 2)) {
		// Test for correct number of arguments.
		argv[1] = (char*)"10000";
	}

	unsigned short servPort = atoi(argv[1]); // First arg:  local port
	
	namedWindow("recv", WINDOW_AUTOSIZE);
	try {
		UDPSocket sock(servPort);

		char buffer[BUF_LEN]; // Buffer for echo string
		int recvMsgSize; // Size of received message
		string sourceAddress; // Address of datagram source
		unsigned short sourcePort; // Port of datagram source

		clock_t last_cycle = clock();

		while (waitKey(1)==-1) {
			// Block until receive message from a client
			do {
				recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
			} while (recvMsgSize > sizeof(int));
			int total_pack = ((int*)buffer)[0];

			//cout << "expecting length of packs:" << total_pack << endl;
			char* longbuf = new char[double(PACK_SIZE * total_pack)];
			for (int i = 0; i < total_pack; i++) {
				recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
				if (recvMsgSize != PACK_SIZE) {
					//cerr << "Received unexpected size pack:" << recvMsgSize << endl;
					continue;
				}
				memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
			}

			//cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;

			Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			Mat frame = imdecode(rawData, IMREAD_COLOR);
			if (frame.size().width == 0) {
				//cerr << "decode failure!" << endl;
				continue;
			}
			delete[] longbuf;
			Mat canny = CannyThreshold(frame);
			imshow("recv", frame);

			clock_t next_cycle = clock();
			double duration = (double(next_cycle - last_cycle)) / (double)CLOCKS_PER_SEC;
			cout << "\teffective FPS:" << (1 / duration) << 
				" \tkbps:" << (double(PACK_SIZE * total_pack / duration / 1024 * 8)) << "       \r";

			last_cycle = next_cycle;
		}
	}
	catch (SocketException & e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}