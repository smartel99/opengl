#include "PracticalSocket.h"
#include <iostream>
#include <cstdlib>

using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

#include <librealsense2/rs.hpp>

int main(int argc, char* argv[]) {
	if ((argc < 3) || (argc > 3)) {
		// Test for correct number of arguments.
		argv[1] = (char*)"127.0.0.1";
		argv[2] = (char*)"10000";
	}

	string servAddress = argv[1];	// First arg: server address.
	unsigned short servPort = Socket::resolveService(argv[2], "udp");

	try {
		UDPSocket sock;
		int jpgegqual = ENCODE_QUALITY;	// Compression Parameter.

		vector<uchar> encoded;

		rs2::pipeline pipe;
		rs2::config cfg;

		cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
		pipe.start(cfg);

		rs2::frameset frames;
		for (int i = 0; i < 30; i++) {
			frames = pipe.wait_for_frames();
		}

		//namedWindow("send", WINDOW_AUTOSIZE);
		clock_t last_cycle = clock();

		while (waitKey(1)==-1) {
			frames = pipe.wait_for_frames();
			rs2::frame color_frame = frames.get_color_frame();

			// Creating OpenCV Matrix from a color image.
			Mat send(Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);
			vector<int> compression_params;
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpgegqual);

			imencode(".jpg", send, encoded, compression_params);
			//imshow("send", send);
			int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

			int ibuf[1];
			ibuf[0] = total_pack;
			sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

			for (int i = 0; i < total_pack; i++) {
				sock.sendTo(&encoded[double(i * PACK_SIZE)], PACK_SIZE, servAddress, servPort);
			}

			clock_t next_cycle = clock();
			double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
			cout << "\teffective FPS:" << (1 / duration) <<
				" \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << "         \r";
			last_cycle = next_cycle;
		}
	}
	catch (SocketException & e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}